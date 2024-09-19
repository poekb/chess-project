#include "../chess-project.h"
#include "svg-loader.h"


const char PIECE_FILES[6] = {
	'k', // king
	'q', // queen
	'r', // rook
	'b', // bishop
	'n', // knight
	'p'  // pawn
};

typedef struct PieceTex {
	SDL_Surface *surface;
	SDL_Texture *texture;
} PieceTex;

PieceTex piece_tex_data[12];

void SVG_init(SDL_Renderer *renderer) {
	IMG_Init(IMG_INIT_PNG);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	for (int i = 0; i < 12; i++) {
		char fileString[] = "assets/00.svg";
		
		fileString[7] = PIECE_FILES[i % 6];
		fileString[8] = i < 6 ? 'd' : 'l';

		SDL_RWops* rw = SDL_RWFromFile(fileString, "r");
		SDL_Surface* surface = IMG_Load_RW(rw, true);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		piece_tex_data[i].surface = surface;
		piece_tex_data[i].texture = texture;
	}
}

void SVG_renderPiece(SDL_Renderer* renderer, Uint8 piece, int pos_x, int pos_y, double cellSize) {
	if (piece == 0) return;
	SDL_Rect texture_rect;
	SDL_Surface* surface = piece_tex_data[piece-1].surface;
	SDL_Texture* texture = piece_tex_data[piece-1].texture;
	texture_rect.x = pos_x + (int)(cellSize * 0.05);
	texture_rect.y = pos_y + (int)(cellSize * 0.05);
	texture_rect.w = (int)((double)surface->w / max(surface->w, surface->h) * cellSize * 0.9);
	texture_rect.h = (int)((double)surface->h / max(surface->w, surface->h) * cellSize * 0.9);

	SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
}

void SVG_clear() {
	for (int i = 0; i < 12; i++) {
		SDL_FreeSurface(piece_tex_data[i].surface);
		SDL_DestroyTexture(piece_tex_data[i].texture);
	}
	IMG_Quit();
}