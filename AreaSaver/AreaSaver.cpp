/*
	AreaSaver
	Copyright (C) 2023 Unstoppable
	
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
*/

#include "General.h"
#include "da.h"
#include "engine.h"
#include "engine_da.h"
#include "AreaSaver.h"
#include "GameObjManager.h"
#include "MoveablePhysClass.h"
#include "PhysicsSceneClass.h"
#include "RawFileClass.h"

/************************************************************/

RENEGADE_FUNCTION
void ScriptableGameObj::Start_Observers()
AT2(0x006B6C40, 0x006B64E0);

bool AABoxClass::Contains(const Vector3& point) const {
	return Is_Inside_AABox(*this, point);
}

/************************************************************/

AreaSaverPluginClass::AreaSaverPluginClass() {
	DAPlayerDataManagerClass<AreaSaverPlayerDataClass>::Init();

	Register_Chat_Command((DAECC)&AreaSaverPluginClass::FirstPosition_ChatCommand, "!firstpos|!pos1", 0, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::SecondPosition_ChatCommand, "!secondpos|!pos2", 0, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::Load_ChatCommand, "!sload", 1, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::Save_ChatCommand, "!ssave", 1, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::SaveOverwrite_ChatCommand, "!ssaveoverwrite", 1, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::SelectMapBounds_ChatCommand, "!selectmapbounds|!smb", 0, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::HighlightBounds_ChatCommand, "!highlightbounds|!hlo|!highlightarea|!hla", 0, DAAccessLevel::ADMINISTRATOR);
	Register_Chat_Command((DAECC)&AreaSaverPluginClass::ClearHighlight_ChatCommand, "!clearhighlight|!chl", 0, DAAccessLevel::ADMINISTRATOR);
}

bool AreaSaverPluginClass::Load_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	if (!Player->Get_GameObj()) {
		DA::Page_Player(Player, "Player object could not be found.");
		return false;
	}

	int loaded = 0;
	switch (Load_Schematic(Player, Text[0], &loaded)) {
		case LRES::LOAD_SUCCESSFUL: {
			DA::Page_Player(Player, "Successfully loaded %d object(s) from the schematic file.", loaded);
			break;
		}
		case LRES::LOAD_FILE_NOT_FOUND: {
			DA::Page_Player(Player, "Specified schematic file could not be found.");
			break;
		}
		case LRES::LOAD_FILE_NOT_OPENED: {
			DA::Page_Player(Player, "Specified schematic file could not be opened.");
			break;
		}
		case LRES::LOAD_FORMAT_INCOMPATIBLE: {
			DA::Page_Player(Player, "The format version of the specified schematic file is not compatible with this version of AreaSaver.");
			break;
		}
	}
	return false;
}

bool AreaSaverPluginClass::Save_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	if (!Player->Get_GameObj()) {
		DA::Page_Player(Player, "Player object could not be found.");
		return false;
	}

	if (Schematic_Exists(Text[0])) {
		DA::Page_Player(Player, "Specified schematic file already exists. Please use !ssaveoverwrite to overwrite the existing schematic file.");
		return false;
	}

	int saved = 0;
	switch (Save_Schematic(Player, Text[0], &saved)) {
		case SRES::SAVE_SUCCESSFUL: {
			DA::Page_Player(Player, "Successfully saved %d object(s).", saved);
			break;
		}
		case SRES::SAVE_FILE_NOT_CREATED: {
			DA::Page_Player(Player, "Could not open the specified file.");
			break;
		}
		case SRES::SAVE_NO_REGION: {
			DA::Page_Player(Player, "No region was selected. Please determine a region using !pos1 and !pos2 first.");
			break;
		}
	}
	return false;
}

bool AreaSaverPluginClass::SaveOverwrite_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	if (!Player->Get_GameObj()) {
		DA::Page_Player(Player, "Player object could not be found.");
		return false;
	}

	int saved = 0;
	switch (Save_Schematic(Player, Text[0], &saved)) {
		case SRES::SAVE_SUCCESSFUL: {
			DA::Page_Player(Player, "Successfully saved %d object(s).", saved);
			break;
		}
		case SRES::SAVE_FILE_NOT_CREATED: {
			DA::Page_Player(Player, "Could not open the specified file.");
			break;
		}
		case SRES::SAVE_NO_REGION: {
			DA::Page_Player(Player, "No region was selected. Please determine a region using !pos1 and !pos2 first.");
			break;
		}
	}
	return false;
}

