#include "chess-project.h"
#include <SDL_ttf.h>



#include "text-renderer.h"

TTF_Font* Font;

void textRend_init() {
    TTF_Init();
    // Az svg-k átméretezéséhez használt "scale quality" növelése
    Font = TTF_OpenFont("C:/Windows/Fonts/ARLRDBD.TTF", 30);
}

void textRend_clean() {
    TTF_CloseFont(Font);

    TTF_Quit();
}

void render_text(SDL_Renderer* renderer, char* text, int size, int pos_x, int pos_y, SDL_Color color) {

    TTF_SetFontSize(Font, size);

    SDL_Surface* text_surface = TTF_RenderText_Blended(Font, text, color);

    SDL_Rect text_rect;
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);


    text_rect.x = pos_x;
    text_rect.y = pos_y;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void render_char(SDL_Renderer* renderer, char c, int size, int pos_x, int pos_y, SDL_Color color) {
    char text[2] = { c,'\0' };

    render_text(renderer, text, size, pos_x, pos_y, color);

}
