#ifndef _CMCSGUI_C_PIN_HPP_INCLUDED
#define _CMCSGUI_C_PIN_HPP_INCLUDED

#include "C_sprite.hpp"
#include "C_bit.hpp"
#include <algorithm>

class Pin
{
public:
    Pin(uint8_t* dataStat, std::size_t posStat,
        uint8_t* dataOutputMod, std::size_t posOutputMod);
    Pin(const SDL_FPoint& pos, uint8_t* dataStat, std::size_t posStat,
        uint8_t* dataOutputMod, std::size_t posOutputMod);

    bool update(const SDL_Event& event);
    void draw(SDL_Renderer* renderer) const;

    void setPosition(const SDL_FPoint& pos);
    SDL_FPoint getPosition() const;

    void setLedTextures(const cmcs::draw::Texture& ledOn, const cmcs::draw::Texture& ledOff);
    void setSwitchTextures(const cmcs::draw::Texture& switchOn, const cmcs::draw::Texture& switchOff);

    void setStat(bool stat);
    bool getStat() const;

    cmcs::Bits<uint8_t>& getBitsStat();
    cmcs::Bits<uint8_t>& getBitsOutputMod();

    void setColor(SDL_Color color);

    void setOutputMod(bool mod);

private:
    mutable cmcs::draw::Sprite g_sprite;

    cmcs::draw::Texture g_textureLedOn;
    cmcs::draw::Texture g_textureLedOff;
    cmcs::draw::Texture g_textureSwitchOn;
    cmcs::draw::Texture g_textureSwitchOff;

    bool g_statMouseOn = false;
    cmcs::Bits<uint8_t> g_stat;
    cmcs::Bits<uint8_t> g_outputMod;

    SDL_Color g_color;

    bool g_flag = false;
};

#endif // _CMCSGUI_C_PIN_HPP_INCLUDED