bool AreaSaverPluginClass::FirstPosition_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		DA::Page_Player(Player, "Player data could not be found.");
		return false;
	}

	if (!Player->Get_GameObj()) {
		DA::Page_Player(Player, "Player object could not be found.");
		return false;
	}

	Vector3 pos = Commands->Get_Position(Player->Get_GameObj());
	Data->Set_First_Position(pos);

	DA::Page_Player(Player, "First position has been set to X: %f, Y: %f, Z: %f", pos.X, pos.Y, pos.Z);
	return false;
}

bool AreaSaverPluginClass::SecondPosition_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		DA::Page_Player(Player, "Player data could not be found.");
		return false;
	}

	if (!Player->Get_GameObj()) {
		DA::Page_Player(Player, "Player object could not be found.");
		return false;
	}

	Vector3 pos = Commands->Get_Position(Player->Get_GameObj());
	Data->Set_Second_Position(pos);

	DA::Page_Player(Player, "Second position has been set to X: %f, Y: %f, Z: %f", pos.X, pos.Y, pos.Z);
	return false;
}

bool AreaSaverPluginClass::SelectMapBounds_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		DA::Page_Player(Player, "Player data could not be found.");
		return false;
	}

	Vector3 min, max;
	PhysicsSceneClass::Get_Instance()->Get_Level_Extents(min, max);
	Data->Set_First_Position(min);
	Data->Set_Second_Position(max);

	DA::Page_Player(Player, "First position has been set to X: %f, Y: %f, Z: %f", min.X, min.Y, min.Z);
	DA::Page_Player(Player, "Second position has been set to X: %f, Y: %f, Z: %f", max.X, max.Y, max.Z);
	return false;
}

bool AreaSaverPluginClass::HighlightBounds_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		DA::Page_Player(Player, "Player data could not be found.");
		return false;
	}

	if (Data->Is_Marker_Enabled()) {
		Clear_Player_Markers(Player);
	}

	if (Data->Get_First_Position().Length2() == 0.f || Data->Get_Second_Position().Length2() == 0.f) {
		DA::Page_Player(Player, "No region was selected. Please determine a region using !pos1 and !pos2 first.");
		return false;
	}

	Vector3 firstPos = Data->Get_First_Position(), secondPos = Data->Get_Second_Position();

	// Create markers at the corners.
	Create_Marker_Object(Player, firstPos);
	Create_Marker_Object(Player, Vector3(firstPos.X, firstPos.Y, secondPos.Z));
	Create_Marker_Object(Player, Vector3(firstPos.X, secondPos.Y, secondPos.Z));
	Create_Marker_Object(Player, Vector3(secondPos.X, firstPos.Y, firstPos.Z));
	Create_Marker_Object(Player, Vector3(secondPos.X, firstPos.Y, secondPos.Z));
	Create_Marker_Object(Player, Vector3(firstPos.X, secondPos.Y, firstPos.Z));
	Create_Marker_Object(Player, Vector3(secondPos.X, secondPos.Y, firstPos.Z));
	Create_Marker_Object(Player, secondPos);

	// Calculate the distance and the required amount of objects.
	float xLen = WWMath::Fabs(firstPos.X - secondPos.X);
	float yLen = WWMath::Fabs(firstPos.Y - secondPos.Y);
	float zLen = WWMath::Fabs(firstPos.Z - secondPos.Z);
	int xObjs = (int)WWMath::Ceil(xLen / 5);
	int yObjs = (int)WWMath::Ceil(yLen / 5);
	int zObjs = (int)WWMath::Ceil(zLen / 5);

	float curX = firstPos.X;
	float curY = firstPos.Y;
	float curZ = firstPos.Z;

	// Create objects for all X axises.
	for (int i = 0; i < 4; ++i) {
		switch (i) {
			case 0: {
				curY = firstPos.Y;
				curZ = firstPos.Z;
				break;
			}
			case 1: {
				curY = secondPos.Y;
				curZ = firstPos.Z;
				break;
			}
			case 2: {
				curY = firstPos.Y;
				curZ = secondPos.Z;
				break;
			}
			case 3: {
				curY = secondPos.Y;
				curZ = secondPos.Z;
				break;
			}
		}
		for (int j = 1; j < xObjs; ++j) {
			Create_Marker_Object(Player, Vector3(WWMath::Lerp(firstPos.X, secondPos.X, (j / (float)xObjs)), curY, curZ));
		}
	}

	// Create objects for all Y axises.
	for (int i = 0; i < 4; ++i) {
		switch (i) {
			case 0: {
				curX = firstPos.X;
				curZ = firstPos.Z;
				break;
			}
			case 1: {
				curX = secondPos.X;
				curZ = firstPos.Z;
				break;
			}
			case 2: {
				curX = firstPos.X;
				curZ = secondPos.Z;
				break;
			}
			case 3: {
				curX = secondPos.X;
				curZ = secondPos.Z;
				break;
			}
		}
		for (int j = 1; j < yObjs; ++j) {
			Create_Marker_Object(Player, Vector3(curX, WWMath::Lerp(firstPos.Y, secondPos.Y, (j / (float)yObjs)), curZ));
		}
	}

	// Create objects for all Z axises.
	for (int i = 0; i < 4; ++i) {
		switch (i) {
			case 0: {
				curX = firstPos.X;
				curY = firstPos.Y;
				break;
			}
			case 1: {
				curX = firstPos.X;
				curY = secondPos.Y;
				break;
			}
			case 2: {
				curX = secondPos.X;
				curY = firstPos.Y;
				break;
			}
			case 3: {
				curX = secondPos.X;
				curY = secondPos.Y;
				break;
			}
		}
		for (int j = 1; j < zObjs; ++j) {
			Create_Marker_Object(Player, Vector3(curX, curY, WWMath::Lerp(firstPos.Z, secondPos.Z, (j / (float)zObjs))));
		}
	}

	Data->Set_Marker_Enabled(true);
	DA::Page_Player(Player, "The selected area is now being highlighted.");
	return false;
}

