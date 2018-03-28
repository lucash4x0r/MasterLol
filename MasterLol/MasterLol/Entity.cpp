#include "Entity.h"

cObject* Entity::getLocalPlayer()
{
	DWORD	lolBase = (DWORD)GetModuleHandle(NULL);
	DWORD	pLocalPlayer = *(DWORD*)(lolBase + oLocalPlayer);
	return (cObject*)(pLocalPlayer);
}

//static double CalcAttackTime()
//{
//	return 1 / (getLocalPlayer()->dAttackSpeed);
//}
//
//static double CalcWindup()
//{
//	return CalcAttackTime()*0.35;
//}

char* Entity::GetObjectType(cObject* object)
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

bool Entity::IsObject(cObject* obj)
{
	return (obj->ObjectType == Champion) ||
		(obj->ObjectType == Other) ||
		(obj->ObjectType == Turret) ||
		(obj->ObjectType == Inibitor);
}

bool Entity::IsTargetable(cObject* obj)
{
	cObject* localPlayer = Entity::getLocalPlayer();
	return (obj &&
		obj->currHp > 0 &&
		obj->TeamId != localPlayer->TeamId);
}

bool Entity::IsInRange(cObject* obj)
{
	cObject* localPlayer = Entity::getLocalPlayer();
	return (localPlayer->fAttackRange >= (localPlayer->vPos - obj->vPos).Length());
}

void Entity::PrintObject(cObject* obj)
{
	std::cout << "===== obj ===== : " << std::endl;
	std::cout << "Type : " << Entity::GetObjectType(obj) << std::endl;
	std::cout << "Position : " << obj->vPos.x << ", " << obj->vPos.y << ", " << obj->vPos.z << std::endl;
	std::cout << "Hp : " << obj->currHp << "\t Max Hp : " << obj->maxHp << std::endl << std::endl;
}

vector<cObject*> Entity::getObjects(DWORD base)
{
	std::vector<cObject*> listTemp;
	listTemp.clear();

	ObjectManager* pObjManager = (ObjectManager*)(base + oObjectManager);
	cObject** ObjManager = *(cObject***)(base + oObjectManager);
	cObject* localPlayer = Entity::getLocalPlayer();

	for (int i = 0; i < pObjManager->MAX_ENTITY; ++i)
	{
		cObject* obj = ObjManager[i];
		if ((DWORD)obj != 0)
			if (Entity::IsObject(obj))
			{
				listTemp.push_back(obj);
			}
	}
	return listTemp;
}


bestEntity Entity::getClosestObject(std::vector<cObject*> listObjects, bool checkInRange, bool checkTargetable)
{
	cObject* localPlayer = Entity::getLocalPlayer();
	bestEntity closestEntity;
	closestEntity.distance = std::numeric_limits<float>::max();
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
			if (checkInRange && !Entity::IsInRange(objTemp))
				continue;
			if (checkTargetable && !Entity::IsTargetable(objTemp))
				continue;
			closestEntity.distance = temp.distance;
			closestEntity.object = objTemp;
			closestEntity.isNew = true;

		}
	}
	return closestEntity;
}
