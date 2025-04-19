#include "comun.h"

#include "config.h"
#include "util.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_sh1107.h"
#include "esp_log.h"

#include "esp_private/esp_clk.h"
#include "portmacro.h"
#include "stdint.h"

#define EXAMPLE_LCD_H_RES 128
#define EXAMPLE_LCD_V_RES 32 // CONFIG_EXAMPLE_SSD1306_HEIGHT

static const char *TAG = "TP";
static const char *TAGW = "warning";

/// prototipo de funciones y declaracion de variables privadas


lv_disp_t * disp = NULL;

lv_disp_t * getDisplay(void)
{
  return(disp);
}

void config_gpio_input_pull_up(gpio_input_t pin);
void config_gpio_output(gpio_output_t pin);
void delay_loco(uint32_t delay);
void example_lvgl_demo_ui(void); // lv_disp_t *disp);


// BaseType_t xGet_lvgl_disp(lv_disp_t *display);

/// declaracion de funciones publicas
void config_gpio(void) {

  config_gpio_input_pull_up(BTN_0);
  config_gpio_input_pull_up(BTN_1);
  config_gpio_input_pull_up(BTN_2);

  config_gpio_output(LED_VERDE);
  config_gpio_output(LED_ROJO);
}

void config_lcd(void) {
  /// Creo semaforo para acceder a cuenta del tiempo
  cuenta_tiempo_t * tiempo = getTiempo();

  tiempo->mutex = xSemaphoreCreateMutex();

  /// Vamos con el display
  ESP_LOGI(TAG, "Initialize I2C bus");
  i2c_master_bus_handle_t i2c_bus = NULL;
  i2c_master_bus_config_t bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .i2c_port = 0,   // I2C_BUS_PORT,
      .sda_io_num = 7, // EXAMPLE_PIN_NUM_SDA,
      .scl_io_num = 6, // EXAMPLE_PIN_NUM_SCL,
      .flags.enable_internal_pullup = true,
  };
  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

  ESP_LOGI(TAG, "Install panel IO");
  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_i2c_config_t io_config = {
      .dev_addr = 0x3C,             // puede ser 3D, EXAMPLE_I2C_HW_ADDR,
      .scl_speed_hz = (100 * 1000), // EXAMPLE_LCD_PIXEL_CLOCK_HZ,
      .control_phase_bytes = 1,     // According to SSD1306 datasheet
      .lcd_cmd_bits =
          8, // EXAMPLE_LCD_CMD_BITS,   // According to SSD1306 datasheet
      .lcd_param_bits =
          8, // EXAMPLE_LCD_PARAM_BITS, // According to SSD1306 datasheet
      // #if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
      .dc_bit_offset = 6, // According to SSD1306 datasheet
                          // #elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
      //       .dc_bit_offset = 0,                     // According to SH1107
      //       datasheet .flags =
      //       {
      //           .disable_control_phase = 1,
      //       }
      // #endif
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

  ESP_LOGI(TAG, "Install SSD1306 panel driver");
  esp_lcd_panel_handle_t panel_handle = NULL;
  esp_lcd_panel_dev_config_t panel_config = {
      .bits_per_pixel = 1,
      .reset_gpio_num = -1, // EXAMPLE_PIN_NUM_RST,
  };
  // #if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
  esp_lcd_panel_ssd1306_config_t ssd1306_config = {
      .height = 32, // EXAMPLE_LCD_V_RES,
  };
  panel_config.vendor_config = &ssd1306_config;
  ESP_ERROR_CHECK(
      esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
  // #elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
  //   ESP_ERROR_CHECK(esp_lcd_new_panel_sh1107(io_handle, &panel_config,
  //   &panel_handle));
  // #endif

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

  // #if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
  //   ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
  // #endif

  ESP_LOGI(TAG, "Initialize LVGL");
  const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
  lvgl_port_init(&lvgl_cfg);

  const lvgl_port_display_cfg_t disp_cfg = {.io_handle = io_handle,
                                            .panel_handle = panel_handle,
                                            .buffer_size = EXAMPLE_LCD_H_RES *
                                                           EXAMPLE_LCD_V_RES,
                                            .double_buffer = true,
                                            .hres = EXAMPLE_LCD_H_RES,
                                            .vres = EXAMPLE_LCD_V_RES,
                                            .monochrome = true,
                                            .rotation = {
                                                .swap_xy = false,
                                                .mirror_x = false,
                                                .mirror_y = false,
                                            }};

  /// Se crea el display
  disp = lvgl_port_add_disp(&disp_cfg);

  /* Rotation of the screen */
  // lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);

  ESP_LOGI(TAG, "Display LVGL Text");

  if (lvgl_port_lock(0)) {
    lv_obj_clean(lv_scr_act());

    // example_lvgl_demo_ui(); // disp);
    vActualizarDisplay();

    // lvgl_boxes(disp);
    //  Release the mutex
    lvgl_port_unlock();
  }
}

BaseType_t xConfig_test(void) {
  // UINT64_MAX
  gpio_set_level(LED_ROJO, 1);
  delay_loco(2);
  gpio_set_level(LED_ROJO, 0);
  gpio_set_level(LED_VERDE, 1);
  delay_loco(2);
  gpio_set_level(LED_VERDE, 0);

  return (1);
}

/// declaracion de funciones privadas
void delay_loco(uint32_t delay) {
  int cpu_freq = esp_clk_cpu_freq();
  int factor = 256 / delay;
  for (uint32_t i = 0; i <= ((cpu_freq) / (factor + 1)); i++)
    asm("nop");
}
void config_gpio_input_pull_up(gpio_input_t pin) {

  gpio_reset_pin(pin);
  gpio_set_direction(pin, GPIO_MODE_INPUT);
  gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

void config_gpio_output(gpio_output_t pin) {
  gpio_reset_pin(pin);
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void example_lvgl_demo_ui(void) { // lv_disp_t *disp) {
  static bool flag = false;
  static lv_style_t style;
  // lv_style_init(&style);
  // lv_style_set_text_font(&style_mph, LV_STATE_DEFAULT, &arial_16);
  // lv_style_set_text_font(&style, LV_FONT_MONTSERRAT_32); // <--- you have to
  // enable other font sizes in menuconfig lv_style_set_text_font(&style,
  // LV_STATE_DEFAULT, &Arial_75); lv_style_set_text_font(&style,
  // LV_STATE_DEFAULT, &lv_font_unscii_16); lv_style_set_text_font(&style,
  // &lv_font_unscii_16); lv_style_set_text_font(&my_style,
  // &lv_font_montserrat_28);  /* Set a larger font */
  //lv_disp_t *disp_new = NULL;
  
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());
  lv_obj_t *label = lv_label_create(scr);

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_obj_add_style(label, &style, 0); // <--- obj is the label
  // lv_obj_add_style(label, &style, 0);  // <--- obj is the label

  lv_obj_t *label2 = lv_label_create(scr);
  lv_label_set_long_mode(label,
                         LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
                                              // /* Circular scroll */
  lv_label_set_long_mode(label2,
                         LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
                                              // /* Circular scroll */

  lv_label_set_text(label2, "20:20"); // Espressif, Hello LVGL.");
  cuenta_tiempo_t * tiempo = getTiempo(); 

  if (flag == true) {

    // lv_label_set_text(label, "18:00"); // Espressif, Hello LVGL.");
    lv_label_set_text_fmt(label, "%02d:%02d:%02d", tiempo->mm, tiempo->ss,
                          tiempo->dd);
    // lv_label_set_text(label, ":"); // Espressif, Hello LVGL.");
    flag = false;
    ESP_LOGI(TAG, "True");

  } else {
    // lv_label_set_text(label, ""); // Espressif, Hello LVGL.");

    // lv_label_set_text(label, "18 00"); // Espressif, Hello LVGL.");
    lv_label_set_text_fmt(label, "%02d:%02d:%02d", tiempo->mm, tiempo->ss,
                          tiempo->dd);

    flag = true;
    ESP_LOGI(TAG, "False");
  }

  /* Size of the screen (if you use rotation 90 or 270, please set
   * disp->driver->ver_res) */
  // lv_obj_set_width(label, disp->driver->hor_res);
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); //LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_align(label2, LV_ALIGN_BOTTOM_MID, 0, 0);
}
// BaseType_t xGet_lvgl_disp(lv_disp_t *display) {
//   display = disp;
//   return (true) ;
// }
// tiempo.horas = 0




/*
void tskContando(void *parametros) {

  /// manejo el contador
  TickType_t ultimo_evento;
  ultimo_evento = xTaskGetTickCount();

  for (;;) {

    if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(1))) {
      if (tiempo.dd >= 9) {
        tiempo.dd = 0;
        if (tiempo.ss >= 59) {
          tiempo.ss = 0;
          if (tiempo.mm >= 59) {
            tiempo.mm = 0;
            if (tiempo.hh >= 23) {
              tiempo.hh = 0;
              /// overflow del contador...
            } else {
              ++tiempo.hh;
            }
          } else {
            ++tiempo.mm;
          }
        } else {
          ++tiempo.ss;
        }
      } else {
        ++tiempo.dd;
      }
      if (lvgl_port_lock(0)) {
        lv_obj_clean(lv_scr_act());

        vActualizarDisplay();

        lvgl_port_unlock();
      }
      xSemaphoreGive(tiempo.mutex);
      //      ESP_LOGW(TAGW, "TOMO MUTEX");
    } else {
      ESP_LOGW(TAGW, "FALLO MUTEX");
    }
    vTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(100));
  }
}

void vActualizarDisplay(void) {
  static lv_style_t style;
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());
  lv_obj_t *label = lv_label_create(scr);

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_obj_add_style(label, &style, 0); // <--- obj is the label

  lv_label_set_long_mode(label,
                         LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_label_set_text_fmt(label, "%02d:%02d:%01d", tiempo.mm, tiempo.ss,
                        tiempo.dd);
}

*/