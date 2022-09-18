#ifndef _CMCS_DRAW_C_TEXT_HPP_INCLUDED
#define _CMCS_DRAW_C_TEXT_HPP_INCLUDED

#include "drawingUtilsExtern.hpp"
#include <SDL.h>
#include "C_font.hpp"
#include "C_texture.hpp"
#include <string>

namespace cmcs::draw
{

class CMCS_DRAWINGUTILS_API Text
{
public:
    Text();
    Text(SDL_Renderer* renderer, const Font& font, std::string str, SDL_Color color={0,0,0,255}, bool useTextureSize=true);

    void draw(SDL_Renderer* renderer);

    void setString(SDL_Renderer* renderer, std::string str, bool useTextureSize=true);
    [[nodiscard]] const std::string& getString() const;

    void setFont(SDL_Renderer* renderer, const Font& font, bool useTextureSize);
    [[nodiscard]] const Font& getFont() const;

    [[nodiscard]] const Texture& getTexture() const;

    void setPosition(SDL_FPoint point);
    [[nodiscard]] SDL_FPoint getPosition() const;
    void setSize(SDL_FPoint size);
    [[nodiscard]] SDL_FPoint getSize() const;

    void setForegroundColor(SDL_Renderer* renderer, SDL_Color color);
    [[nodiscard]] const SDL_Color& getForegroundColor() const;

    [[nodiscard]] const SDL_FRect& getRect() const;

private:
    void refreshInternalTexture(SDL_Renderer* renderer);

    Font g_font;
    Texture g_texture;
    std::string g_string;
    SDL_Color g_fgColor;
    SDL_FRect g_rect;
};

}//end cmcs::draw

#endif //_CMCS_DRAW_C_TEXT_HPP_INCLUDED
