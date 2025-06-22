#include "comun.h"

#include "display.h"
#include "states.h"

#include "config.h" // getDisplay

#include "clock.h" // getTime
#include "font/lv_font.h"
#include "freertos/idf_additions.h"
#include "misc/lv_style.h"
#include "misc/lv_style_gen.h"
#include <stdint.h>

BaseType_t config_display(void) {
  BaseType_t err = -1; // error

  err = 0; // OK

  return (err);
}

// lv_obj_t *label_clock;
// static lv_obj_t *label;
static lv_obj_t *lblSplash;
static lv_obj_t *lblCrono;
static lv_obj_t *lblLap0;
static lv_obj_t *lblLap1;
static lv_obj_t *lblLap2;

/// Menu
static lv_obj_t *lblMenu;
static lv_obj_t *lblHora;
static lv_obj_t *lblAlarma;

static lv_obj_t *lblChngTime;

SemaphoreHandle_t mutex_hide_hora;
static bool hide_hora;

BaseType_t init_lbl_alarma() {

  BaseType_t err = -1; // error
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblAlarma = label_tmp;
  static lv_style_t style;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_8);
  lv_obj_add_style(lblAlarma, &style,
                   0); // <--- obj is the label
  lv_obj_set_pos(lblAlarma, 0,
                 16);               // el origen esta arriba a la ezquierda
  lv_label_set_text(lblAlarma, ""); // label vacio

  return (err);
}

BaseType_t init_lbl_chng_hora() {

  BaseType_t err = -1; // error
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblChngTime = label_tmp;
  static lv_style_t style;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_obj_add_style(lblChngTime, &style,
                   0); // <--- obj is the label
  lv_obj_set_pos(lblChngTime, 0,
                 0);                  // el origen esta arriba a la ezquierda
  lv_label_set_text(lblChngTime, ""); // label vacio

  return (err);
}

BaseType_t init_lbl_hora() {

  BaseType_t err = -1; // error
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblHora = label_tmp;
  static lv_style_t style;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_obj_add_style(lblHora, &style,
                   0); // <--- obj is the label
  lv_obj_set_pos(lblHora, 0,
                 0);              // el origen esta arriba a la ezquierda
  lv_label_set_text(lblHora, ""); // label vacio

  return (err);
}

BaseType_t init_lbl_menu() {

  BaseType_t err = -1; // error
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblMenu = label_tmp;
  static lv_style_t style;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_montserrat_10);
  lv_obj_add_style(lblMenu, &style,
                   0); // <--- obj is the label
  lv_obj_set_pos(lblMenu, 0,
                 0); // el origen esta arriba a la ezquierda
  // lv_label_set_text(*labelsLaps[i], "88:88:8  88:88:8");
  lv_label_set_text(lblMenu, ""); // label vacio

  // lv_label_set_long_mode(
  //     label,
  //     LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);

  return (err);
}

BaseType_t init_lbl_laps(void) {
  BaseType_t err = -1; // error
  lv_obj_t **labelsLaps[] = {&lblLap0, &lblLap1, &lblLap2};
  static lv_style_t stylesLaps[3];
  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  for (int i = 0; i < 3; i++) {
    /// puede devolver NULL si hay un error
    lv_obj_t *label_tmp = lv_label_create(scr);
    if (label_tmp != NULL) {
      err = 0; // OK
    } else {
      ESP_LOGI("err", "label note created");
    }

    *labelsLaps[i] = label_tmp;

    lv_style_init(&stylesLaps[i]);
    lv_style_set_text_font(&stylesLaps[i], &lv_font_unscii_8);
    lv_obj_add_style(*labelsLaps[i], &stylesLaps[i],
                     0); // <--- obj is the label
    lv_obj_set_pos(*labelsLaps[i], 0,
                   8 * i + 8); // el origen esta arriba a la ezquierda
    // lv_label_set_text(*labelsLaps[i], "88:88:8  88:88:8");
    lv_label_set_text(*labelsLaps[i], ""); // label vacio

    // lv_label_set_long_mode(
    //     label,
    //     LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
  }
  ESP_LOGW("puntero a label Lap0 array", "%p", *labelsLaps[0]);
  ESP_LOGW("puntero a label Lap0 ", "%p", lblLap0);
  ESP_LOGW("puntero a label Lap1 array", "%p", *labelsLaps[1]);
  ESP_LOGW("puntero a label Lap1 ", "%p", lblLap1);
  ESP_LOGW("puntero a label Lap2 array", "%p", *labelsLaps[2]);
  ESP_LOGW("puntero a label Lap2 ", "%p", lblLap2);

  return (err);
}

