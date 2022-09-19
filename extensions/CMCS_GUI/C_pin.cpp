#include "C_pin.hpp"

Pin::Pin(uint8_t* dataStat, std::size_t posStat,
         uint8_t* dataOutputMod, std::size_t posOutputMod) :
         g_stat(dataStat, posStat),
         g_outputMod(dataOutputMod, posOutputMod)
{
}
Pin::Pin(const SDL_FPoint& pos, uint8_t* dataStat, std::size_t posStat,
         uint8_t* dataOutputMod, std::size_t posOutputMod) :
         Pin(dataStat, posStat, dataOutputMod, posOutputMod)
{
    this->setPosition(pos);
}

bool Pin::update(const SDL_Event& event)
{
    bool returnTrueIfPressed = false;

    if (event.type == SDL_MOUSEMOTION)
    {
        SDL_FPoint mousePos = {(float)event.motion.x, (float)event.motion.y};

        SDL_FRect rect = this->g_sprite.getRect();

        if ( SDL_PointInFRect(&mousePos, &rect) )
        {
            this->g_statMouseOn = true;
        }
        else
        {
            this->g_statMouseOn = false;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (!this->g_flag)
        {
            this->g_flag = true;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if ( this->g_statMouseOn )
                {
                    this->g_stat = !this->g_stat;
                }
            }
            returnTrueIfPressed = true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            this->g_flag = false;
        }
    }

    if (this->g_statMouseOn)
    {
        this->g_sprite.setColorMod({180, 180, 180, 255});
    }
    else
    {
        this->g_sprite.setColorMod({255, 255, 255, 255});
    }

    return returnTrueIfPressed;
}
void Pin::draw(SDL_Renderer* renderer) const
{
    if (this->g_outputMod)
    {
        this->g_sprite.setTexture(this->g_stat ? this->g_textureLedOn : this->g_textureLedOff);
    }
    else
    {
        this->g_sprite.setTexture(this->g_stat ? this->g_textureSwitchOn : this->g_textureSwitchOff);
    }

    this->g_sprite.draw(renderer);
}

void Pin::setPosition(const SDL_FPoint& pos)
{
    this->g_sprite.setPosition(pos);
}
SDL_FPoint Pin::getPosition() const
{
    return this->g_sprite.getPosition();
}

void Pin::setLedTextures(const cmcs::draw::Texture& ledOn, const cmcs::draw::Texture& ledOff)
{
    this->g_textureLedOn = ledOn;
    this->g_textureLedOff = ledOff;
}
void Pin::setSwitchTextures(const cmcs::draw::Texture& switchOn, const cmcs::draw::Texture& switchOff)
{
    this->g_textureSwitchOn = switchOn;
    this->g_textureSwitchOff = switchOff;
}

void Pin::setStat(bool stat)
{
    this->g_stat = stat;
}
bool Pin::getStat() const
{
    return this->g_stat;
}

cmcs::Bits<uint8_t>& Pin::getBitsStat()
{
    return this->g_stat;
}
cmcs::Bits<uint8_t>& Pin::getBitsOutputMod()
{
    return this->g_outputMod;
}

void Pin::setOutputMod(bool mod)
{
    this->g_outputMod = mod;
}