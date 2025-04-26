#include "comun.h"

#include "display.h"

#include "clock.h"
#include "config.h"
#include "util.h"

#include "driver/gpio.h" // macros relacionadas a los pines
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include "soc/gpio_reg.h" // lectura directa a registro de gpio
#include <stdint.h>       // tipos de datos estandar

#define MASK_KEYS (1 << BTN_0) | (1 << BTN_1) | (1 << BTN_2)

static const char *TAG = "TP";
static const char *TAGW = "warning";

cuenta_tiempo_t tiempo = {0, 0, 0, 0};

cuenta_tiempo_t *getTiempo(void) { return (&tiempo); }

/// === funciones publicas
void tskKeys(void *parametres) {
#define FLANCO_ASCENDETEN(TECLA)                                               \
  (~keys_act & (1 << TECLA)) &&                                                \
      ((~keys_act & (1 << TECLA)) != (~keys_ant & (1 << TECLA)))

  uint32_t keys_act = 0;
  TaskHandle_t hndlTskCnt = xTaskGetHandle("CONTANDO");
  TaskHandle_t hndlTskBlink = xTaskGetHandle("BLINK");

  eTaskState tskState;
  for (;;) {
    uint32_t keys_ant = keys_act;
    keys_act = REG_READ(GPIO_IN_REG);
    keys_act &= MASK_KEYS;

    /// Flanco acendeten en BTN_1
    if (FLANCO_ASCENDETEN(BTN_1)) {
      ESP_LOGI("tskKeys", "flanco en btn_1");
      tskState = eTaskGetState(hndlTskCnt);
      if (tskState == eSuspended) {
        vTaskResume(hndlTskCnt);
        vTaskResume(hndlTskBlink);
        gpio_set_level(LED_ROJO, 0);

      } else {
        vTaskSuspend(hndlTskCnt);
        vTaskSuspend(hndlTskBlink);
        gpio_set_level(LED_VERDE, 0);
        gpio_set_level(LED_ROJO, 1);

        ESP_LOGI("tskSuspend", "flanco en btn_1");
      }

    } else {
      if (FLANCO_ASCENDETEN(BTN_0)) {
        ESP_LOGI("tskKeys", "flanco en btn_0");
        tskState = eTaskGetState(hndlTskCnt);
        if (tskState == eSuspended) {
          ESP_LOGI("tskSuspend", "flanco en btn_0");
          if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(1))) {
            tiempo.dd = 0;
            tiempo.ss = 0;
            tiempo.mm = 0;
            tiempo.hh = 0;
            xSemaphoreGive(tiempo.mutex);

            // if (lvgl_port_lock(0)) {
            //   lv_obj_clean(lv_scr_act());

            vActualizarDisplay();

            //  lvgl_port_unlock();
            //}
          }
        }
      }
    }
    uint32_t mascara_conversion = 1;
    if (keys_ant != keys_act) {
      char gpio_value[32];
      for (int i = 0; i < 32; i++) {

        // gpio_value[31-i] = ( (keys_act & 1<<i) != 0 ) ? '1' : '0';
        gpio_value[31 - i] = ((keys_act & mascara_conversion) != 0) ? '1' : '0';
        mascara_conversion <<= 1;
      }
      ESP_LOGI("", "Switches: %s", gpio_value);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void tskBlink(void *parametros) {
  TickType_t ultimo_evento;
  ultimo_evento = xTaskGetTickCount();

  for (;;) {
    gpio_set_level(LED_VERDE, 1);
    //      vTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(500));
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LED_VERDE, 0);
    //      vTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(500));
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void tskContando(void *parametros) {

  /// manejo el contador
  BaseType_t err;
  TickType_t ultimo_evento;
  ultimo_evento = xTaskGetTickCount();

  for (;;) {
    err = inc_time();
    if (err != 0) {
      ESP_LOGI("error", "fallo incremento de cuenta en el clock");
    }

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
      // if (lvgl_port_lock(0)) {
      //   lv_obj_clean(lv_scr_act());

      vActualizarDisplay();

      //  lvgl_port_unlock();
      //}
      xSemaphoreGive(tiempo.mutex);
      //      ESP_LOGW(TAGW, "TOMO MUTEX");
    } else {
      ESP_LOGW(TAGW, "FALLO MUTEX");
    }

    //      BaseType_t xWasDelayed = xTaskDelayUntil(&ultimo_evento,
    //      pdMS_TO_TICKS(100));

    //      if (xWasDelayed == pdFALSE)
    //        ESP_LOGW(TAGW, "FALLO vTaskDelayUntil");

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/// --- helper fuction
lv_obj_t *label;

void vInitDisplay(void) {
  // init_display();

  bool mutex = lvgl_port_lock(100);
  if (mutex) {
    lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());
    static lv_style_t style;

    lv_obj_t *label_tmp = lv_label_create(scr);
    label = label_tmp;

    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_unscii_16);
    lv_obj_add_style(label, &style, 0); // <--- obj is the label

    lv_label_set_long_mode(
        label,
        LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
    lvgl_port_unlock();
  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }
}

void vActualizarDisplay(void) {
  //update_display();

  static bool flag = true;

  if (flag) {
 //   vInitDisplay();
    flag = false;
  }
  bool mutex = lvgl_port_lock(0);
  if (mutex) {
    // lv_obj_clean(lv_scr_act());

  
    //    lv_label_set_text(label, "--:--:-");
    lv_label_set_text_fmt(label, "%02d:%02d:%01d", tiempo.mm, tiempo.ss,
                          tiempo.dd);
    //  Release the mutex
    // lv_label_refr_text(label);
    // lv_refr_now(NULL);
    // lv_obj_invalidate(label);

    lvgl_port_unlock();
  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }
  
}