BaseType_t init_lbl_crono(void) {
  BaseType_t err = -1; // error

  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());
  static lv_style_t style;

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblCrono = label_tmp;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_8);
  lv_obj_add_style(lblCrono, &style, 0); // <--- obj is the label

  // lv_label_set_long_mode(lblCrono,
  //                        LV_LABEL_LONG_CLIP); //
  //                        LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_pos(lblCrono, 0,
                 0); // el origen esta arriba a la ezquierda
  // lv_label_set_text(lblCrono, "22:22:2");
  lv_label_set_text(lblCrono, ""); // label vacio
  return (err);
}

BaseType_t init_lbl_splash(void) {
  BaseType_t err = -1; // error

  lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());

  static lv_style_t style;

  /// puede devolver NULL si hay un error
  lv_obj_t *label_tmp = lv_label_create(scr);
  if (label_tmp != NULL) {
    err = 0; // OK
  } else {
    ESP_LOGI("err", "label note created");
  }

  lblSplash = label_tmp;

  lv_style_init(&style);

  // Create a custom style for an inverted label

  // lv_style_set_text_font(&style, &lv_font_montserrat_16);
  lv_style_set_text_font(&style, &lv_font_montserrat_14);
  // lv_style_set_text_color(&style, lv_color_white());
  // lv_style_set_text_color(&style, lv_color_black());
  // lv_style_set_bg_color(&style,lv_color_black());
  lv_obj_add_style(lblSplash, &style, 0); // <--- obj is the label

  // lv_label_set_long_mode(label,
  //                        LV_LABEL_LONG_CLIP); //
  //                        LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_pos(lblSplash, 0,
                 0); // el origen esta arriba a la izquierda
  // char texto[] = "Hora 88:88\nAlarm 88:88";
  char texto[] = "Reloj(A) y Crono\nPresiones Accion";
  lv_label_set_text(lblSplash, texto);
  ESP_LOGI("display init", "escribo CRONOMETRO");

  return (err);
}

