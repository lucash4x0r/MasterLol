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
#include <d3d9.h>
#include <d3d9types.h>

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

		DWORD pCursor = *(DWORD*)(lolBase + oCursor);
		Cursor* cursor = (Cursor*)(pCursor);

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
		cout << "Type : " << EntityUtils::GetObjectType(LocalPlayerTemp) << endl;
		cout << "Position : " << LocalPlayerTemp->vPos.x << ", " << LocalPlayerTemp->vPos.y << ", " << LocalPlayerTemp->vPos.z << endl;
		cout << "Hp : " << LocalPlayerTemp->currHp << "\t Max Hp : " << LocalPlayerTemp->maxHp << endl << endl;

		cout << "===== Cursor : =====" << endl;
		cout << "Position : " << cursor->vPos.x << ", " << cursor->vPos.y << ", " << cursor->vPos.z << endl;


		while (!GetAsyncKeyState(VK_END))
		{
			if (GetAsyncKeyState(0x43))//C
			{
				hook.callIssueOrder((DWORD)(lolBase + fnIssueOrder), LocalPlayerTemp, 2, &cursor->vPos, 0, 0, 0, 1);
				cout << "Windup Time : " << EntityUtils::CalcWindup() << endl;
			}

			if (GetAsyncKeyState(VK_V))
			{
				EntityUtils::bestEntity closestObject;
				closestObject.isNew = false;
				vector<cObject*> listObjects = EntityUtils::getObjects(lolBase);
				closestObject = EntityUtils::getClosestObject(listObjects, true, true);
				if (closestObject.object != 0 && closestObject.isNew && EntityUtils::IsTargetable(closestObject.object))
				{
					cout << endl << " ==ClosestObject ==" << endl;
					cout << "Distance from player : " << closestObject.distance << "m" << endl;
					EntityUtils::PrintObject(closestObject.object);
					hook.callIssueOrder((DWORD)(lolBase + fnIssueOrder), LocalPlayerTemp, MoveType::attack,
						&closestObject.object->vPos, closestObject.object, 0, 0, 1);
				}
				else
				{
					hook.callIssueOrder((DWORD)(lolBase + fnIssueOrder), LocalPlayerTemp, 2, &cursor->vPos, 0, 0, 0, 1);
				}
			}

			if (GetAsyncKeyState(VK_HOME))
			{
				cObject** ObjManager = *(cObject***)(lolBase + oObjectManager);
				for (int i = 0; i < 10000; ++i)
				{
					cObject* obj = ObjManager[i];
					if ((DWORD)obj != 0)
						if (EntityUtils::IsObject(obj))
						{
							EntityUtils::PrintObject(obj);
						}
				}
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
