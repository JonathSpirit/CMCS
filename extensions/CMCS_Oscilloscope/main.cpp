#include "CMCS_API.hpp"

#include "C_text.hpp"
#include "C_sprite.hpp"
#include <replxx.hxx>

#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>

#define KMCS_EXT_MAXSIGNAL 2

#define NUM_OF_PORTS 3

using namespace cmcs;

ShareableData* gCoreData;
std::thread* gThread;
bool gRunning;
/**
typedef void (*CoreFunc_SetPortNum)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPort)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPortType)(unsigned char);
typedef unsigned char& (*CoreFunc_GetPortHide)(unsigned char);

typedef ExtensionIdentity (*FunctionInitCall)( const ExtensionData& );
typedef void (*FunctionUninitCall)(void);
typedef bool (*FunctionUpdateCall)( sf::RenderWindow&, fge::Event&);**/

struct OscSignal
{
    std::string _name;
    SDL_Color _color;
    cmcs::Bits<uint8_t> _data;
    int _posy;

    bool _valueBefore;

    //sf::Clock _timing;
    //sf::Time _timingUp;
    //sf::Time _timingDown;
};

#if 0
bool API KMCSlib_Update( sf::RenderWindow& main_screen, fge::Event& evt )
{
    _myScreen.setActive(true);

    for (unsigned int i=0; i<osc_target.size(); ++i)
    {
        bool actualValue = osc_target[i]._data;

        osc_textTime.setString("Wouaouh");
        //osc_textTime.setString( (std::string)"Th : "+/*fge::string::ToStr(_osc_th_before[pi])+*/"[ms] Tb : "+/*fge::string::ToStr(_osc_tb_before[pi])+*/"[ms] Freq : "+/*ValueToScience(1/(_osc_th_before[pi]*pow(10,-3)+_osc_tb_before[pi]*pow(10,-3)+0.01f))+*/"Hz]" );
        //osc_textTime.setPosition(0,260+22*pi);
        //osc_textTime.setFillColor(_signal_color[pi]);

        for (unsigned int y=0; y<340; ++y)
        {
            for (unsigned int x=0; x<6; ++x)
            {
                osc_graphImg.setPixel((osc_graphPos.x+x)%320, y, sf::Color::Transparent);
            }
        }

        if (actualValue)
        {//haut
            if (osc_target[i]._valueBefore)
            {//ligne droite haut
                osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-20, osc_target[i]._color);
            }
            else
            {//passage vers le haut
                //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                /*_osc_tb_before[pi] = clkDuration.count();
                _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                for (unsigned int y=0; y<20; ++y)
                {
                    osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-y, osc_target[i]._color);
                }
            }
        }
        else
        {//bas
            if (!osc_target[i]._valueBefore)
            {//ligne droite bas
                osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy, osc_target[i]._color);
            }
            else
            {//passage vers le bas
                //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                /*_osc_th_before[pi] = clkDuration.count();
                _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                for (unsigned int y=0; y<20; ++y)
                {
                    osc_graphImg.setPixel(osc_graphPos.x, osc_target[i]._posy-20+y, osc_target[i]._color);
                }
            }
        }

        osc_target[i]._valueBefore = actualValue;
    }
    ++osc_graphPos.x;
    osc_graphPos.x = osc_graphPos.x%320;

    //_myScreen.setPosition(_myScreen.getPosition() - sf::Vector2i(330,0));

    _myScreen.clear();

    _myScreen.draw(osc_spriteBackground);
    osc_graphTexture.loadFromImage(osc_graphImg);
    sf::Sprite osc_graphSprite;
    osc_graphSprite.setTexture(osc_graphTexture);
    _myScreen.draw(osc_graphSprite);

    _myScreen.display();

    _myScreen.setActive(false);
    return true;
}
#endif

