#include "C_text.hpp"

namespace cmcs::draw
{

Text::Text() :
        g_rect({0.0f,0.0f,0.0f,0.0f}),
        g_fgColor({0,0,0,255})
{
}
Text::Text(SDL_Renderer* renderer, const Font& font, std::string str, SDL_Color color, bool useTextureSize) :
        Text()
{
    this->setFont(renderer, font, false);
    this->setForegroundColor(renderer, color);
    this->setString(renderer, std::move(str), useTextureSize);
}

void Text::draw(SDL_Renderer* renderer)
{
    if (renderer != nullptr && this->g_texture.get() != nullptr)
    {
        SDL_RenderCopyF(renderer, this->g_texture.get(), nullptr, &this->g_rect);
    }
}

void Text::setString(SDL_Renderer* renderer, std::string str, bool useTextureSize)
{
    this->g_string = std::move(str);
    this->refreshInternalTexture(renderer);
    if (useTextureSize && this->g_texture.get() != nullptr)
    {
        int w,h;
        if (SDL_QueryTexture(this->g_texture.get(), nullptr, nullptr, &w, &h) == 0)
        {
            this->setSize({static_cast<float>(w), static_cast<float>(h)});
        }
    }
}

const std::string& Text::getString() const
{
    return this->g_string;
}

void Text::setFont(SDL_Renderer* renderer, const Font& font, bool useTextureSize)
{
    this->g_font = font;
    this->refreshInternalTexture(renderer);
    if (useTextureSize && this->g_texture.get() != nullptr)
    {
        int w,h;
        if (SDL_QueryTexture(this->g_texture.get(), nullptr, nullptr, &w, &h) == 0)
        {
            this->setSize({static_cast<float>(w), static_cast<float>(h)});
        }
    }
}
const Font& Text::getFont() const
{
    return this->g_font;
}

const Texture& Text::getTexture() const
{
    return this->g_texture;
}

void Text::setPosition(SDL_FPoint point)
{
    this->g_rect.x = point.x;
    this->g_rect.y = point.y;
}

SDL_FPoint Text::getPosition() const
{
    return {this->g_rect.x, this->g_rect.y};
}

void Text::setSize(SDL_FPoint size)
{
    this->g_rect.w = size.x;
    this->g_rect.h = size.y;
}

SDL_FPoint Text::getSize() const
{
    return {this->g_rect.w, this->g_rect.h};
}

void Text::setForegroundColor(SDL_Renderer* renderer, SDL_Color color)
{
    this->g_fgColor = color;
    this->refreshInternalTexture(renderer);
}
const SDL_Color& Text::getForegroundColor() const
{
    return this->g_fgColor;
}

const SDL_FRect& Text::getRect() const
{
    return this->g_rect;
}

void Text::refreshInternalTexture(SDL_Renderer* renderer)
{
    if (this->g_font.get() != nullptr)
    {
        SDL_Surface* surface = TTF_RenderText_Solid(this->g_font.get(), this->g_string.c_str(), this->g_fgColor);
        if (surface != nullptr)
        {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            this->g_texture.set(texture);
        }
    }
}

}//end cmcs::draw