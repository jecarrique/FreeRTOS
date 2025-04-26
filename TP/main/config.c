#include "comun.h"

#include "config.h"
#include "util.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_sh1107.h"

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

  //if (lvgl_port_lock(0)) {
  //  lv_obj_clean(lv_scr_act());

    // example_lvgl_demo_ui(); // disp);
    vInitDisplay();

    vActualizarDisplay();

    // lvgl_boxes(disp);
    //  Release the mutex
  //  lvgl_port_unlock();
  //}
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