void SetPixel(SDL_Surface* surface, int x, int y, uint32_t pixel)
{
    uint32_t* const target_pixel = (uint32_t*) ((uint8_t*) surface->pixels
            + y * surface->pitch
            + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}
void UpdateTexture(SDL_Texture* texture, SDL_Surface* surface)
{
    SDL_Surface* lockedSurface;
    SDL_LockTextureToSurface(texture, nullptr, &lockedSurface);

    SDL_BlitSurface(surface, nullptr, lockedSurface, nullptr);

    SDL_UnlockTexture(texture);
}

void ThreadUpdate()
{
    replxx::Replxx* terminal;
    {
        auto terminalData = gCoreData->get("terminal")->acquirePointer<replxx::Replxx>();
        terminal = terminalData.second;
    }

    terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "info", "hi from CMCS_Oscilloscope extension !\n"));

    auto* window = SDL_CreateWindow("CMCS - Oscilloscope", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 320,340, SDL_WINDOW_OPENGL);
    auto* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "error", "bad renderer: %s\n"), SDL_GetError());
    }

    draw::Texture textureOscilloscope;
    draw::Texture texturePowerOff;
    draw::Texture texturePowerOn;

    if (!textureOscilloscope.loadBmp(renderer, "data/libOscilloscope/images/osc.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!texturePowerOn.loadBmp(renderer, "data/libOscilloscope/images/PowerOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!texturePowerOff.loadBmp(renderer, "data/libOscilloscope/images/PowerOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "error", "bad texture: %s\n"), SDL_GetError());
    }

    draw::Font font;
    if (!font.loadFont(CMCS_MAIN_FONT_PATH, 16))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT("CMCS_Oscilloscope", "error", "bad font: %s\n"), SDL_GetError());
    }

    draw::Texture graphTexture{ SDL_CreateTexture(renderer,
                                                  SDL_PIXELFORMAT_RGBA32,
                                                  SDL_TEXTUREACCESS_STREAMING,
                                                  320, 340) };
    SDL_Surface* graphImg = SDL_CreateRGBSurfaceWithFormat(0, 320,340, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_SetSurfaceBlendMode(graphImg, SDL_BLENDMODE_NONE);
    SDL_FillRect(graphImg, nullptr, SDL_MapRGBA(graphImg->format,0,255,0,0));

    SDL_Point graphPos;
    std::vector<OscSignal> osc_target;

    draw::Sprite spriteBackground;
    spriteBackground.setTexture(textureOscilloscope);

    draw::Sprite spriteGraph;
    spriteGraph.setTexture(graphTexture);

    draw::Sprite spritePower;
    spritePower.setTexture(texturePowerOn);
    spritePower.setPosition({278.0f,348.0f});

    draw::Text textTime;
    textTime.setFont(renderer, font, true);

    auto deviceData = gCoreData->get("C8051_data");
    {
        auto data = deviceData->acquirePointer<device::C8051_data>();

        osc_target.push_back({"test1", SDL_Color{255, 0, 0, 255}, {&data.second->_dataRam[0xA0], 4}, 64});
        osc_target.back()._valueBefore = osc_target.back()._data;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    uint64_t lastTime;
    uint64_t currentTime;

    while (gRunning)
    {
        SDL_Event event;
        while ( SDL_PollEvent(&event) )
        {
            if (event.type == SDL_QUIT)
            {
                gRunning=false;
            }
        }

        currentTime = SDL_GetTicks64();
        uint32_t remainingTime = std::clamp<int64_t>(static_cast<int64_t>(1000/60) - static_cast<int64_t>(currentTime-lastTime), 0, 1000/60);
        lastTime = currentTime;
        SDL_Delay(remainingTime);

        SDL_RenderClear(renderer);

        for (std::size_t i=0; i<osc_target.size(); ++i)
        {
            bool actualValue = osc_target[i]._data;

            textTime.setString(renderer, "Wouaouh");
            //osc_textTime.setString( (std::string)"Th : "+/*fge::string::ToStr(_osc_th_before[pi])+*/"[ms] Tb : "+/*fge::string::ToStr(_osc_tb_before[pi])+*/"[ms] Freq : "+/*ValueToScience(1/(_osc_th_before[pi]*pow(10,-3)+_osc_tb_before[pi]*pow(10,-3)+0.01f))+*/"Hz]" );
            //osc_textTime.setPosition(0,260+22*pi);
            //osc_textTime.setFillColor(_signal_color[pi]);

            for (int y=0; y<340; ++y)
            {
                for (int x=0; x<6; ++x)
                {
                    SetPixel(graphImg, (graphPos.x+x)%320, y, SDL_MapRGBA(graphImg->format,255,255,255,0));
                }
            }

            if (actualValue)
            {//haut
                if (osc_target[i]._valueBefore)
                {//ligne droite haut
                    SetPixel(graphImg, graphPos.x, osc_target[i]._posy-20, SDL_MapRGBA(graphImg->format,
                                                                                       osc_target[i]._color.r,
                                                                                       osc_target[i]._color.g,
                                                                                       osc_target[i]._color.b,
                                                                                       osc_target[i]._color.a));
                }
                else
                {//passage vers le haut
                    //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                    //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                    /*_osc_tb_before[pi] = clkDuration.count();
                    _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                    for (int y=0; y<20; ++y)
                    {
                        SetPixel(graphImg, graphPos.x, osc_target[i]._posy-y, SDL_MapRGBA(graphImg->format,
                                                                                           osc_target[i]._color.r,
                                                                                           osc_target[i]._color.g,
                                                                                           osc_target[i]._color.b,
                                                                                           osc_target[i]._color.a));
                    }
                }
            }
            else
            {//bas
                if (!osc_target[i]._valueBefore)
                {//ligne droite bas
                    SetPixel(graphImg, graphPos.x, osc_target[i]._posy, SDL_MapRGBA(graphImg->format,
                                                                                      osc_target[i]._color.r,
                                                                                      osc_target[i]._color.g,
                                                                                      osc_target[i]._color.b,
                                                                                      osc_target[i]._color.a));
                }
                else
                {//passage vers le bas
                    //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                    //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                    /*_osc_th_before[pi] = clkDuration.count();
                    _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                    for (int y=0; y<20; ++y)
                    {
                        SetPixel(graphImg, graphPos.x, osc_target[i]._posy-20+y, SDL_MapRGBA(graphImg->format,
                                                                                        osc_target[i]._color.r,
                                                                                        osc_target[i]._color.g,
                                                                                        osc_target[i]._color.b,
                                                                                        osc_target[i]._color.a));
                    }
                }
            }

            osc_target[i]._valueBefore = actualValue;
        }
        ++graphPos.x;
        graphPos.x = graphPos.x%320;

        //_myScreen.setPosition(_myScreen.getPosition() - sf::Vector2i(330,0));

        spriteBackground.draw(renderer);
        UpdateTexture(graphTexture.get(), graphImg);
        spriteGraph.draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(graphImg);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

CMCS_API void CMCSlib_Init(ShareableData& data)
{
    gRunning = true;
    gCoreData = &data;
    gThread = new std::thread(ThreadUpdate);
}

CMCS_API ExtensionIdentity CMCSlib_retrieveExtensionIdentity()
{
    return {"CMCS_Oscilloscope", "V1.0", "Guillaume G."};
}

CMCS_API bool CMCSlib_Update()
{
    return true;
}

CMCS_API void CMCSlib_Uninit()
{
    gRunning = false;
    if (gThread->joinable())
    {
        gThread->join();
    }
    delete gThread;
}