#ifndef GUARD_LTEXTURE
#define GUARD_LTEXTURE
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <cstring>

class LTexture{
    public:
        LTexture();
        ~LTexture();

        bool loadFromFile(std::string path, SDL_Renderer* renderer);
        
        bool loadFromRenderedText(TTF_Font* font, SDL_Renderer* renderer,
                                  std::string textureText,
                                  SDL_Color textColor);
        void free();

        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        void setBlendMode(SDL_BlendMode blending);
        void setAlpha(Uint8 alpha);

        void render(SDL_Renderer* Renderer,
                    int x, int y, SDL_Rect* clip = NULL,
                    double angle = 0.0, SDL_Point* center = NULL,
                    SDL_RendererFlip flip = SDL_FLIP_NONE);
        
        int getWidth();
        int getHeight();

		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		int getPitch();

		void setWindow(SDL_Window* newWindow);
		void setRenderer(SDL_Renderer* newRenderer);

    private:
		SDL_Renderer* renderer;
		SDL_Window* window;

        SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

        int mWidth;
        int mHeight;
};
#endif
