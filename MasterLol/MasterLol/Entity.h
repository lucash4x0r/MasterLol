#pragma once
#include <vector>

#include "sdk.h"

struct bestEntity
{
	float distance;
	cObject* object;
	bool isNew;
};

class Entity
{
public:
	static cObject* getLocalPlayer();
	static char* GetObjectType(cObject* object);
	static bool IsObject(cObject* obj);
	static bool IsTargetable(cObject* obj);
	static bool IsInRange(cObject* obj);
	static void PrintObject(cObject* obj);
	static vector<cObject*> getObjects(DWORD base);
	static bestEntity getClosestObject(std::vector<cObject*> listObjects,
		bool checkInRange, bool checkTargetable);
};


