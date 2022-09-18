#include "simul_main.hpp"
#include "simul_ext.hpp"
#include "simul_core.hpp"
#include "C_shareableData.hpp"

#include <replxx.hxx>

#include <SDL.h>
#include <SDL_ttf.h>

#include <vector>
#include <list>
#include <string>
#include <iomanip>
#include "simul_math.hpp"
#include <thread>

namespace simul
{

replxx::Replxx* gTerminalPtr{nullptr};

}//end simul

int main(int argv, char** args)
{
    replxx::Replxx terminal;
    terminal.install_window_change_handler();
    simul::gTerminalPtr = &terminal;

    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    TTF_Init();

    simul::SIMUL_STATS simulStat{simul::SIMUL_STAT_IDLE};
    std::thread* simulThread{nullptr};
    bool run = true;
    ///SetConsoleTitle("C51 MicroController Simulator"); TODO

    ShareableData shareableData;
    shareableData.add(&simul::core::gDeviceData, "C8051_data");
    shareableData.add( &terminal, "terminal");

    while (run)
    {
        std::string_view input{ terminal.input("\x1b[1;32minput\x1b[0m> ") };

        std::vector<std::string> inputArguments;
        simul::math::Split(std::string{input}, inputArguments);

        if (inputArguments.empty())
        {
            continue;
        }

        if (inputArguments[0] == "run")
        {
            if (simulStat == simul::SIMUL_STAT_IDLE)
            {
                simulStat = simul::SIMUL_STAT_RUNNING;
                terminal.print(S_PRINT_FORMAT("info", "Executing code, to stop please write \"stop\" ...\n"));
                simul::core::_threadRunStepCode = true;
                simulThread = new std::thread(simul::core::ThreadRunStepCode);
            }
            else
            {
                terminal.print(S_PRINT_FORMAT("info", "The code is already running, to stop please write \"stop\" ...\n"));
            }
        }
        if (inputArguments[0] == "stop")
        {
            if (simulStat == simul::SIMUL_STAT_IDLE)
            {
                terminal.print(S_PRINT_FORMAT("info", "Code execution is already stopped, to run it please write \"run\" ...\n"));
            }
            else
            {
                simulStat = simul::SIMUL_STAT_IDLE;
                terminal.print(S_PRINT_FORMAT("info", "Code execution stopped !\n"));
                simul::core::_threadRunStepCode = false;
                simulThread->join();
                delete simulThread;
            }
        }

        if (simulStat == simul::SIMUL_STAT_IDLE)
        {
            if (inputArguments[0] == "next")
            {
                simul::core::ExecuteStepCode(true);
            }
            if (inputArguments[0] == "read")
            {
                if (inputArguments.size() >= 2)
                {
                    unsigned short argValue = simul::math::GetNumFromString(inputArguments[1]);

                    terminal.print(S_PRINT_FORMAT("info", "Trying to read at <data \"%u\"> :\n"), argValue);

                    try
                    {
                        std::stringstream stream;
                        stream << std::hex << static_cast<unsigned short>(simul::core::gDeviceData._dataRam.at(argValue));

                        terminal.print(S_PRINT_FORMAT("info", "value at $\"%u\" is equal to x%s\n"), argValue, stream.str().c_str());
                    }
                    catch (std::exception& e)
                    {
                        terminal.print(S_PRINT_FORMAT("error", "%s\n"), e.what());
                    }
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "read <arg1: memory index>\n"));
            }
            if (inputArguments[0] == "load")
            {
                if (inputArguments.size() >= 3)
                {
                    if (inputArguments[1] == "hex")
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Loading hex file <\"%s\">:\n"), inputArguments[2].c_str());
                        if ( simul::core::LoadCodeHex(inputArguments[2]) )
                        {
                            terminal.print(S_PRINT_FORMAT("info", "File loaded !\n"));
                        }
                        else
                        {
                            terminal.print(S_PRINT_FORMAT("error", "Invalid file !\n"));
                        }
                        continue;
                    }
                    if (inputArguments[1] == "bin")
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Loading bin file <\"%s\">:\n"), inputArguments[2].c_str());
                        if ( simul::core::LoadCodeBin(inputArguments[2]) )
                        {
                            terminal.print(S_PRINT_FORMAT("info", "File loaded !\n"));
                        }
                        else
                        {
                            terminal.print(S_PRINT_FORMAT("error", "Invalid file !\n"));
                        }
                        continue;
                    }

                    terminal.print(S_PRINT_FORMAT("error", "Unknown file type : %s\n"), inputArguments[1].c_str());
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "load <arg1: type of the file> <arg2: path of the file>\n"));
            }
            if (inputArguments[0] == "export")
            {
                if (inputArguments.size() >= 3)
                {
                    if (inputArguments[1] == "asm")
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Exporting to assembly file <\"%s\">:\n"), inputArguments[2].c_str());
                        if ( simul::core::WriteCodeAsm(inputArguments[2]) )
                        {
                            terminal.print(S_PRINT_FORMAT("info", "File exported !\n"));
                        }
                        else
                        {
                            terminal.print(S_PRINT_FORMAT("error", "Invalid export !\n"));
                        }
                        continue;
                    }

                    terminal.print(S_PRINT_FORMAT("error", "\"Unknown file type : %s\n"), inputArguments[1].c_str());
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "export <arg1: type of the file> <arg2: path of the file>\n"));
            }
            if (inputArguments[0] == "dll")
            {
                if (inputArguments.size() >= 3)
                {
                    if (inputArguments[1] == "load")
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Loading extension DLL <\"%s\">:\n"), inputArguments[2].c_str());
                        if ( simul::ext::LoadExtension(inputArguments[2], shareableData) )
                        {
                            terminal.print(S_PRINT_FORMAT("info", "Extension loaded !\n"));
                        }
                        else
                        {
                            terminal.print(S_PRINT_FORMAT("error", "Invalid extension !\n"));
                        }
                        continue;
                    }
                    if (inputArguments[1] == "unload")
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Unloading extension DLL <\"%s\">:\n"), inputArguments[2].c_str());
                        if ( simul::ext::FreeExtension(inputArguments[2]) )
                        {
                            terminal.print(S_PRINT_FORMAT("info", "Extension unloaded !\n"));
                        }
                        else
                        {
                            terminal.print(S_PRINT_FORMAT("error", "Invalid extension !\n"));
                        }
                        continue;
                    }
                }
                terminal.print(S_PRINT_FORMAT("error", "dll <arg1: load/unload> <arg2: path of the file>\n"));
            }
            if (inputArguments[0] == "set")
            {
                if (inputArguments.size() >= 2)
                {
                    terminal.print(S_PRINT_FORMAT("info", "Loading device <\"%s\">:\n"), inputArguments[1].c_str());
                    if ( simul::core::LoadDevice(inputArguments[1]) )
                    {
                        terminal.print(S_PRINT_FORMAT("info", "Device loaded !\n"));
                    }
                    else
                    {
                        terminal.print(S_PRINT_FORMAT("error", "Invalid device !\n"));
                    }
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "set <arg1: path of the file>\n"));
            }
            if (inputArguments[0] == "write")
            {
                if (inputArguments.size() >= 3)
                {
                    unsigned char addValue = simul::math::GetNumFromString(inputArguments[1]);
                    unsigned char argValue = simul::math::GetNumFromString(inputArguments[2]);

                    terminal.print(S_PRINT_FORMAT("info", "Trying to write at <data \"%u\"> :\n"), addValue);

                    try
                    {
                        simul::core::gDeviceData._dataRam.at(addValue) = argValue;

                        terminal.print(S_PRINT_FORMAT("info", "\"value at $\"%u\" is equal to %u\n"), addValue, simul::core::gDeviceData._dataRam.at(addValue));
                    }
                    catch (std::exception& e)
                    {
                        terminal.print(S_PRINT_FORMAT("error", "%s\n"), e.what());
                    }
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "write <arg1: memory index> <arg2: new value>\n"));
            }
            if (inputArguments[0] == "jump")
            {
                if (inputArguments.size() >= 2)
                {
                    unsigned short argValue = simul::math::GetNumFromString(inputArguments[1]);

                    terminal.print(S_PRINT_FORMAT("info", "\"Jumping to <\"%u\">\"\n"), argValue);

                    simul::core::gDeviceData._cursor = argValue;
                    simul::core::gDeviceData._programCounter = argValue;
                    continue;
                }
                terminal.print(S_PRINT_FORMAT("error", "jump <arg1: new address>\n"));
            }
        }

        if (inputArguments[0] == "exit")
        {
            run = false;
            if (simulStat == simul::SIMUL_STAT_RUNNING)
            {
                simul::core::_threadRunStepCode = false;
                simulThread->join();
                delete simulThread;
            }
        }
    }

    ///Fin

    simul::ext::FreeAll();

    TTF_Quit();
    SDL_Quit();

    return 0;
}
