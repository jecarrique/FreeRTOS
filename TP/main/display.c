#include "comun.h"

#include "display.h"

#include "config.h" // getDisplay

#include "clock.h" // getTime

BaseType_t config_display(void) {
  BaseType_t err = -1; // error

  err = 0; // OK

  return (err);
}

lv_obj_t *label_clock;
static lv_obj_t *label;
static lv_obj_t *lblCrono;
static lv_obj_t *lblLap0;
static lv_obj_t *lblLap1;
static lv_obj_t *lblLap2;

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
    lv_label_set_text(*labelsLaps[i], "88:88:8  88:88:8");
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
  lv_label_set_text(lblCrono, "22:22:2");
  return (err);
}

BaseType_t init_lbl_gral(void) {
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

  label = label_tmp;

  lv_style_init(&style);
  lv_style_set_text_font(&style, &lv_font_unscii_16);
  lv_obj_add_style(label, &style, 0); // <--- obj is the label

  lv_label_set_long_mode(label,
                         LV_LABEL_LONG_CLIP); // LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_label_set_text(label, "");
  return (err);
}

BaseType_t init_display(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(100);
  if (mutex) {
    err = init_lbl_gral();
    err = init_lbl_laps();
    err = init_lbl_crono();

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

    tiempo_comm_t tiempo;
    get_time(&tiempo.partes);
    // lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                       tiempo.partes.ss, tiempo.partes.dd);

    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t update_display_crono(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    tiempo_comm_t tiempo;
    get_time(&tiempo.partes);
    lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
                          tiempo.partes.ss, tiempo.partes.dd);

    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t reset_display_crono_labels(void) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    tiempo_comm_t tiempo;
    // get_time(&tiempo.partes);
    // lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);

    lv_obj_t **labelsLaps[] = {&lblLap0, &lblLap1, &lblLap2};
    for (unsigned int i = 0; i < 3; i++) {
      lv_label_set_text_fmt(*labelsLaps[i], "Lap %u: 00:00:0",i);
    }
    lvgl_port_unlock();
    err = 0; // OK

  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}

BaseType_t update_display_crono_label(unsigned int label_num, tiempo_comm_t tiempo) {
  BaseType_t err = -1; // error
  bool mutex = lvgl_port_lock(0);
  if (mutex) {

    // get_time(&tiempo.partes);
    // lv_label_set_text_fmt(lblCrono, "%02d:%02d:%01d", tiempo.partes.mm,
    //                        tiempo.partes.ss, tiempo.partes.dd);

    lv_obj_t **labelsLaps[] = {&lblLap0, &lblLap1, &lblLap2};
    if ( ( 0 <= label_num ) && ( label_num <= 2 ) ) {
     lv_label_set_text_fmt(*labelsLaps[label_num], "Lap %u: %02d:%02d:%01d", label_num, tiempo.partes.mm,
                            tiempo.partes.ss, tiempo.partes.dd);
    err = 0; // OK
    lvgl_port_unlock();
    }
    else {
      lvgl_port_unlock();
      err = -1; // error
    }
  } else {
    ESP_LOGI("err", "mutex del display no disponible");
  }

  return (err);
}