bool AreaSaverPluginClass::ClearHighlight_ChatCommand(cPlayer* Player, const DATokenClass& Text, TextMessageEnum ChatType) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		DA::Page_Player(Player, "Player data could not be found.");
		return false;
	}

	Clear_Player_Markers(Player);
	DA::Page_Player(Player, "The highlight has been removed.");
	return false;
}


void AreaSaverPluginClass::Collect_Objects(cPlayer* Player, DynamicVectorClass<GameObject*>& Objects) {
	if (AreaSaverPlayerDataClass* Data = Get_Player_Data(Player)) {
		LineSegClass diagonal(Data->Get_First_Position(), Data->Get_Second_Position());
		AABoxClass box;
		box.Init(diagonal);

		for (SLNode<BaseGameObj>* n = GameObjManager::GameObjList.Head(); n; n = n->Next()) {
			if (PhysicalGameObj* obj = n->Data()->As_PhysicalGameObj()) {
				if (Player->Get_GameObj() != obj && !obj->Find_Observer("AreaSaverFlareDummyObserverClass")) {
					if (box.Contains(Commands->Get_Position(obj))) {
						Objects.Add(obj);
					}
				}
			}
		}
	}
}

bool AreaSaverPluginClass::Schematic_Exists(const StringClass& Name) {
	StringClass fileLoc;
	fileLoc += AREASAVERSCHEMATIC_LOCATION;
	fileLoc += "\\";
	fileLoc += Name;
	fileLoc += AREASAVERSCHEMATIC_EXTENSION;

	RawFileClass file(fileLoc);
	return file.Is_Available(0);
}

