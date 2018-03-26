#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/sdk.hpp"

namespace Utils
{
    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
    void AttachConsole();

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole();

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...);

	bool NextLine();
    
    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey();

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::uint8_t* PatternScan(void* module, const char* signature);

}
