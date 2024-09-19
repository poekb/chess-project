void renderer_init(SDL_Renderer* renderer);
void rederer_cleanUp();
void render_board(SDL_Renderer* renderer, double table_size, Uint8 board[8][8]);
void render_dynamic(SDL_Renderer* renderer);
void high_light_cell(SDL_Renderer* renderer, int row, int col);


void sdl_init(int szeles, int magas, SDL_Window** pwindow, SDL_Renderer** prenderer);