LRES AreaSaverPluginClass::Load_Schematic(cPlayer* Player, const StringClass& Name, int* LoadedObjects) {
	if (!Schematic_Exists(Name)) {
		return LRES::LOAD_FILE_NOT_FOUND;
	}

	Vector3 origPos = Commands->Get_Position(Player->Get_GameObj());

	StringClass fileLoc;
	fileLoc += AREASAVERSCHEMATIC_LOCATION;
	fileLoc += "\\";
	fileLoc += Name;
	fileLoc += AREASAVERSCHEMATIC_EXTENSION;

	RawFileClass file(fileLoc);
	if (!file.Open(1)) {
		return LRES::LOAD_FILE_NOT_OPENED;
	}

	ChunkLoadClass cload(&file);
	uint32 fileVer;

	if (!cload.Open_Chunk() || cload.Cur_Chunk_ID() != CHUNK_AREASAVR ||
		cload.SimpleRead(fileVer) <= 0 || fileVer != AREASAVER_FORMATVERSION) {
		return LRES::LOAD_FORMAT_INCOMPATIBLE;
	}

	while (cload.Open_Chunk()) {
		switch (cload.Cur_Chunk_ID()) {
			case CHUNK_SAVEINFO: {
				cload.SimpleRead(*LoadedObjects);
				
				StringClass _n;
				cload.Read(_n);

				StringClass _m;
				cload.Read(_m);

				break;
			}
			case CHUNK_SAVEOBJS: {
				unsigned long presetId = 0, healthskin = 0, shieldskin = 0;
				unsigned int colgroup = 0;
				int team = -1;
				StringClass model, botTag;
				Vector3 position, xvec, yvec, zvec, velocity;
				float gravity = 1, health = 0, healthmax = 0, shield = 0, shieldmax = 0, scale = 1, skeletonw = 1, skeletonh = 1;
				bool visible = true, rendered = true, stealth = false, immovable = false, engine = false, innate = false, freeze = false, flymode = false, damageAnims = true;
				DynamicVectorClass<StringClass> scriptNameList, scriptParamList;
				DynamicVectorClass<AreaSaverWeaponStruct*> weaponList;

				while (cload.Open_Micro_Chunk()) {
					switch (cload.Cur_Micro_Chunk_ID()) {
						case MCHUNK_DEFINFO: {
							cload.SimpleRead(presetId);
							break;
						}
						case MCHUNK_PHYINFO: {
							cload.SimpleRead(xvec);
							cload.SimpleRead(yvec);
							cload.SimpleRead(zvec);
							cload.SimpleRead(position);
							cload.Read(model);
							cload.SimpleRead(colgroup);
							cload.SimpleRead(velocity);
							cload.SimpleRead(gravity);
							break;
						}
						case MCHUNK_OBJINFO: {
							cload.SimpleRead(health);
							cload.SimpleRead(shield);
							cload.SimpleRead(healthmax);
							cload.SimpleRead(shieldmax);
							cload.SimpleRead(healthskin);
							cload.SimpleRead(shieldskin);
							cload.SimpleRead(team);
							cload.SimpleRead(visible);
							cload.SimpleRead(rendered);
							cload.SimpleRead(stealth);
							cload.SimpleRead(immovable);
							cload.SimpleRead(engine);
							cload.SimpleRead(innate);
							cload.SimpleRead(freeze);
							cload.SimpleRead(flymode);
							cload.SimpleRead(scale);
							cload.SimpleRead(damageAnims);
							cload.SimpleRead(skeletonw);
							cload.SimpleRead(skeletonh);
							cload.Read(botTag);
							break;
						}
						case MCHUNK_SCRIPTS: {
							StringClass name, params;
							cload.Read(name);
							cload.Read(params);
							scriptNameList.Add(name);
							scriptParamList.Add(params);
							break;
						}
						case MCHUNK_WEAPONS: {
							AreaSaverWeaponStruct* weap = new AreaSaverWeaponStruct;
							cload.SimpleRead(*weap);
							weaponList.Add(weap);
							break;
						}
					}
					cload.Close_Micro_Chunk();
				}

				DefinitionClass* def = Find_Definition(presetId);
				if (!def) {
					(*LoadedObjects)--;
					break;
				}

				PhysicalGameObj* obj = (PhysicalGameObj*)def->Create();
				if (!obj->As_PhysicalGameObj()) {
					obj->Set_Delete_Pending();
					(*LoadedObjects)--;
					break;
				}
				
				Matrix3D transform(xvec, yvec, zvec, origPos + position);
				obj->Set_Transform(transform);
				Commands->Set_Model(obj, model);
				obj->Set_Collision_Group(colgroup);
				Set_Object_Type(obj, team);

				Commands->Set_Is_Visible(obj, visible);
				Commands->Set_Is_Rendered(obj, !rendered);
				if (obj->As_SmartGameObj()) {
					Commands->Enable_Stealth(obj, stealth);
				}
				if (obj->Peek_Physical_Object()->As_MoveablePhysClass()) {
					obj->Peek_Physical_Object()->Set_Immovable(immovable);
					Set_Velocity(obj, velocity);
					Set_Gravity_Multiplier(obj, gravity);
				}

				DefenseObjectClass* defense = obj->Get_Defense_Object();
				defense->Set_Health_Max(healthmax);
				defense->Set_Shield_Strength_Max(shieldmax);
				defense->Set_Health(health);
				defense->Set_Shield_Strength(shield);
				defense->Set_Skin(healthskin);
				defense->Set_Shield_Type(shieldskin);

				if (VehicleGameObj* vehicle = obj->As_VehicleGameObj()) {
					Commands->Enable_Engine(vehicle, engine);
				}

				if (SoldierGameObj* soldier = obj->As_SoldierGameObj()) {
					if (innate) {
						Commands->Innate_Enable(soldier);
					}
					else {
						Commands->Innate_Disable(soldier);
					}

					if ((flymode && !soldier->Get_Fly_Mode()) || (!flymode && soldier->Get_Fly_Mode())) {
						soldier->Toggle_Fly_Mode();
					}

					soldier->Set_Scale_Across_Network(scale);
					soldier->Set_Freeze(freeze);
					soldier->Set_Can_Play_Damage_Animations(damageAnims);
					soldier->Set_Skeleton_Width(skeletonw);
					soldier->Set_Skeleton_Height(skeletonh);
					soldier->Set_Bot_Tag(botTag);
				}

				if (obj->As_ArmedGameObj()) {
					Commands->Clear_Weapons(obj);
					WeaponBagClass* bag = obj->As_ArmedGameObj()->Get_Weapon_Bag();

					while (weaponList.Count() > 0) {
						AreaSaverWeaponStruct* weapStruct = weaponList[0];
						if (WeaponClass* weap = bag->Add_Weapon(weapStruct->weaponDefId)) {
							// Set_Bullets changes clip, Set_Clip_Bullets changes inventory. They're reverse.
							Set_Bullets(obj, weap->Get_Name(), weapStruct->clipRounds);
							Set_Clip_Bullets(obj, weap->Get_Name(), weapStruct->invRounds);
							if (weapStruct->selected) {
								bag->Select_Weapon(weap);
							}
						}
						weaponList.Delete(0);
						delete weapStruct;
					}
				}

				
				while (obj->Get_Observers().Count() > 0) {
					obj->Remove_Observer(obj->Get_Observers()[0]);
				}

				for (int i = 0; i < scriptNameList.Count(); ++i) {
					Commands->Attach_Script(obj, scriptNameList[i], scriptParamList[i]);
				}
				obj->Start_Observers();

				break;
			}
		}
		cload.Close_Chunk();
	}
	cload.Close_Chunk();

	return LRES::LOAD_SUCCESSFUL;
}

