#include "comun.h"
#include "esp_log.h"
#include "portmacro.h"

#include "keyboard.h"

// privated staff
void vTimerCallback_BTN0(TimerHandle_t xTimer);
void vTskKeyboardScan(void *args);

TimerHandle_t xOneShotTimer_BTN0;

BaseType_t flag_BTN_0 = 0; // 1 habilitado y 0 deshabilitado
BaseType_t keyboard_init(void) {
  BaseType_t err = -1;

  /* Create the one shot timer, storing the handle to the created timer in
   xOneShotTimer. */
  xOneShotTimer_BTN0 = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "timer_BTN0",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(50),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdFALSE,
      /* This example does not use the timer id. */
      0,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback_BTN0);
  if (xOneShotTimer_BTN0 != NULL) { // se creo bien el timer
    // lanzo timer
    BaseType_t xTimerStart_BTN0 =
        xTimerStart(xOneShotTimer_BTN0, 0); // no se espera nada
    if (xTimerStart_BTN0 == pdPASS) {
      err = 0; // todo OK
    }
  }
  // configuro teclas
  // configuro timers
  if (err == 0) {
    ESP_LOGI("Timer BTN_0 OK", "Todo Bien");
  } else {
    ESP_LOGI("Timer BTN_0 NoOK", "Todo Bien");
  }
  
  xTaskCreate(vTskKeyboardScan, "KEY_SCAN", configMINIMAL_STACK_SIZE * 4, NULL,
    tskIDLE_PRIORITY + 1, NULL);
  return (err);
}

// en el futuro sera una ISR
void vTskKeyboardScan(void *args) {
#define FLANCO_ASCENDETEN(TECLA)                                               \
  (~keys_act & (1 << TECLA)) &&                                                \
      ((~keys_act & (1 << TECLA)) != (~keys_ant & (1 << TECLA)))
#define MASK_KEYS (1 << BTN_0) | (1 << BTN_1) | (1 << BTN_2)

  uint32_t keys_act = 0;
  for (;;) {
    uint32_t keys_ant = keys_act;
    keys_act = REG_READ(GPIO_IN_REG);
    keys_act &= MASK_KEYS;

    if (flag_BTN_0 == 1) {
      if (FLANCO_ASCENDETEN(BTN_0)) {
        ESP_LOGI("keyboard scan", "flanco ascendete en btn_0");
        BaseType_t xTimerStart_BTN0 =
            xTimerStart(xOneShotTimer_BTN0, 0); // no se espera nada
        if (xTimerStart_BTN0 == pdPASS) {
          ESP_LOGI(" Timer BTN_0", "lanzado");
        } else {
          ESP_LOGI(" Timer BTN_0", "err");
        }
        // lanzo timer
        flag_BTN_0 = 0;
      }

      else {
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));

  }
}

// Timers BTN_0
void vTimerCallback_BTN0(TimerHandle_t xTimer) {
  if (flag_BTN_0 == 0) {

    // lanzo timer
    flag_BTN_0 = 1;
    ESP_LOGI("Timer BTN_0 ok", "flag_BTN_0 == 1");
  }

  else {
    ESP_LOGI("Timer BTN_0: err", "flag_BTN_0 != 1");
  }
}
