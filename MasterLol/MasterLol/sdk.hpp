#ifndef SDK_H
#define SDK_H

#include "detours.h"
#include <vector>
#pragma comment(lib, "detours.lib")
#pragma once

#define fnIssueOrder 0x1C89E0
#define fnMainLoop 0x73F000
#define fnAttackDelay  0x551090
#define fnAttackCastDelay 0x550F80 
#define fnGetBoundingRadius 0x1F68F0 
#define fnCastSpell 0x54C520
#define fnGetSpellData 0x5415F0 
#define fnDrawCircle 0x544B70

#define oRenderer 0x02E41DCC
#define oLocalPlayer 0x2E1DC08
#define oObjectManager 0x2E18B50

#define oUnderMouseObject 0x21BF714
#define oCursor 0x1571784 
#define oGameTime 0x2E15D4C

//IssueOrder : 81 EC B0 ?? ?? ?? 53 8B D9 C7
//MaiLoop    : 83 EC 08 53 8B D9 89 5C 24 04

using namespace std;

enum MoveType
{
	move = 2,
	attack = 3
};

enum ObjectType
{
	Other = 3073,
	Champion = 5121,
	Turret = 9217,
	Inibitor = 131077,
	Missile = 32769
};

enum Teams
{
	Blue = 100,
	Red = 200,
	Neutral = 300
};

class Cursor
{
public:
	char pad_0x0000[0x90]; //0x0000
	Vector vPos; //0x0090 
	char pad_0x009C[0x28]; //0x009C

}; //Size=0x00C4

class cObject
{
public:
	__int32 Base; //0x0000 
	char pad_0x0004[0x10]; //0x0004
	__int32 TeamId; //0x0014 
	__int32 ObjectType; //0x0018 
	char pad_0x001C[0x14]; //0x001C
	__int32 NameLength; //0x0030 
	char pad_0x0034[0xC8]; //0x0034
	Vector vPos; //0x00FC 
	char pad_0x0108[0x570]; //0x0108
	float currHp; //0x0678 
	char pad_0x067C[0xC]; //0x067C
	float maxHp; //0x0688 
	char pad_0x068C[0x358]; //0x068C
	float fAdditionalAD; //0x09E4 
	char pad_0x09E8[0x4]; //0x09E8
	float fAdditionalAP; //0x09EC 
	char pad_0x09F0[0x68]; //0x09F0
	double dAttackSpeed; //0x0A58 
	float fBaseAD; //0x0A60 
	char pad_0x0A64[0x5CC]; //0x0A64
	float fAttackRange; //0x1030 
	char pad_0x1034[0xBC]; //0x1034

}; //Size=0x10F0


class ObjectManager
{
public:
	unsigned char Padding0[0x04]; // 0x00
	int MAX_ENTITY; //0x04
};

class Renderer
{
public:
	char pad_0x0000[0x8C]; //0x0000
	matrix3x4_t viewMatrix; //0x008C 
	matrix3x4_t projMatrix; //0x00CC 

}; //Size=0x010C

class EntityUtils
{
public:

	static cObject* getLocalPlayer()
	{
		DWORD	lolBase = (DWORD)GetModuleHandle(NULL);
		DWORD	pLocalPlayer = *(DWORD*)(lolBase + oLocalPlayer);
		return (cObject*)(pLocalPlayer);
	}

	static double CalcAttackTime() 
	{
		return 1 / (getLocalPlayer()->dAttackSpeed);
	}

	static double CalcWindup() 
	{
		return CalcAttackTime()*0.35;
	}
	static char* GetObjectType(cObject* object)
	{
		switch (object->ObjectType)
		{
		case Champion: return "Champion";
		case Other: return "Other";
		case Inibitor: return "Inibitor";
		case Turret: return "Turret";
		default: return "N/D";
		}
	}

	static bool IsObject(cObject* obj)
	{
		return (obj->ObjectType == Champion) ||
			(obj->ObjectType == Other) ||
			(obj->ObjectType == Turret) ||
			(obj->ObjectType == Inibitor);
	}

	static bool IsTargetable(cObject* obj)
	{
		cObject* localPlayer = EntityUtils::getLocalPlayer();
		return (obj &&
			obj->currHp > 0 &&
			obj->TeamId != localPlayer->TeamId);
	}

	static bool IsInRange(cObject* obj)
	{
		cObject* localPlayer = EntityUtils::getLocalPlayer();
		return (localPlayer->fAttackRange >= (localPlayer->vPos - obj->vPos).Length());
	}

	static void PrintObject(cObject* obj)
	{
		std::cout << "===== obj ===== : " << std::endl;
		std::cout << "Type : " << EntityUtils::GetObjectType(obj) << std::endl;
		std::cout << "Position : " << obj->vPos.x << ", " << obj->vPos.y << ", " << obj->vPos.z << std::endl;
		std::cout << "Hp : " << obj->currHp << "\t Max Hp : " << obj->maxHp << std::endl << std::endl;
	}

	static vector<cObject*> getObjects(DWORD base)
	{
		std::vector<cObject*> listTemp;
		listTemp.clear();

		ObjectManager* pObjManager = (ObjectManager*)(base + oObjectManager);
		cObject** ObjManager = *(cObject***)(base + oObjectManager);
		cObject* localPlayer = EntityUtils::getLocalPlayer();

		for (int i = 0; i < pObjManager->MAX_ENTITY; ++i)
		{
			cObject* obj = ObjManager[i];
			if ((DWORD)obj != 0)
				if (EntityUtils::IsObject(obj))
				{
					listTemp.push_back(obj);
				}
		}
		return listTemp;
	}

	struct bestEntity
	{
		float distance;
		cObject* object;
		bool isNew;
	};
	static bestEntity getClosestObject(std::vector<cObject*> listObjects, bool checkInRange, bool checkTargetable)
	{
		cObject* localPlayer = EntityUtils::getLocalPlayer();
		bestEntity closestEntity;
		closestEntity.distance = FLOAT32_MAX;
		closestEntity.object = listObjects.at(0);

		for (int i = 0; i < listObjects.size(); ++i)
		{
			bestEntity temp;
			cObject* objTemp = listObjects.at(i);
			if (objTemp->Base == localPlayer->Base)
				continue;
			temp.distance = (localPlayer->vPos - objTemp->vPos).Length();
			if (temp.distance < closestEntity.distance)
			{
				if (checkInRange && !EntityUtils::IsInRange(objTemp))
					continue;
				if (checkTargetable && !EntityUtils::IsTargetable(objTemp))
					continue;
				closestEntity.distance = temp.distance;
				closestEntity.object = objTemp;
				closestEntity.isNew = true;

			}
		}
		return closestEntity;
	}

};
#endif