SRES AreaSaverPluginClass::Save_Schematic(cPlayer* Player, const StringClass& Name, int* SavedObjects) {
	Vector3 firstPos;
	Vector3 secondPos;

	if (AreaSaverPlayerDataClass* Data = Get_Player_Data(Player)) {
		firstPos = Data->Get_First_Position();
		secondPos = Data->Get_Second_Position();
		if (firstPos.Length2() == 0.f || secondPos.Length2() == 0.f) {
			return SRES::SAVE_NO_REGION;
		}
	}

	DynamicVectorClass<GameObject*> objs;
	Collect_Objects(Player, objs);
	(*SavedObjects) = objs.Count();

	Vector3 origPos = Commands->Get_Position(Player->Get_GameObj());

	StringClass fileLoc;
	fileLoc += AREASAVERSCHEMATIC_LOCATION;
	fileLoc += "\\";
	fileLoc += Name;
	fileLoc += AREASAVERSCHEMATIC_EXTENSION;

	RawFileClass file(fileLoc);
	if (!file.Open(2)) {
		return SRES::SAVE_FILE_NOT_CREATED;
	}
	ChunkSaveClass csave(&file);
	csave.Begin_Chunk(CHUNK_AREASAVR);
	csave.SimpleWrite(AREASAVER_FORMATVERSION);

	csave.Begin_Chunk(CHUNK_SAVEINFO);
	csave.SimpleWrite(objs.Count());
	csave.Write(StringClass(Player->Get_Name()));
	csave.Write(The_Game()->Get_Map_Name());
	csave.End_Chunk();

	for (int i = 0; i < objs.Count(); ++i) {
		PhysicalGameObj* obj = objs[i]->As_PhysicalGameObj();

		csave.Begin_Chunk(CHUNK_SAVEOBJS);

		csave.Begin_Micro_Chunk(MCHUNK_DEFINFO);
		csave.SimpleWrite(obj->Get_Definition().Get_ID());
		csave.End_Micro_Chunk();

		csave.Begin_Micro_Chunk(MCHUNK_PHYINFO);
		Matrix3D transform = obj->Get_Transform();
		csave.SimpleWrite(transform.Get_X_Vector());
		csave.SimpleWrite(transform.Get_Y_Vector());
		csave.SimpleWrite(transform.Get_Z_Vector());
		csave.SimpleWrite(transform.Get_Translation() - origPos);
		csave.Write(Get_Model(obj));
		csave.SimpleWrite((int)obj->Get_Collision_Group());
		if (obj->Peek_Physical_Object()->As_MoveablePhysClass()) {
			csave.SimpleWrite(Get_Velocity(obj));
			csave.SimpleWrite(obj->Peek_Physical_Object()->As_MoveablePhysClass()->Get_Gravity_Multiplier());
		}
		else {
			csave.SimpleWrite(Vector3(0, 0, 0));
			csave.SimpleWrite(1.f);
		}
		csave.End_Micro_Chunk();

		csave.Begin_Micro_Chunk(MCHUNK_OBJINFO);
		DefenseObjectClass* defense = obj->Get_Defense_Object();
		csave.SimpleWrite(defense->Get_Health());
		csave.SimpleWrite(defense->Get_Shield_Strength());
		csave.SimpleWrite(defense->Get_Health_Max());
		csave.SimpleWrite(defense->Get_Shield_Strength_Max());
		csave.SimpleWrite(defense->Get_Skin());
		csave.SimpleWrite(defense->Get_Shield_Type());
		csave.SimpleWrite(obj->Get_Player_Type());
		if (obj->As_SoldierGameObj()) {
			csave.SimpleWrite(obj->As_SoldierGameObj()->Is_Visible());
		}
		else if (obj->As_VehicleGameObj()) {
			csave.SimpleWrite(obj->As_VehicleGameObj()->Get_Is_Scripts_Visible());
		}
		else {
			csave.SimpleWrite(true);
		}

		if (obj->Peek_Model()) {
			csave.SimpleWrite(obj->Peek_Model()->Is_Hidden() == 1);
		}
		else {
			csave.SimpleWrite(false);
		}

		if (obj->As_SmartGameObj()) {
			csave.SimpleWrite(obj->As_SmartGameObj()->Is_Stealth_Enabled());
		}
		else {
			csave.SimpleWrite(false);
		}

		if (obj->Peek_Physical_Object()->As_MoveablePhysClass()) {
			csave.SimpleWrite(obj->Peek_Physical_Object()->As_MoveablePhysClass()->Is_Immovable());
		}
		else {
			csave.SimpleWrite(false);
		}

		if (obj->As_VehicleGameObj()) {
			csave.SimpleWrite(Is_Engine_Enabled(obj));
		}
		else {
			csave.SimpleWrite(false);
		}

		if (obj->As_SoldierGameObj()) {
			csave.SimpleWrite(obj->As_SoldierGameObj()->Is_Innate_Enabled());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Is_Frozen());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Get_Fly_Mode());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Get_Scale_Across_Network());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Can_Play_Damage_Animations());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Get_Skeleton_Width());
			csave.SimpleWrite(obj->As_SoldierGameObj()->Get_Skeleton_Heigth());
			csave.Write(StringClass(obj->As_SoldierGameObj()->Get_Bot_Tag()));
		}
		else {
			csave.SimpleWrite(false);
			csave.SimpleWrite(false);
			csave.SimpleWrite(false);
			csave.SimpleWrite(1.f);
			csave.SimpleWrite(true);
			csave.SimpleWrite(1.f);
			csave.SimpleWrite(1.f);
			csave.Write("");
		}
		csave.End_Micro_Chunk();

		SimpleDynVecClass<GameObjObserverClass*> list = obj->Get_Observers();
		for (int i = 0; i < list.Count(); ++i) {
			try {
				if (ScriptImpClass* script = dynamic_cast<ScriptImpClass*>((list[i]))) {
					StringClass paramBuf;
					for (int i = 0; i < script->Get_Parameter_Count(); ++i) {
						paramBuf += script->Get_Parameter(i);
						paramBuf += ",";
					}
					paramBuf.TruncateRight(1);

					csave.Begin_Micro_Chunk(MCHUNK_SCRIPTS);
					csave.Write(script->Get_Name());
					csave.Write(paramBuf);
					csave.End_Micro_Chunk();
				}
			}
			catch (...) {

			}
		}

		if (obj->As_ArmedGameObj()) {
			WeaponBagClass* bag = obj->As_ArmedGameObj()->Get_Weapon_Bag();
			for (int i = 1; i < bag->Get_Count(); ++i) {
				AreaSaverWeaponStruct weap;
				weap.weaponDefId = bag->Peek_Weapon(i)->Get_ID();
				weap.clipRounds = bag->Peek_Weapon(i)->Get_Clip_Rounds();
				weap.invRounds = bag->Peek_Weapon(i)->Get_Inventory_Rounds();
				weap.selected = bag->Get_Index() == i;
				
				csave.Begin_Micro_Chunk(MCHUNK_WEAPONS);
				csave.SimpleWrite(weap);
				csave.End_Micro_Chunk();
			}
		}

		csave.End_Chunk();
	}

	csave.End_Chunk();

	return SRES::SAVE_SUCCESSFUL;
}

