#include "global.h"
#include <SDL_ttf.h>

void textRend_init();
void textRend_clean();
void render_text(SDL_Renderer* renderer, char* text, int size, int pos_x, int pos_y, SDL_Color color);
void render_char(SDL_Renderer* renderer, char c, int size, int pos_x, int pos_y, SDL_Color color);

