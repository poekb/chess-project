#include "textRenderer.h"
#include "boardRenderer.h"

TTF_Font* Font;

void textRendererInit() {
    TTF_Init();
    Font = TTF_OpenFont("assets/font.TTF", 30);
}

void textRendererCleanup() {
    TTF_CloseFont(Font);

    TTF_Quit();
}

void renderTextbox(SDL_Renderer* renderer, char* text, SDL_Rect rect, SDL_Color bgColor, SDL_Color textColor) {
    setDrawColor(renderer, bgColor);
    SDL_RenderFillRect(renderer, &rect);

    TTF_SetFontSize(Font, rect.h/3*2);

    SDL_Surface* text_surface = TTF_RenderText_Blended(Font, text, textColor);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_Rect text_rect;
    text_rect.x = rect.x + (rect.w - text_surface->w) / 2;
    text_rect.y = rect.y + (rect.h - text_surface->h) / 2;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void renderText(SDL_Renderer* renderer, char* text, int size, int pos_x, int pos_y, SDL_Color color) {

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

void renderChar(SDL_Renderer* renderer, char c, int size, int pos_x, int pos_y, SDL_Color color) {
    char text[2] = { c,'\0' };

    renderText(renderer, text, size, pos_x, pos_y, color);

}
