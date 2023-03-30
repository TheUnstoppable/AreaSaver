/*
	AreaSaver
	Copyright (C) 2023 Unstoppable

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
*/

#pragma once

#include "da_event.h"
#include "da_gameobj.h"
#include "da_player.h"

#define AREASAVER_FORMATVERSION (uint32)1

#define AREASAVERSCHEMATIC_LOCATION "AreaSaverSchematics"
#define AREASAVERSCHEMATIC_EXTENSION ".sch"
#define LRES AreaSaverPluginClass::LoadResult
#define SRES AreaSaverPluginClass::SaveResult

typedef enum {
	CHUNK_AREASAVR = 0x52565341,
	CHUNK_SAVEINFO = 0x464E4953,
	CHUNK_SAVEOBJS = 0x4A424F53,
	MCHUNK_NUMOBJS = 0x00000043,
	MCHUNK_PLYRINF = 0x00000050,
	MCHUNK_MAPINFO = 0x0000004D,
	MCHUNK_DEFINFO = 0x00000044,
	MCHUNK_PHYINFO = 0x00000050,
	MCHUNK_OBJINFO = 0x0000004F,
	MCHUNK_SCRIPTS = 0x00000053,
	MCHUNK_WEAPONS = 0x00000057
};

struct AreaSaverWeaponStruct {
	int weaponDefId;
	int clipRounds;
	int invRounds;
	bool selected;
};

class AreaSaverFlareDummyObserverClass : public DAGameObjObserverClass {
private:
	cPlayer* FlareOwner;

public:
	const char* Get_Name() override { return "AreaSaverFlareDummyObserverClass"; }

	cPlayer* Get_Flare_Owner() const { return FlareOwner; }
	void Set_Flare_Owner(cPlayer* owner) { FlareOwner = owner; }
};

class AreaSaverPlayerDataClass : public DAPlayerDataClass {
private:
	Vector3 FirstPos;
	Vector3 SecondPos;
	bool MarkerEnabled;

public:
	void Init() override;
	void Clear_Level() override;

	const Vector3& Get_First_Position() const { return FirstPos; }
	const Vector3& Get_Second_Position() const { return SecondPos; }
	bool Is_Marker_Enabled() const { return MarkerEnabled; }
	void Set_First_Position(const Vector3& pos) { FirstPos = pos; }
	void Set_Second_Position(const Vector3& pos) { SecondPos = pos; }
	void Set_Marker_Enabled(bool enable) { MarkerEnabled = enable; }
};

class AreaSaverPluginClass : public DAEventClass, public DAPlayerDataManagerClass<AreaSaverPlayerDataClass> {
	typedef enum {
		LOAD_SUCCESSFUL,
		LOAD_FILE_NOT_FOUND,
		LOAD_FILE_NOT_OPENED,
		LOAD_FORMAT_INCOMPATIBLE,
		LOAD_MAX
	} LoadResult;

	typedef enum {
		SAVE_SUCCESSFUL,
		SAVE_NO_REGION,
		SAVE_FILE_NOT_CREATED,
		SAVE_MAX
	} SaveResult;

public:
	AreaSaverPluginClass();

	bool Load_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool Save_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool SaveOverwrite_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool FirstPosition_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool SecondPosition_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool SelectMapBounds_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool HighlightBounds_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);
	bool ClearHighlight_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType);

protected:
	void Collect_Objects(cPlayer* Player, DynamicVectorClass<GameObject*>& Objects);
	bool Schematic_Exists(const StringClass& Name);
	LoadResult Load_Schematic(cPlayer* Player, const StringClass& Name, int* LoadedObjects);
	SaveResult Save_Schematic(cPlayer* Player, const StringClass& Name, int* SavedObjects);
	void Create_Marker_Object(cPlayer* Player, Vector3 Position);
	void Clear_Player_Markers(cPlayer* Player);
};

