#include "comun.h"

#include "display.h"

#include "config.h" // getDisplay

BaseType_t config_display(void) {
    BaseType_t err = -1; // error

    err = 0; // OK

    return (err);

}

lv_obj_t *label_clock;
static lv_obj_t *label;
BaseType_t init_display(void){
    BaseType_t err = -1; // error
    bool mutex = lvgl_port_lock(100);
    if (mutex) {
      lv_obj_t *scr = lv_disp_get_scr_act(getDisplay());
      static lv_style_t style;
  
      /// puede devolver NULL si hay un error
      lv_obj_t *label_tmp = lv_label_create(scr);
      if(label_tmp != NULL) {
        err = 0; // OK
      }
      else {
        ESP_LOGI("err", "label note created");
      }
      
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

    return (err);

}
BaseType_t update_display(void){
    BaseType_t err = -1; // error
    bool mutex = lvgl_port_lock(0);
    if (mutex) {
        // lv_obj_clean(lv_scr_act());
    
      
        //    lv_label_set_text(label, "--:--:-");
        //lv_label_set_text_fmt(label, "%02d:%02d:%01d", tiempo.mm, tiempo.ss,
        //                      tiempo.dd);
        //  Release the mutex
        // lv_label_refr_text(label);
        // lv_refr_now(NULL);
        // lv_obj_invalidate(label);
    
        lvgl_port_unlock();
        err = 0; // OK

      } else {
        ESP_LOGI("err", "mutex del display no disponible");
      }


    return (err);

}