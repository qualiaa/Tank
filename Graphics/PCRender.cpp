#include "PCRender.hpp"

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstring>
#include "Texture.hpp"
#include "../Utility/Vector.hpp"
#include "../Utility/Rect.hpp"
#include "../System/Game.cpp"

/* --------------------------------------- *
 * Constructor and Destructor
 * --------------------------------------- */

/////////////////////////
//// Constructor
/////////////////////////
PCRender::PCRender()
{
    _screen = nullptr;
}

/////////////////////////
//// Destructor
/////////////////////////
PCRender::~PCRender()
{
    Game::Instance()->log() << "PCRender engine shutting down";

    for(auto iter = _images.begin(); iter != _images.end(); iter++)
    {
        SDL_FreeSurface(iter->second);
    }

    _textures.clear();
}

/* --------------------------------------- *
 * IRender method overrides
 * --------------------------------------- */

/////////////////////////
//// initialize
/////////////////////////
bool PCRender::initialize()
{
    _screen = SDL_GetVideoSurface();
    if(_screen == nullptr)
    {
        Game::Instance()->log() << "Could not retrieve video surface: " << std::string(SDL_GetError());
        return false;
    }

    return true;
}

void PCRender::draw(Texture const* texture, Vectorf const& pos)
{
    //Rect r = { 0, 0, texture.w, texture.h } ;
    //draw( texture, pos, r);

    draw(texture, pos, { 0,  0,  texture->width, texture->height });
}

void PCRender::draw(Texture const* texture, Vectorf const& pos, Rect const& clip)
{
    // Retrieve SDL_Surface from Resources
    std::string tName = texture->name;

    SDL_Surface* surface = _images[tName];

    if(surface != nullptr)
    {
        // Convert position to SDL_Rect
        SDL_Rect offset;

        offset.x = pos.x;
        offset.y = pos.y;

        SDL_Rect sdlClip;

        sdlClip.x = clip.x;
        sdlClip.y = clip.y;
        sdlClip.w = clip.w;
        sdlClip.h = clip.h;

        SDL_BlitSurface(surface, &sdlClip, _screen, &offset);
    }
    else
    {
        Game::Instance()->log() << "Render Error: Texture does not exist (" << texture->name << ")";
    }
}

//TODO Make this support multiple fonts
void PCRender::drawText(char const* text, Vectorf const& pos)
{
    //TODO Get this from screen width or elsewhere
    const int maxWidth = 600;

    SDL_Surface* font = _images["fontsmall"];

    char c; // character buffer
    int length = strlen(text);
    int letterSize = font->h;
    int width = length*letterSize;
    //int height = ceil((float)width/maxWidth)*letterSize;

    if(width > maxWidth)
    {
        width = maxWidth;
    }

    /*
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                width,
                                                height,
                                                32,
                                                font->format->Rmask,
                                                font->format->Gmask,
                                                font->format->Bmask,
                                                font->format->Amask);
                                                */

    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.w = letterSize;
    srcRect.h = letterSize;
    srcRect.x = 0;
    srcRect.y = 0;

    destRect.x = pos.x;
    destRect.y = pos.y;

    for(int i = 0; i < length; ++i)
    {
        c = toupper(text[i]);

        srcRect.x = ((int)c-32)*letterSize;

        //Blit directly to the screen
        //May not work
        SDL_BlitSurface(font, &srcRect, _screen, &destRect);

        destRect.x += letterSize;

        if(destRect.x > maxWidth - letterSize)
        {
            destRect.x  = pos.x;
            destRect.y += letterSize;
        }
    }
}

void PCRender::flipDisplay()
{
    SDL_Flip(_screen);
    SDL_FillRect(_screen, nullptr, SDL_MapRGBA(_screen->format,0x66,0xFF,0xFF,0xFF));
}

bool PCRender::loadImage(char const* name, char const* fileName)
{
    SDL_Surface* temp = nullptr;
    SDL_Surface* optimized = nullptr;

    temp = IMG_Load(fileName);

    if(temp == nullptr)
    {
        Game::Instance()->log() << SDL_GetError();

        return false;
    }

    optimized = SDL_DisplayFormatAlpha(temp);

    SDL_FreeSurface(temp);

    if(optimized  == nullptr)
    {
        Game::Instance()->log() << SDL_GetError();

        return false;
    }

    _images.insert(std::pair<std::string, SDL_Surface*>(name, optimized));

    Texture t = { name, optimized->w, optimized->h };

    _textures.insert(std::pair<std::string, Texture>(name, t));

    return true;
}

Texture const* PCRender::getTexture(char const* name)
{
    std::string strName = name;

    return &_textures[strName];
}