void AreaSaverPluginClass::Create_Marker_Object(cPlayer* Player, Vector3 Position) {
	if (PhysicalGameObj* Flare = Create_Object("Daves Arrow", Position)) {
		Flare->Set_Object_Dirty_Bit(NetworkObjectClass::BIT_CREATION, false);
		Flare->Set_Object_Dirty_Bit(Player->Get_Id(), NetworkObjectClass::BIT_CREATION, true);
		Commands->Set_Model(Flare, "ag_sigflare_gld");

		AreaSaverFlareDummyObserverClass* Observer = new AreaSaverFlareDummyObserverClass;
		Observer->Set_Flare_Owner(Player);
		Flare->Add_Observer(Observer);
	}
}

void AreaSaverPluginClass::Clear_Player_Markers(cPlayer* Player) {
	AreaSaverPlayerDataClass* Data = Get_Player_Data(Player);
	if (!Data) {
		return;
	}
	
	for (SLNode<BaseGameObj>* n = GameObjManager::GameObjList.Head(); n; n = n->Next()) {
		if (n->Data() && n->Data()->As_PhysicalGameObj() && 
			n->Data()->As_PhysicalGameObj()->Find_Observer("AreaSaverFlareDummyObserverClass") &&
			((AreaSaverFlareDummyObserverClass*)n->Data()->As_PhysicalGameObj()->Find_Observer("AreaSaverFlareDummyObserverClass"))->Get_Flare_Owner() == Player) {
			n->Data()->Set_Delete_Pending();
		}
	}

	Data->Set_Marker_Enabled(false);
}

void AreaSaverPlayerDataClass::Init() {
	FirstPos = Vector3(0, 0, 0);
	SecondPos = Vector3(0, 0, 0);
}

void AreaSaverPlayerDataClass::Clear_Level() {
	FirstPos = Vector3(0, 0, 0);
	SecondPos = Vector3(0, 0, 0);
}


AreaSaverPluginClass* Plugin = 0;

extern "C" {
	_declspec(dllexport) void Plugin_Init() {
		Plugin = new AreaSaverPluginClass;
	}
	_declspec(dllexport) void Plugin_Shutdown() {
		delete Plugin;
	}
}
