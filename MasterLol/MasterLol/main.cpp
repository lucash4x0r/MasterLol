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

#include "sdk.hpp"
#pragma comment(lib, "detours.lib")
#include <d3d.h>

#include "hooks.h"

using namespace std;

bool g_Unload = false;

cObject* LocalPlayerTemp;

DWORD WINAPI OnDllAttach(LPVOID base)
{
	try {
#ifdef _DEBUG
		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());
		freopen("CONOUT$", "w", stdout);
#endif
		cout << "Initialisation ..." << endl;


		DWORD lolBase = (DWORD)GetModuleHandle(NULL);

		hooks hook = hooks();
		hook.hookAll(lolBase);

		ObjectManager* objManager = (ObjectManager*)(lolBase + oObjectManager);

		DWORD pLocalPlayer = *(DWORD*)(lolBase + oLocalPlayer);
		LocalPlayerTemp = (cObject*)(pLocalPlayer);

		cout << "League of Legends base is : ";
		cout << showbase // show the 0x prefix
			<< internal // fill between the prefix and the number
			<< setfill('0'); // fill with 0s

		cout << hex << lolBase << dec << endl; //Print LolBase address

		cout << "===== Object Manager =====" << endl;
		cout << "Max Entities : " << objManager->MAX_ENTITY << endl;

		cout << "===== LocalPlayer ===== : " << endl;
		cout << "Type : " << Entity::GetObjectType(LocalPlayerTemp) << endl;
		cout << "Position : " << LocalPlayerTemp->vPos.x << ", " << LocalPlayerTemp->vPos.y << ", " << LocalPlayerTemp->vPos.z << endl;
		cout << "Hp : " << LocalPlayerTemp->currHp << "\t Max Hp : " << LocalPlayerTemp->maxHp << endl << endl;


		while (!GetAsyncKeyState(VK_END))
		{
			if (GetAsyncKeyState(VK_PRIOR))//PAGE UP key
			{
				Vector* vec = new Vector();
				vec->x = 566;
				vec->y = 183;
				vec->z = 556;
				hook.callIssueOrder((DWORD)(lolBase + fnIssueOrder), LocalPlayerTemp, 2, vec, 0, 0, 0, 1);
				//hkDrawCircle fn_drawcircle = (hkDrawCircle)(lolBase + fnDrawCircle);//);

				//D3DCOLOR colorYellow = D3DCOLOR_RGBA(50, 50, 0, 1);

				//fn_drawcircle(&LocalPlayer->vPos, (float)(615), (int*)&colorYellow, 0, 0, 0, 1);
				//Sleep(250);
			}

			if (GetAsyncKeyState(VK_HOME))
			{
				cObject** ObjManager = *(cObject***)(lolBase + oObjectManager);
				for (int i = 0; i < 10000; ++i)
				{
					cObject* obj = ObjManager[i];
					if ((DWORD)obj != 0)
						if (Entity::IsObject(obj))
						{
							Entity::PrintObject(obj);
						}
				}
			}

			Sleep(250);
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
