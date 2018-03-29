#define NOMINMAX
#include <Windows.h>

#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include <string.h>
#include <sstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>

#pragma comment(lib, "detours.lib")
#include <d3d.h>
#include <d3d9.h>
#include <d3d9types.h>

#include "sdk.h"
#include "hooks.h"
#include "helpers\utils.hpp"
#include"Entity.h"

using namespace std;

bool g_Unload = false;

DWORD WINAPI OnDllAttach(LPVOID base)
{
	try {
#ifdef _DEBUG
		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());
		freopen("CONOUT$", "w", stdout);
#endif
		cout << "Initialisation ..." << endl;

		Utils::Init();
		Utils::Dump();

		Utils::Test();

		hooks hook = hooks();
		hook.hookAll(Utils::LolBase);

		while (!GetAsyncKeyState(VK_END)) //Panic key
		{
			if (GetAsyncKeyState(VK_C))//C
			{
				Utils::MoveTo(MoveType::move, 0);
				//cout << "Windup Time : " << EntityUtils::CalcWindup() << endl;
			}

			if (GetAsyncKeyState(VK_V))
			{
				bestEntity closestObject;
				closestObject.isNew = false;
				vector<cObject*> listObjects = Entity::getObjects(Utils::LolBase);
				closestObject = Entity::getClosestObject(listObjects, true, true);
				if (closestObject.object != 0 && closestObject.isNew && Entity::IsTargetable(closestObject.object))
				{
					//Entity::PrintObject(closestObject.object);
					Utils::MoveTo(MoveType::attack, closestObject.object);
				}
				else
				{
					Utils::MoveTo(MoveType::move, 0);
				}
			}

			if (GetAsyncKeyState(VK_HOME))
			{
				vector<cObject*> listObjects = Entity::getObjects(Utils::LolBase);
				cout << endl << "Number of objects : " << listObjects.size() << endl;
			}

			Sleep(150);
		}

		Beep(523, 250);
		FreeConsole();
		hook.unHookAll();
		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
	catch (const std::exception& ex) {
		cout << "Error";
		FreeConsole();

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
	Utils::DetachConsole();
#endif

	return TRUE;
}

BOOL WINAPI DllMain(
	_In_      HINSTANCE hinstDll,
	_In_      DWORD     fdwReason,
	_In_opt_  LPVOID    lpvReserved
)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDll);
		CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpvReserved == nullptr)
			return OnDllDetach();
		return TRUE;
	default:
		return TRUE;
	}
}
