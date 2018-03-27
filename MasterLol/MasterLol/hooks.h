#pragma once
#include "helpers/utils.hpp"
#include "detours.h"
#include "sdk.hpp"

typedef void(__cdecl* _fnDrawCircle)(Vector * position, float range, int * color, int a4, float a5, int a6, float alpha);

typedef int(__stdcall* _fnMainLoop) (int a1);

typedef void(__thiscall* _fnIssueOrder)(cObject* Player, int dwOrder, Vector* TargetPos,
									   cObject* TargetPtr, bool attackLoc, bool isPassive, int NetWorkId);

class hooks
{
public:
	hooks();
	~hooks();
	void hookDrawCircle(DWORD base);
	void unHookDrawCircle();

	void hookIssueOrder(DWORD base);
	void unHookIssueOrder();
	void callIssueOrder(DWORD addr, cObject* Player, int dwOrder, Vector* TargetPos,
		cObject* TargetPtr, bool attackLoc, bool isPassive, int NetWorkId);

	void hookMainLoop(DWORD base);
	void unHookMainLoop();

	void hookAll(DWORD base);
	void unHookAll();
};

