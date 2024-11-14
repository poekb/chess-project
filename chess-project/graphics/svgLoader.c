#include "svgLoader.h"

const char PIECE_FILES[6] = {
	'p',
	'n',
	'b',
	'r',
	'q',
	'k'
};

typedef struct PieceTex {
	SDL_Surface *surface;
	SDL_Texture *texture;
} PieceTex;

PieceTex piece_tex_data[12];

// Load svg files in textures
void SVG_Init(SDL_Renderer *renderer) {
	IMG_Init(IMG_INIT_PNG);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	for (int i = 0; i < 12; i++) {
		char fileString[20];
		
		sprintf(fileString, "assets/%c%c.svg", PIECE_FILES[i % 6], i < 6 ? 'l' : 'd');

		SDL_RWops* rw = SDL_RWFromFile(fileString, "r");
		SDL_Surface* surface = IMG_Load_RW(rw, true);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		piece_tex_data[i].surface = surface;
		piece_tex_data[i].texture = texture;
	}
}

// Copy the texture of a piece to the renderer
void SVG_RenderPiece(SDL_Renderer* renderer, Piece piece, SDL_Rect rect) {
	if (piece == 0) return;
	if (!IsWhite(piece)) piece -= 2;
	SDL_Surface* surface = piece_tex_data[piece-1].surface;
	SDL_Texture* texture = piece_tex_data[piece-1].texture;

	int trimX = (int)(rect.w * 0.05);
	int trimY = (int)(rect.h * 0.05);

	rect.x += trimX;
	rect.y += trimY;
	rect.w -= trimX*2;
	rect.h -= trimY*2;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void SVG_Clear() {
	for (int i = 0; i < 12; i++) {
		SDL_FreeSurface(piece_tex_data[i].surface);
		SDL_DestroyTexture(piece_tex_data[i].texture);
	}
	IMG_Quit();
}