#ifndef DISPLAY_H
#define DISPLAY_H

BaseType_t config_display(void);

BaseType_t init_display(void);
BaseType_t update_display(void);
BaseType_t update_display_crono(void);
BaseType_t reset_display_crono_labels(void);
BaseType_t update_display_crono_label(unsigned int label_num, tiempo_comm_t tiempo);





#endif /* DISPLAY_H */