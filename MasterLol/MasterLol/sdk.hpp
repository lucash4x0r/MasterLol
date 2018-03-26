#include "detours.h"
#pragma comment(lib, "detours.lib")
#pragma once

#define fnIssueOrder 0x1C8E00
#define fnMainLoop 0x73F7D0
#define fnAttackDelay  0x550750
#define fnAttackCastDelay 0x550640
#define fnGetBoundingRadius 0x1F67D0 
#define fnCastSpell 0x54BD40
#define fnGetSpellData 0x540EB0
#define fnDrawCircle 0x544410 

#define oRenderer 0x02E41DCC
#define oLocalPlayer 0x2E1DC08
#define oObjectManager 0x2E18B50

#define oUnderMouseObject 0x21BF714
#define oCursor 0x1571784 
#define oGameTime 0x21BF73C

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
	__int32 Base; //0x00
	char pad_0x0000[0x10]; //0x04
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
	char pad_0x068C[0x5F4]; //0x068C

}; //Size=0x0C80


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

class Entity
{
public:
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

	static void PrintObject(cObject* obj)
	{
		std::cout << "===== obj ===== : " << std::endl;
		std::cout << "Type : " << Entity::GetObjectType(obj) << std::endl;
		std::cout << "Position : " << obj->vPos.x << ", " << obj->vPos.y << ", " << obj->vPos.z << std::endl;
		std::cout << "Hp : " << obj->currHp << "\t Max Hp : " << obj->maxHp << std::endl << std::endl;
	}

};

