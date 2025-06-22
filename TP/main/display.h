#ifndef DISPLAY_H
#define DISPLAY_H

BaseType_t config_display(void);

BaseType_t init_display(void);
BaseType_t update_display(void);
BaseType_t delete_display(void);

BaseType_t update_display_hora(void);
BaseType_t show_display_hora(void);
BaseType_t hide_display_hora(void);


BaseType_t update_display_crono(void);
BaseType_t reset_display_crono_laps(void);
BaseType_t update_display_crono_laps(unsigned int label_num, tiempo_comm_t tiempo);

BaseType_t update_display_menu(unsigned int menu_selsect);

bool update_display_chng_hora(uint32_t key);

bool update_display_chng_alarm(uint32_t key);





#endif /* DISPLAY_H */