#include "CMCS_API.hpp"

#include "C_text.hpp"
#include "C_sprite.hpp"
#include "C_surface.hpp"
#include <replxx.hxx>

#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>

#define KMCS_EXT_MAXSIGNAL 2

#define NUM_OF_PORTS 3

#define EXTENSION_NAME "CMCS_Oscilloscope"

using gPinPressedCallback_t = std::function<void(uint8_t*, std::size_t, uint8_t*, std::size_t, SDL_MouseButtonEvent)>;
using gPinPressedCallbacks_t = std::vector<gPinPressedCallback_t>;

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

void ThreadUpdate()
{
    replxx::Replxx* terminal;
    {
        auto terminalData = gCoreData->get("terminal")->acquirePointer<replxx::Replxx>();
        terminal = terminalData.second;
    }

    terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "info", "hi from CMCS_Oscilloscope extension !\n"));

    auto* window = SDL_CreateWindow("CMCS - Oscilloscope", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 320,340, SDL_WINDOW_OPENGL);
    auto* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad renderer: %s\n"), SDL_GetError());
    }

    draw::Texture textureOscilloscope;
    draw::Texture texturePowerOff;
    draw::Texture texturePowerOn;

    if (!textureOscilloscope.loadBmp(renderer, "data/libOscilloscope/images/osc.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!texturePowerOn.loadBmp(renderer, "data/libOscilloscope/images/PowerOn.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }
    if (!texturePowerOff.loadBmp(renderer, "data/libOscilloscope/images/PowerOff.bmp"))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad texture: %s\n"), SDL_GetError());
    }

    draw::Font font;
    if (!font.loadFont(CMCS_MAIN_FONT_PATH, 16))
    {
        terminal->print(CMCS_EXT_PRINT_FORMAT(EXTENSION_NAME, "error", "bad font: %s\n"), SDL_GetError());
    }

    draw::Texture graphTexture{ SDL_CreateTexture(renderer,
                                                  SDL_PIXELFORMAT_RGBA32,
                                                  SDL_TEXTUREACCESS_STREAMING,
                                                  320, 340) };
    draw::Surface graphImg{SDL_CreateRGBSurfaceWithFormat(0, 320,340, 32, SDL_PIXELFORMAT_RGBA32)};
    SDL_SetSurfaceBlendMode(graphImg.get(), SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(graphTexture.get(), SDL_BLENDMODE_BLEND);
    graphImg.fillRect(nullptr, {0,255,0,0});

    SDL_Point graphPos;
    std::vector<OscSignal> signalTargets;
    std::mutex extensionsCallbackMutex;

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

        signalTargets.push_back({"test1", SDL_Color{255, 0, 0, 255}, {&data.second->_dataRam[0xA0], 4}, 64});
        signalTargets.back()._valueBefore = signalTargets.back()._data;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{2000}); ///TODO remove that
    auto functionResetAllPinColor = gCoreData->get("CMCS_GUI:functionResetAllPinColor");
    auto functionSetPinColor = gCoreData->get("CMCS_GUI:functionSetPinColor");

    {
        auto callbacks = gCoreData->get("CMCS_GUI:pinPressedCallbacks");
        if (callbacks)
        {
            auto data = callbacks->acquirePointer<gPinPressedCallbacks_t>();

            data.second->push_back([&](uint8_t* dataStat, std::size_t posStat,
                    uint8_t* dataOutputMod, std::size_t posOutputMod, SDL_MouseButtonEvent event){
                std::scoped_lock<std::mutex> lock{extensionsCallbackMutex};
                if (event.button == SDL_BUTTON_RIGHT)
                {
                    signalTargets.back()._data = {dataStat, posStat};
                    signalTargets.back()._valueBefore = signalTargets.back()._data;
                    auto ptrFunc1 = functionResetAllPinColor->acquirePointer<std::function<void()> >();
                    auto ptrFunc2 = functionSetPinColor->acquirePointer<std::function<void(Bits<uint8_t>, SDL_Color)> >();
                    (*ptrFunc1.second)();
                    (*ptrFunc2.second)(signalTargets.back()._data, signalTargets.back()._color);
                }
            });
        }
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

        extensionsCallbackMutex.lock();
        for (std::size_t i=0; i < signalTargets.size(); ++i)
        {
            bool actualValue = signalTargets[i]._data;

            textTime.setString(renderer, "Wouaouh");
            //osc_textTime.setString( (std::string)"Th : "+/*fge::string::ToStr(_osc_th_before[pi])+*/"[ms] Tb : "+/*fge::string::ToStr(_osc_tb_before[pi])+*/"[ms] Freq : "+/*ValueToScience(1/(_osc_th_before[pi]*pow(10,-3)+_osc_tb_before[pi]*pow(10,-3)+0.01f))+*/"Hz]" );
            //osc_textTime.setPosition(0,260+22*pi);
            //osc_textTime.setFillColor(_signal_color[pi]);

            for (int y=0; y<340; ++y)
            {
                for (int x=0; x<6; ++x)
                {
                    graphImg.setPixel((graphPos.x+x)%320, y, {255,255,255,0});
                }
            }

            if (actualValue)
            {//haut
                if (signalTargets[i]._valueBefore)
                {//ligne droite haut
                    graphImg.setPixel(graphPos.x, signalTargets[i]._posy - 20, signalTargets[i]._color);
                }
                else
                {//passage vers le haut
                    //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                    //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                    /*_osc_tb_before[pi] = clkDuration.count();
                    _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                    for (int y=0; y<20; ++y)
                    {
                        graphImg.setPixel(graphPos.x, signalTargets[i]._posy - y, signalTargets[i]._color);
                    }
                }
            }
            else
            {//bas
                if (!signalTargets[i]._valueBefore)
                {//ligne droite bas
                    graphImg.setPixel(graphPos.x, signalTargets[i]._posy, signalTargets[i]._color);
                }
                else
                {//passage vers le bas
                    //auto timeclk_now = chrono::_V2::high_resolution_clock::now();
                    //millisec_t clkDuration(chrono::duration_cast<millisec_t>(timeclk_now - _osc_time[pi]));

                    /*_osc_th_before[pi] = clkDuration.count();
                    _osc_time[pi] = std::chrono::_V2::high_resolution_clock::now();*/
                    for (int y=0; y<20; ++y)
                    {
                        graphImg.setPixel(graphPos.x, signalTargets[i]._posy - 20 + y, signalTargets[i]._color);
                    }
                }
            }

            signalTargets[i]._valueBefore = actualValue;
        }
        extensionsCallbackMutex.unlock();

        ++graphPos.x;
        graphPos.x = graphPos.x%320;

        //_myScreen.setPosition(_myScreen.getPosition() - sf::Vector2i(330,0));

        spriteBackground.draw(renderer);
        graphTexture.updateTextureFromSurface(graphImg.get());
        spriteGraph.draw(renderer);

        SDL_RenderPresent(renderer);
    }

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
    return {EXTENSION_NAME, "V1.0", "Guillaume G."};
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