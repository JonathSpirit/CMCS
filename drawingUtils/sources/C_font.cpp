#include "C_font.hpp"

namespace cmcs::draw
{

Font::Font() = default;

Font::Font(TTF_Font* newFont)
{
    this->set(newFont);
}

bool Font::loadFont(const char* path, int ptsize)
{
    TTF_Font* font = TTF_OpenFont(path, ptsize);
    if (font == nullptr)
    {
        return false;
    }
    this->set(font);
    return true;
}

void Font::set(TTF_Font* font)
{
    this->g_font.reset(font, Font::fontDeleter);
}
TTF_Font* Font::get() const
{
    return this->g_font.get();
}

void Font::fontDeleter(TTF_Font* font)
{
    if (font != nullptr)
    {
        TTF_CloseFont(font);
    }
}

}//end cmcs::draw