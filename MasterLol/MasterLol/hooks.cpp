#include "hooks.h"

using namespace std;

DWORD DrawCircleAddr = 0;
void DrawCircle(Vector * position, float range, int * color, int a4, float a5, int a6, float alpha)
{
#pragma region PRINT_DRAW_CIRCLE_ARGS
	/*cout << "Position : " << position->x << ", " << position->y << ", " << position->z << endl;
	cout << "range : " << range << endl;
	cout << "color : " << color << endl;
	cout << "a4 : " << a4 << endl;
	cout << "a5 : " << a5 << endl;
	cout << "a6 : " << a6 << endl;
	cout << "alpha : " << alpha << endl;*/
#pragma endregion

	_fnDrawCircle originalDrawCircle = (_fnDrawCircle)DrawCircleAddr;
	return originalDrawCircle(position, range, color, a4, a5, a6, alpha); //a3 = color
}

DWORD IssueOrderAddr = 0;
cObject* LocalPlayer;
void IssueOrder(/*cObject* localPlayer, */int dwOrder, Vector* TargetPos,
	cObject* TargetPtr, bool attackLoc, bool isPassive, int NetWorkId)
{
#pragma region PRINT_ISSUE_ORDER_ARGS
	cout << "Player : " << &LocalPlayer << endl;
	cout << "dwOrder : " << dwOrder << endl;
	cout << "TargetPos : " << TargetPos->x << ", " << TargetPos->y << ", " << TargetPos->z << endl;
	cout << "attackLoc : " << attackLoc << endl;
	cout << "isPassive : " << isPassive << endl;
	cout << "NetWorkId : " << NetWorkId << endl;
#pragma endregion

	_fnIssueOrder originalIssueOrder = (_fnIssueOrder)IssueOrderAddr;
	return originalIssueOrder(LocalPlayer, dwOrder, TargetPos, TargetPtr, attackLoc, isPassive, NetWorkId);
}

hooks::hooks()
{
}


hooks::~hooks()
{
}


void hooks::hookDrawCircle(DWORD base)
{
	DrawCircleAddr = (DWORD)(base + fnDrawCircle);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	// this will hook the function
	DetourAttach(&(LPVOID&)DrawCircleAddr, &DrawCircle);
	DetourTransactionCommit();
}

void hooks::unHookDrawCircle()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// this will hook the function
	DetourDetach(&(LPVOID&)DrawCircleAddr, &DrawCircle);

	DetourTransactionCommit();
}

void hooks::hookIssueOrder(DWORD base)
{
	IssueOrderAddr = (DWORD)(base + fnIssueOrder);

	DWORD pLocalPlayer = *(DWORD*)(base + oLocalPlayer);
	LocalPlayer = (cObject*)(pLocalPlayer);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	// this will hook the function
	DetourAttach(&(LPVOID&)IssueOrderAddr, &IssueOrder);
	DetourTransactionCommit();
}

void hooks::unHookIssueOrder()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	// this will hook the function
	DetourDetach(&(LPVOID&)IssueOrderAddr, &IssueOrder);
	DetourTransactionCommit();
}

void hooks::callIssueOrder(DWORD addr, cObject* Player, int dwOrder, Vector* TargetPos,
	cObject* TargetPtr, bool attackLoc, bool isPassive, int NetWorkId) 
{
	IssueOrderAddr = addr;
	_fnIssueOrder originalIssueOrder = (_fnIssueOrder)IssueOrderAddr;
	return originalIssueOrder(Player, dwOrder, TargetPos, TargetPtr, attackLoc, isPassive, NetWorkId);
}

void hooks::hookAll(DWORD base)
{
	hookDrawCircle(base);
	//hookIssueOrder(base);
}

void hooks::unHookAll()
{
	unHookDrawCircle();
	//unHookIssueOrder();
}

