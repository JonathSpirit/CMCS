#include "C_sprite.hpp"

namespace cmcs::draw
{

Sprite::Sprite() :
        g_rect({0.0f,0.0f,0.0f,0.0f}),
        g_colorMod({255,255,255,255})
{
}

void Sprite::draw(SDL_Renderer* renderer)
{
    if (renderer != nullptr && this->g_texture.get() != nullptr)
    {
        SDL_SetTextureColorMod(this->g_texture.get(), this->g_colorMod.r, this->g_colorMod.g, this->g_colorMod.b);
        SDL_SetTextureAlphaMod(this->g_texture.get(), this->g_colorMod.a);
        SDL_RenderCopyF(renderer, this->g_texture.get(), nullptr, &this->g_rect);
    }
}

void Sprite::setTexture(const Texture& texture, bool useTextureSize)
{
    this->g_texture = texture;
    if (useTextureSize && this->g_texture.get() != nullptr)
    {
        int w,h;
        if (SDL_QueryTexture(this->g_texture.get(), nullptr, nullptr, &w, &h) == 0)
        {
            this->setSize({static_cast<float>(w), static_cast<float>(h)});
        }
    }
}

const Texture& Sprite::getTexture() const
{
    return this->g_texture;
}

void Sprite::setColorMod(SDL_Color color)
{
    this->g_colorMod = color;
}
const SDL_Color& Sprite::getColorMod() const
{
    return this->g_colorMod;
}

void Sprite::setPosition(SDL_FPoint point)
{
    this->g_rect.x = point.x;
    this->g_rect.y = point.y;
}

SDL_FPoint Sprite::getPosition() const
{
    return {this->g_rect.x, this->g_rect.y};
}

void Sprite::setSize(SDL_FPoint size)
{
    this->g_rect.w = size.x;
    this->g_rect.h = size.y;
}

SDL_FPoint Sprite::getSize() const
{
    return {this->g_rect.w, this->g_rect.h};
}

const SDL_FRect& Sprite::getRect() const
{
    return this->g_rect;
}

}//end cmcs::draw