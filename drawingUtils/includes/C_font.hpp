#ifndef _CMCS_DRAW_C_FONT_HPP_INCLUDED
#define _CMCS_DRAW_C_FONT_HPP_INCLUDED

#include "drawingUtilsExtern.hpp"
#include <SDL_ttf.h>
#include <memory>

namespace cmcs::draw
{

class CMCS_DRAWINGUTILS_API Font
{
public:
    Font();
    explicit Font(TTF_Font* newFont);

    bool loadFont(const char* path, int ptsize);

    void set(TTF_Font* font);
    [[nodiscard]] TTF_Font* get() const;

private:
    static void fontDeleter(TTF_Font* font);
    std::shared_ptr<TTF_Font> g_font;
};

}//end cmcs::draw

#endif //_CMCS_DRAW_C_FONT_HPP_INCLUDED
