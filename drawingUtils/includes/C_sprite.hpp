#ifndef _CMCS_DRAW_C_SPRITE_HPP_INCLUDED
#define _CMCS_DRAW_C_SPRITE_HPP_INCLUDED

#include "drawingUtilsExtern.hpp"
#include <SDL.h>
#include "C_texture.hpp"

namespace cmcs::draw
{

class CMCS_DRAWINGUTILS_API Sprite
{
public:
    Sprite();

    void draw(SDL_Renderer* renderer);

    void setTexture(const Texture& texture, bool useTextureSize=true);
    [[nodiscard]] const Texture& getTexture() const;

    void setColorMod(SDL_Color color);
    [[nodiscard]] const SDL_Color& getColorMod() const;

    void setPosition(SDL_FPoint point);
    [[nodiscard]] SDL_FPoint getPosition() const;
    void setSize(SDL_FPoint size);
    [[nodiscard]] SDL_FPoint getSize() const;

    [[nodiscard]] const SDL_FRect& getRect() const;

private:
    Texture g_texture;
    SDL_FRect g_rect;
    SDL_Color g_colorMod;
};

}//end cmcs::draw

#endif //_CMCS_DRAW_C_SPRITE_HPP_INCLUDED