BaseType_t init_display(void) {
  BaseType_t err = -1;                       // error
  mutex_hide_hora = xSemaphoreCreateMutex(); // creo mutex
  hide_hora = true;
  bool mutex = lvgl_port_lock(100);
  if (mutex) {
    err = init_lbl_laps();
    err = init_lbl_crono();
    err = init_lbl_splash();
    err = init_lbl_menu();
    err = init_lbl_hora();
    err = init_lbl_alarma();
    err = init_lbl_chng_hora();
    lvgl_port_unlock();
  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}
BaseType_t update_display(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {
    // lv_obj_clean(lv_scr_act());

    //    lv_label_set_text(label, "--:--:-");
    // lv_label_set_text_fmt(label, "%02d:%02d:%01d", tiempo.mm, tiempo.ss,
    //                      tiempo.dd);
    //  Release the mutex
    // lv_label_refr_text(label);
    // lv_refr_now(NULL);
    // lv_obj_invalidate(label);

    // tiempo_comm_t tiempo;
    // get_time(&tiempo.partes);
    //  lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);
    // lv_label_set_text(label, ""); // borro texto inicial

    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t delete_display(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {
    // lv_obj_clean(lv_scr_act());

    //    lv_label_set_text(label, "--:--:-");
    // lv_label_set_text_fmt(label, "%02d:%02d:%01d", tiempo.mm, tiempo.ss,
    //                      tiempo.dd);
    //  Release the mutex
    // lv_label_refr_text(label);
    // lv_refr_now(NULL);
    // lv_obj_invalidate(label);

    // tiempo_comm_t tiempo;
    // get_time(&tiempo.partes);
    //  lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);
    lv_label_set_text(lblSplash, "");   // borro texto inicial
    lv_label_set_text(lblMenu, "");     // borro texto
    lv_label_set_text(lblCrono, "");    // borro texto
    lv_label_set_text(lblLap0, "");     // borro texto
    lv_label_set_text(lblLap1, "");     // borro texto
    lv_label_set_text(lblLap2, "");     // borro texto
    lv_label_set_text(lblHora, "");     // borro texto
    lv_label_set_text(lblAlarma, "");   // borro texto
    lv_label_set_text(lblChngTime, ""); // borro texto

    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}
BaseType_t show_display_hora(void) {
  BaseType_t err = -1; // error

  if (xSemaphoreTake(mutex_hide_hora, pdMS_TO_TICKS(1)) == pdTRUE) {
    hide_hora = false;
    ESP_LOGI("hide_hora", "false");

    xSemaphoreGive(mutex_hide_hora);
    err = 1;
  }

  return (err);
}
BaseType_t hide_display_hora(void) {
  BaseType_t err = -1; // error

  if (xSemaphoreTake(mutex_hide_hora, pdMS_TO_TICKS(1)) == pdTRUE) {
    hide_hora = true;
    ESP_LOGI("hide_hora", "true");
    xSemaphoreGive(mutex_hide_hora);
    err = 1;
  }

  return (err);
}

bool update_display_chng_hora(uint32_t key) {
  bool exit = false;
  static uint8_t st_cnt = 0; // 0 es idle, 1 es horas, 2 es minutos
  static uint8_t hh_tmp = 0;
  static uint8_t mm_tmp = 0;

  switch (st_cnt) {
  case 0:
    ESP_LOGI("CHNG_HORA", "Case 0");

    st_cnt = 1;
    tiempo_comm_t tiempo;
    get_hora(&tiempo.partes);
    hh_tmp = tiempo.partes.hh;
    mm_tmp = tiempo.partes.mm;
    break;
  case 1: // horas
    ESP_LOGI("CHNG_HORA", "Case 1");
    switch (key) {
    case BTN_ACT:
      st_cnt = 2;
      break;
    case BTN_UP:
      hh_tmp++;
      if (hh_tmp == 24) {
        hh_tmp = 0;
      }
      break;
    case BTN_DOWN:
      hh_tmp--;
      if (hh_tmp == 255) {
        hh_tmp = 23;
      }
      break;
    }
    break;
  case 2: // minutos
    ESP_LOGI("CHNG_HORA", "Case 2");
    switch (key) {
    case BTN_ACT:
      st_cnt = 0;
      exit = true;

      tiempo_comm_t tiempo;
      tiempo.partes.hh = hh_tmp;
      tiempo.partes.mm = mm_tmp;
      set_hora(&tiempo.partes);
      break;
    case BTN_UP:
      mm_tmp++;
      if (mm_tmp == 60) {
        mm_tmp = 0;
      }
      break;
    case BTN_DOWN:
      mm_tmp--;
      if (mm_tmp == 255) {
        mm_tmp = 59;
      }
      break;
    }
    break;
  }
 
  bool mutex = lvgl_port_lock(0);

  if (mutex) {

    lv_label_set_text_fmt(lblChngTime, "C. Hora\n %02d:%02d", hh_tmp, mm_tmp);

    lvgl_port_unlock();
    // err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }
  return (exit);
}


bool update_display_chng_alarma(uint32_t key) {
  bool exit = false;
  static uint8_t st_cnt = 0; // 0 es idle, 1 es horas, 2 es minutos
  static uint8_t hh_tmp = 0;
  static uint8_t mm_tmp = 0;

  switch (st_cnt) {
  case 0:
    ESP_LOGI("CHNG_ALARM", "Case 0");

    st_cnt = 1;
    tiempo_comm_t tiempo;
    get_alarma(&tiempo.partes);
    hh_tmp = tiempo.partes.hh;
    mm_tmp = tiempo.partes.mm;
    break;
  case 1: // horas
    ESP_LOGI("CHNG_ALARM", "Case 1");
    switch (key) {
    case BTN_ACT:
      st_cnt = 2;
      break;
    case BTN_UP:
      hh_tmp++;
      if (hh_tmp == 24) {
        hh_tmp = 0;
      }
      break;
    case BTN_DOWN:
      hh_tmp--;
      if (hh_tmp == 255) {
        hh_tmp = 23;
      }
      break;
    }
    break;
  case 2: // minutos
    ESP_LOGI("CHNG_ALARM", "Case 2");
    switch (key) {
    case BTN_ACT:
      st_cnt = 0;
      exit = true;

      tiempo_comm_t tiempo;
      tiempo.partes.hh = hh_tmp;
      tiempo.partes.mm = mm_tmp;
      set_alarma(&tiempo.partes);
      break;
    case BTN_UP:
      mm_tmp++;
      if (mm_tmp == 60) {
        mm_tmp = 0;
      }
      break;
    case BTN_DOWN:
      mm_tmp--;
      if (mm_tmp == 255) {
        mm_tmp = 59;
      }
      break;
    }
    break;
  }
 
  bool mutex = lvgl_port_lock(0);

  if (mutex) {

    lv_label_set_text_fmt(lblChngTime, "C. Alarm\n %02d:%02d", hh_tmp, mm_tmp);

    lvgl_port_unlock();
    // err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }
  return (exit);
}


BaseType_t update_display_hora(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(mutex_hide_hora, pdMS_TO_TICKS(1)) == pdTRUE) {
    if (false == hide_hora) {
      bool mutex = lvgl_port_lock(0);
      if (mutex) {

        tiempo_comm_t tiempo, alarma;
        get_hora(&tiempo.partes);
        lv_label_set_text_fmt(lblHora, "%02d:%02d", tiempo.partes.hh,
                              tiempo.partes.mm);
        get_alarma(&alarma.partes);
        if ((alarma.partes.hh == tiempo.partes.hh) &&
            (alarma.partes.mm == tiempo.partes.mm)) {
          lv_label_set_text(lblAlarma, "ALARMA RING");
        } else {
          lv_label_set_text_fmt(lblAlarma, "Alarma: %02d:%02d",
                                alarma.partes.hh, alarma.partes.mm);
        }

        lvgl_port_unlock();
        err = 0; // OK

      } else {
        ESP_LOGI("err", "mutex del display no disponible");
      }
    }
    xSemaphoreGive(mutex_hide_hora);
  }
  return (err);
}

BaseType_t update_display_crono(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    tiempo_comm_t tiempo;
    get_crono(&tiempo.partes);
    lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
                          tiempo.partes.ss, tiempo.partes.dd);

    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t reset_display_crono_laps(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    tiempo_comm_t tiempo;
    // get_time(&tiempo.partes);
    // lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);

    lv_obj_t **labelsLaps[] = {&lblLap0, &lblLap1, &lblLap2};
    for (unsigned int i = 0; i < 3; i++) {
      lv_label_set_text_fmt(*labelsLaps[i], "Lap %u: 00:00:0", i);
    }
    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t update_display_crono_laps(unsigned int label_num,
                                     tiempo_comm_t tiempo) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    // get_time(&tiempo.partes);
    // lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);

    lv_obj_t **labelsLaps[] = {&lblLap0, &lblLap1, &lblLap2};
    if ((0 <= label_num) && (label_num <= 2)) {
      lv_label_set_text_fmt(*labelsLaps[label_num], "Lap %u: %02d:%02d:%01d",
                            label_num, tiempo.partes.mm, tiempo.partes.ss,
                            tiempo.partes.dd);
      err = 0; // OK
      lvgl_port_unlock();
    } else {
      lvgl_port_unlock();
      err = -1; // error
    }
  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

/// --- Menu ---

BaseType_t update_display_menu(unsigned int menu_selsect) {
  BaseType_t err = -1; // error

  bool mutex = lvgl_port_lock(0);

  if (mutex) {

    switch (menu_selsect) {
    case 0:
      lv_label_set_text(
          lblMenu,
          ">Iniciar Cronometro \n_Configurar Alarma \n_Actualizar Hora");
      break;

    case 1:
      lv_label_set_text(
          lblMenu,
          "_Iniciar Cronometro \n>Configurar Alarma \n_Actualizar Hora");
      break;

    case 2:
      lv_label_set_text(
          lblMenu,
          "_Iniciar Cronometro \n_Configurar Alarma \n>Actualizar Hora");
      break;
    }

    err = 0; // OK
    lvgl_port_unlock();
  } else {
    lvgl_port_unlock();
    err = -1; // error
    ESP_LOGI("err", "mutex del display no disponible");
  }
  return (err);
}
