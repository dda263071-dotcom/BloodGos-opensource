#ifndef _KEYBOARD_H
#define _KEYBOARD_H

bool keyboard_init(void);
char keyboard_getchar(void);
char keyboard_read_char(void);
bool keyboard_has_data(void);
void keyboard_read_string(char* buffer, size_t max_len);
void keyboard_get_state(bool* shift, bool* ctrl, bool* alt, bool* caps);

#endif
