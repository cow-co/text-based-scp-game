#ifndef MENU_H
#define MENU_H

extern const char saveFileName[64];

int save_game(const char* const path);
int load_game(const char* const path);
void present_menu();

#endif
