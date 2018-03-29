#pragma once

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <initializer_list>
#include <vector>
#include <iomanip>

#include "..\sdk.h"
#include "..\Entity.h"
#include "..\hooks.h"
#include <Psapi.h>
#include <d3d9.h>





namespace Utils
{
	extern DWORD LolBase;
	extern DWORD pLocalPlayer;
	extern DWORD pCursor;

	void Init();
	void Dump();

	Cursor* getCursor();

	void MoveTo(MoveType moveType, cObject* target);

	void Test();
	bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);
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
