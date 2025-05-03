#include "comun.h"
#include "esp_log.h"
#include "portmacro.h"
#include <stdint.h>

#include "keyboard.h"

// privated staff
void vTimerCallback_BTN0(TimerHandle_t xTimer);
void vTskKeyboardScan(void *args);

// isr https://esp32.com/viewtopic.php?t=345 y
// https://github.com/espressif/esp-idf/issues/11860 y
// https://esp32.com/viewtopic.php?t=345&start=20
//  https://github.com/anoochit/esp32-example/blob/master/2080_GPIO_Interrupt/main/gpio_intr.c
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
/// ========================
/// === version con isr ===
/// ======================

TimerHandle_t xOneShotTimer[3];
static void IRAM_ATTR gpio_isr_handler(void *arg);

void vTimerCallback(TimerHandle_t xTimer) {
  uint32_t timerID;

  timerID = (uint32_t)pvTimerGetTimerID(xTimer);

  switch (timerID) {
  case BTN_0:
    // ESP_LOGI("timer:", "BTN_0");
    //  hook isr handler for specific gpio pin again
    gpio_isr_handler_add(BTN_0, gpio_isr_handler, (void *)BTN_0);
    break;

  case BTN_1:
    // ESP_LOGI("timer:", "BTN_1");
    //  hook isr handler for specific gpio pin again
    gpio_isr_handler_add(BTN_1, gpio_isr_handler, (void *)BTN_1);
    break;
  case BTN_2:
    // ESP_LOGI("timer:", "BTN_1");
    //  hook isr handler for specific gpio pin again
    gpio_isr_handler_add(BTN_2, gpio_isr_handler, (void *)BTN_2);
    break;

  default:
    ESP_LOGI("timer_ISR_BTN:", "default");

    break;
  }
}

BaseType_t init_inputs(void) {
  BaseType_t err = 0;

  xOneShotTimer[0] = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "timer_BTN0",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(50),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdFALSE,
      /* This example does not use the timer id. */
      (void *const)BTN_0,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback);

  xOneShotTimer[1] = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "timer_BTN1",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(50),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdFALSE,
      /* This example does not use the timer id. */
      (void *const)BTN_1,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback);

  xOneShotTimer[2] = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "timer_BTN2",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(50),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdFALSE,
      /* This example does not use the timer id. */
      (void *const)BTN_2,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback);
  for (int i = 0; i < 3; i++) {
    if (xOneShotTimer[i] == NULL) {
      err = -1; // error
    }
  }

  gpio_config_t io_conf = {};
  // interrupt of rising edge
  io_conf.intr_type = GPIO_INTR_NEGEDGE;
  // bit mask of the pins, use GPIO4/5 here
  io_conf.pin_bit_mask = (1 << BTN_0) | (1 << BTN_1) | (1 << BTN_2);
  // set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  // enable pull-up mode
  io_conf.pull_up_en = 1;
  if (gpio_config(&io_conf) != ESP_OK) {
    err = -1; // error
  };

  // install gpio isr service
  if (gpio_install_isr_service(0) != ESP_OK) { // hay otras opciones en
    err = -1;                                  // error
  };
  // hook isr handler for specific gpio pin
  if (gpio_isr_handler_add(BTN_0, gpio_isr_handler, (void *)BTN_0) != ESP_OK) {
    err = -1; // error
  };
  // hook isr handler for specific gpio pin
  if (gpio_isr_handler_add(BTN_1, gpio_isr_handler, (void *)BTN_1) != ESP_OK) {
    err = -1; // error
  };

  // hook isr handler for specific gpio pin
  if (gpio_isr_handler_add(BTN_2, gpio_isr_handler, (void *)BTN_2) != ESP_OK) {
    err = -1; // error
  };

  return (err);
}

inline static BaseType_t task_notify(uint32_t gpio_num, BaseType_t *pxHigherPriorityTaskWoken) {
  static bool flag_task_to_notify = true;

  static TaskHandle_t hndlTskToSuspend;

  if (true == flag_task_to_notify) {
    flag_task_to_notify = false;
    hndlTskToSuspend = xTaskGetHandle("ESTADOS");
  }
  /* xHigherPriorityTaskWoken must be initialised to pdFALSE. If calling
   xTaskNotifyFromISR() unblocks the handling task, and the priority of
   the handling task is higher than the priority of the currently running task,
   then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */

  /// tiene retorno...
  if (pdPASS != xTaskNotifyFromISR(hndlTskToSuspend, gpio_num,
                                   eSetValueWithoutOverwrite,
                                   pxHigherPriorityTaskWoken)) {
    /// error
    ESP_DRAM_LOGE("Notificacion", "No enviada por anterior pendiente");
  } else {
    ESP_DRAM_LOGE("Notificacion", "Enviada exitosamente");
  }
  return (0);
}

static void IRAM_ATTR gpio_isr_handler(void *arg) {
  uint32_t gpio_num = (uint32_t)arg;
  //  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
  //  uint32_t keys_act = REG_READ(GPIO_IN_REG);
  //  uint32_t mask_input = 1 << gpio_num;

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. If calling
   xTaskNotifyFromISR() unblocks the handling task, and the priority of
   the handling task is higher than the priority of the currently running task,
   then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  static bool flag[] = {0, 0};
  switch (gpio_num) {

  case BTN_0:
    // remove isr handler for gpio number.
    gpio_isr_handler_remove(BTN_0);

    xTimerStart(xOneShotTimer[0], 0);

    if (flag[0] == false) {
      flag[0] = true;
      gpio_set_intr_type(BTN_0, GPIO_INTR_POSEDGE);
      ESP_DRAM_LOGE("INPUT_0", "Faling Edge");
      task_notify(gpio_num, &xHigherPriorityTaskWoken);

    } else {
      flag[0] = false;
      gpio_set_intr_type(BTN_0, GPIO_INTR_NEGEDGE);
      ESP_DRAM_LOGE("INPUT_0", "Rising Edge");
    }

    break;

  case BTN_1:
    gpio_isr_handler_remove(BTN_1);

    xTimerStart(xOneShotTimer[1], 0);

    if (flag[1] == false) {
      flag[1] = true;
      gpio_set_intr_type(BTN_1, GPIO_INTR_POSEDGE);
      ESP_DRAM_LOGE("INPUT_1", "Faling Edge");
      task_notify(gpio_num, &xHigherPriorityTaskWoken);

    } else {
      flag[1] = false;
      gpio_set_intr_type(BTN_1, GPIO_INTR_NEGEDGE);
      ESP_DRAM_LOGE("INPUT_1", "Rising Edge");
    }
    break;

  case BTN_2:
    gpio_isr_handler_remove(BTN_2);

    xTimerStart(xOneShotTimer[2], 0);

    if (flag[2] == false) {
      flag[2] = true;
      gpio_set_intr_type(BTN_2, GPIO_INTR_POSEDGE);
      ESP_DRAM_LOGE("INPUT_2", "Faling Edge");
      task_notify(gpio_num, &xHigherPriorityTaskWoken);
    } else {
      flag[2] = false;
      gpio_set_intr_type(BTN_2, GPIO_INTR_NEGEDGE);
      ESP_DRAM_LOGE("INPUT_2", "Rising Edge");
    }
    break;
  }
  /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.

   The macro used to do this is dependent on the port and may be called

   portEND_SWITCHING_ISR. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
