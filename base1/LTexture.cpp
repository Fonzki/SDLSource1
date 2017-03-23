#include "LTexture.h"

LTexture::LTexture(){
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    free();
}

bool LTexture::loadFromFile(std::string path, SDL_Renderer* renderer){
    free();

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        SDL_Log("failed to load image %s! SDL_Error: %s\n", 
                path.c_str(), IMG_GetError());
    } else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(
                        loadedSurface->format, 0, 0xFF, 0xFF));

        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if(newTexture == NULL){
            SDL_Log("unable to create texture from %s! SDL_Error: %s\n",
                    path.c_str(), SDL_GetError());
        } else {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != NULL;
}/*
bool LTexture::loadFromFile(std::string path, SDL_Renderer* renderer){
    free();

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        SDL_Log("failed to load image %s! SDL_Error: %s\n", 
                path.c_str(), IMG_GetError());
    } else {
		if(!window) {SDL_Log("lol no window\n"); return false;}

       	SDL_Surface* formattedSurface = 
	    	SDL_ConvertSurface(loadedSurface, 
				SDL_GetWindowSurface(window)->format, 0);
		
		if(!formattedSurface){
			SDL_Log("failed to convert loaded surface to display format\n");
		} else {
			if(!renderer) return false;
			newTexture = SDL_CreateTexture(renderer,
										   SDL_GetWindowPixelFormat(window),
										   SDL_TEXTUREACCESS_STREAMING,
										   formattedSurface->w,
										   formattedSurface->h);
			if(!newTexture){
				SDL_Log("failed to create blank texture!\n");
			} else {
				SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);
				memcpy(mPixels, formattedSurface->pixels,
					   formattedSurface->pitch * formattedSurface->h);
				SDL_UnlockTexture(newTexture);
				mPixels = NULL;

				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;
			}
			SDL_FreeSurface(formattedSurface);
		}
        SDL_FreeSurface(loadedSurface);
    }
    mTexture = newTexture;
    return mTexture != NULL;
}
*/
void LTexture::free(){
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(SDL_Renderer* renderer,
                      int x, int y, SDL_Rect* clip,
                      double angle, SDL_Point* center,
                      SDL_RendererFlip flip){
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if(clip != NULL){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad,
                   angle, center, flip);
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

bool LTexture::loadFromRenderedText(TTF_Font* font, SDL_Renderer* renderer,
                                    std::string textureText,
                                    SDL_Color textColor){
    free();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font,
                                                    textureText.c_str(),
                                                    textColor);
    if(textSurface == NULL){
        SDL_Log("Unable to render text surface! SDL_ttf: %s\n", TTF_GetError());
    } else {
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(mTexture == NULL){
            SDL_Log("failed to create texture from text! SDL_Error: %s\n",
                    SDL_GetError());
        } else {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    return mTexture != NULL;
}

void LTexture::setWindow(SDL_Window* newWindow){
	window = newWindow;
}

void LTexture::setRenderer(SDL_Renderer* newRenderer){
	renderer = newRenderer;
}

bool LTexture::lockTexture(){
	if(mPixels != NULL){
		SDL_Log("texture already locked!\n");
		return false;
	}

	if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0){
		SDL_Log("unable to lock texture%s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool LTexture::unlockTexture(){
	if(!mPixels){
		SDL_Log("texture is not locked\n");
		return false;
	} else {
		SDL_UnlockTexture(mTexture);
		mPixels = NULL;
		mPitch = 0;
	}

	return true;
}

void* LTexture::getPixels(){
	return mPixels;
}

int LTexture::getPitch(){
	return mPitch;
}
