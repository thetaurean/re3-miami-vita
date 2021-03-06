#pragma once

enum eSound : uint16
{
	SOUND_CAR_DOOR_CLOSE_BONNET = 0,
	SOUND_CAR_DOOR_CLOSE_BUMPER,
	SOUND_CAR_DOOR_CLOSE_FRONT_LEFT,
	SOUND_CAR_DOOR_CLOSE_FRONT_RIGHT,
	SOUND_CAR_DOOR_CLOSE_BACK_LEFT,
	SOUND_CAR_DOOR_CLOSE_BACK_RIGHT,
	SOUND_CAR_DOOR_OPEN_BONNET,
	SOUND_CAR_DOOR_OPEN_BUMPER,
	SOUND_CAR_DOOR_OPEN_FRONT_LEFT,
	SOUND_CAR_DOOR_OPEN_FRONT_RIGHT,
	SOUND_CAR_DOOR_OPEN_BACK_LEFT,
	SOUND_CAR_DOOR_OPEN_BACK_RIGHT,
	SOUND_CAR_WINDSHIELD_CRACK,
	SOUND_CAR_JUMP,
	SOUND_CAR_JUMP_2,
	SOUND_CAR_TYRE_POP,
	SOUND_16,
	SOUND_17,
	SOUND_CAR_ENGINE_START,
	SOUND_CAR_LIGHT_BREAK,
	SOUND_CAR_HYDRAULIC_1,
	SOUND_CAR_HYDRAULIC_2,
	SOUND_CAR_HYDRAULIC_3,
	SOUND_CAR_JERK,
	SOUND_CAR_SPLASH,
	SOUND_BOAT_SLOWDOWN,
	SOUND_TRAIN_DOOR_CLOSE,
	SOUND_TRAIN_DOOR_OPEN,
	SOUND_CAR_TANK_TURRET_ROTATE,
	SOUND_CAR_BOMB_TICK,
	SOUND_PLANE_ON_GROUND,
	SOUND_31,
	SOUND_32,
	SOUND_STEP_START,
	SOUND_STEP_END,
	SOUND_FALL_LAND,
	SOUND_FALL_COLLAPSE,
	SOUND_FIGHT_37,
	SOUND_FIGHT_38,
	SOUND_FIGHT_39,
	SOUND_FIGHT_40,
	SOUND_FIGHT_41,
	SOUND_FIGHT_42,
	SOUND_FIGHT_43,
	SOUND_FIGHT_44,
	SOUND_FIGHT_45,
	SOUND_FIGHT_46,
	SOUND_FIGHT_47,
	SOUND_FIGHT_48,
	SOUND_49,
	SOUND_WEAPON_BAT_ATTACK,
	SOUND_WEAPON_KNIFE_ATTACK,
	SOUND_WEAPON_CHAINSAW_ATTACK,
	SOUND_WEAPON_CHAINSAW_IDLE,
	SOUND_WEAPON_CHAINSAW_MADECONTACT,
	SOUND_WEAPON_SHOT_FIRED,
	SOUND_WEAPON_RELOAD,
	SOUND_WEAPON_AK47_BULLET_ECHO,
	SOUND_WEAPON_FLAMETHROWER_FIRE,
	SOUND_WEAPON_SNIPER_SHOT_NO_ZOOM,
	SOUND_WEAPON_ROCKET_SHOT_NO_ZOOM,
	SOUND_WEAPON_HIT_PED,
	SOUND_WEAPON_HIT_VEHICLE,
	SOUND_GARAGE_NO_MONEY,
	SOUND_GARAGE_BAD_VEHICLE,
	SOUND_GARAGE_OPENING,
	SOUND_GARAGE_BOMB_ALREADY_SET,
	SOUND_GARAGE_BOMB1_SET,
	SOUND_GARAGE_BOMB2_SET,
	SOUND_GARAGE_BOMB3_SET,
	SOUND_70,
	SOUND_71,
	SOUND_GARAGE_VEHICLE_DECLINED,
	SOUND_GARAGE_VEHICLE_ACCEPTED,
	SOUND_GARAGE_DOOR_CLOSED,
	SOUND_GARAGE_DOOR_OPENED,
	SOUND_CRANE_PICKUP,
	SOUND_PICKUP_WEAPON_BOUGHT,
	SOUND_PICKUP_WEAPON,
	SOUND_PICKUP_HEALTH,
	SOUND_80,
	SOUND_81,
	SOUND_PICKUP_ADRENALINE,
	SOUND_PICKUP_ARMOUR,
	SOUND_PICKUP_BONUS,
	SOUND_PICKUP_MONEY,
	SOUND_PICKUP_HIDDEN_PACKAGE,
	SOUND_PICKUP_PACMAN_PILL,
	SOUND_PICKUP_PACMAN_PACKAGE,
	SOUND_PICKUP_FLOAT_PACKAGE,
	SOUND_BOMB_TIMED_ACTIVATED,
	SOUND_91,
	SOUND_BOMB_ONIGNITION_ACTIVATED,
	SOUND_BOMB_TICK,
	SOUND_RAMPAGE_START,
	SOUND_RAMPAGE_ONGOING,
	SOUND_RAMPAGE_PASSED,
	SOUND_RAMPAGE_FAILED,
	SOUND_RAMPAGE_KILL,
	SOUND_RAMPAGE_CAR_BLOWN,
	SOUND_EVIDENCE_PICKUP,
	SOUND_UNLOAD_GOLD,
	SOUND_PAGER,
	SOUND_PED_DEATH,
	SOUND_PED_DAMAGE,
	SOUND_PED_HIT,
	SOUND_PED_LAND,
	SOUND_PED_BULLET_HIT,
	SOUND_PED_BURNING,
	SOUND_PED_PLAYER_REACTTOCOP,
	SOUND_PED_ARREST_COP,
	SOUND_PED_MIAMIVICE_EXITING_CAR,
	SOUND_PED_COP_HELIPILOTPHRASE,
	SOUND_PED_PULLOUTWEAPON,
	SOUND_PED_HELI_PLAYER_FOUND,
	SOUND_115,
	SOUND_PED_ON_FIRE,
	SOUND_PED_AIMING,
	SOUND_PED_HANDS_UP,
	SOUND_PED_HANDS_COWER,
	SOUND_PED_FLEE_SPRINT,
	SOUND_PED_CAR_JACKING,
	SOUND_PED_MUGGING,
	SOUND_PED_CAR_JACKED,
	SOUND_PED_ROBBED,
	SOUND_PED_ACCIDENTREACTION1,
	SOUND_PED_UNK_126,
	SOUND_PED_PLAYER_AFTERSEX,
	SOUND_PED_PLAYER_BEFORESEX,
	SOUND_PED_COP_UNK_129, // also used for medics
	SOUND_PED_COP_MANYCOPSAROUND, // also used for medics
	SOUND_PED_GUNAIMEDAT2,
	SOUND_PED_COP_ALONE, // also used for medics
	SOUND_PED_GUNAIMEDAT3,
	SOUND_PED_COP_REACTION,
	SOUND_PED_COP_LITTLECOPSAROUND, // also used for medics
	SOUND_PED_PLAYER_FARFROMCOPS, // also used for medics
	SOUND_PED_TAXI_WAIT,
	SOUND_PED_ATTACK,
	SOUND_PED_DEFEND,
	SOUND_140,
	SOUND_PED_LEAVE_VEHICLE,
	SOUND_PED_EVADE,
	SOUND_PED_FLEE_RUN,
	SOUND_PED_CRASH_VEHICLE,
	SOUND_PED_CRASH_CAR,
	SOUND_PED_ANNOYED_DRIVER,
	SOUND_PED_147,
	SOUND_PED_SOLICIT,
	SOUND_PED_149,
	SOUND_PED_150,
	SOUND_PED_EXTINGUISHING_FIRE,
	SOUND_PED_WAIT_DOUBLEBACK,
	SOUND_153,
	SOUND_PED_CHAT_SEXY,
	SOUND_PED_CHAT_EVENT,
	SOUND_PED_PED_COLLISION,
	SOUND_PED_CHAT,
	SOUND_PED_TAXI_CALL,
	SOUND_RACE_START_3,
	SOUND_RACE_START_2,
	SOUND_RACE_START_1,
	SOUND_RACE_START_GO,
	SOUND_SPLASH,
	SOUND_WATER_FALL,
	SOUND_SPLATTER,
	SOUND_CAR_PED_COLLISION,
	SOUND_CLOCK_TICK,
	SOUND_PART_MISSION_COMPLETE,
	SOUND_FRONTEND_MENU_STARTING, // same with SOUND_HUD_SOUND

	// TODO(Miami): What are 170-175??
	
	SOUND_FRONTEND_NO_RADIO = 176, // those 3 are all same sound
	SOUND_FRONTEND_RADIO_CHANGE,
	SOUND_FRONTEND_RADIO_CHANGE_2,
	SOUND_HUD_SOUND,
	SOUND_180,
	SOUND_181,
	SOUND_182,
	SOUND_LIGHTNING,
	SOUND_BULLETTRACE_1,
	SOUND_BULLETTRACE_2,
	SOUND_186, // makes same sound with 40
	SOUND_187, // makes same sound with 46
	SOUND_MELEE_ATTACK_START,
	SOUND_SKATING,
	SOUND_WEAPON_MINIGUN_ATTACK,
	SOUND_WEAPON_MINIGUN_2,
	SOUND_WEAPON_MINIGUN_3,
	SOUND_AMMUNATION_IMRAN_ARM_BOMB,
	SOUND_194,

	// TODO(Miami): They're frontend sounds but names are copy-paste and incorrect
	SOUND_FRONTEND_MENU_NEW_PAGE,
	SOUND_FRONTEND_FAIL,
	SOUND_FRONTEND_MENU_SETTING_CHANGE,
	SOUND_FRONTEND_MENU_BACK,
	SOUND_FRONTEND_AUDIO_TEST,

	SOUND_INJURED_PED_MALE_OUCH,
	SOUND_INJURED_PED_FEMALE,
	SOUND_SET_202,
	SOUND_SET_203,
	SOUND_TOTAL_SOUNDS = 204,
	SOUND_NO_SOUND = 205,
};


enum eScriptSounds : uint16 {
	SCRIPT_SOUND_0 = 0,
	SCRIPT_SOUND_1,
	SCRIPT_SOUND_2,
	SCRIPT_SOUND_3,
	SCRIPT_SOUND_PARTY_1_LOOP_S,
	SCRIPT_SOUND_PARTY_1_LOOP_L,
	SCRIPT_SOUND_PARTY_2_LOOP_S,
	SCRIPT_SOUND_PARTY_2_LOOP_L,
	SCRIPT_SOUND_PARTY_3_LOOP_S,
	SCRIPT_SOUND_PARTY_3_LOOP_L,
	SCRIPT_SOUND_PARTY_4_LOOP_S,
	SCRIPT_SOUND_PARTY_4_LOOP_L,
	SCRIPT_SOUND_PARTY_5_LOOP_S,
	SCRIPT_SOUND_PARTY_5_LOOP_L,
	SCRIPT_SOUND_PARTY_6_LOOP_S,
	SCRIPT_SOUND_PARTY_6_LOOP_L,
	SCRIPT_SOUND_PARTY_7_LOOP_S,
	SCRIPT_SOUND_PARTY_7_LOOP_L,
	SCRIPT_SOUND_PARTY_8_LOOP_S,
	SCRIPT_SOUND_PARTY_8_LOOP_L,
	SCRIPT_SOUND_PARTY_9_LOOP_S,
	SCRIPT_SOUND_PARTY_9_LOOP_L,
	SCRIPT_SOUND_PARTY_10_LOOP_S,
	SCRIPT_SOUND_PARTY_10_LOOP_L,
	SCRIPT_SOUND_PARTY_11_LOOP_S,
	SCRIPT_SOUND_PARTY_11_LOOP_L,
	SCRIPT_SOUND_PARTY_12_LOOP_S,
	SCRIPT_SOUND_PARTY_12_LOOP_L,
	SCRIPT_SOUND_PARTY_13_LOOP_S,
	SCRIPT_SOUND_PARTY_13_LOOP_L,
	SCRIPT_SOUND_STRIP_CLUB_LOOP_1_S,
	SCRIPT_SOUND_STRIP_CLUB_LOOP_1_L,
	SCRIPT_SOUND_STRIP_CLUB_LOOP_2_S,
	SCRIPT_SOUND_STRIP_CLUB_LOOP_2_L,
	SCRIPT_SOUND_WORK_SHOP_LOOP_S,
	SCRIPT_SOUND_WORK_SHOP_LOOP_L,
	SCRIPT_SOUND_SAWMILL_LOOP_S,
	SCRIPT_SOUND_SAWMILL_LOOP_L,
	SCRIPT_SOUND_38,
	SCRIPT_SOUND_39,

	// MIAMI: only these are true so far
	SCRIPT_SOUND_MALIBU_1,
	SCRIPT_SOUND_MALIBU_2,
	SCRIPT_SOUND_MALIBU_3,
	SCRIPT_SOUND_STRIPCLUB_1,
	SCRIPT_SOUND_STRIPCLUB_2,
	SCRIPT_SOUND_STRIPCLUB_3,

	SCRIPT_SOUND_46_S,
	SCRIPT_SOUND_47_L,
	SCRIPT_SOUND_MARCO_BISTRO_S,
	SCRIPT_SOUND_MARCO_BISTRO_L,
	SCRIPT_SOUND_AIRPORT_LOOP_S,
	SCRIPT_SOUND_AIRPORT_LOOP_L,
	SCRIPT_SOUND_SHOP_LOOP_S,
	SCRIPT_SOUND_SHOP_LOOP_L,
	SCRIPT_SOUND_CINEMA_LOOP_S,
	SCRIPT_SOUND_CINEMA_LOOP_L,
	SCRIPT_SOUND_DOCKS_LOOP_S,
	SCRIPT_SOUND_DOCKS_LOOP_L,
	SCRIPT_SOUND_HOME_LOOP_S,
	SCRIPT_SOUND_HOME_LOOP_L,
	SCRIPT_SOUND_FRANKIE_PIANO,
	SCRIPT_SOUND_PARTY_1_LOOP,
	SCRIPT_SOUND_PORN_CINEMA_1_S,
	SCRIPT_SOUND_PORN_CINEMA_1_L,
	SCRIPT_SOUND_PORN_CINEMA_2_S,
	SCRIPT_SOUND_PORN_CINEMA_2_L,
	SCRIPT_SOUND_PORN_CINEMA_3_S,
	SCRIPT_SOUND_PORN_CINEMA_3_L,
	SCRIPT_SOUND_BANK_ALARM_LOOP_S,
	SCRIPT_SOUND_BANK_ALARM_LOOP_L,
	SCRIPT_SOUND_POLICE_BALL_LOOP_S,
	SCRIPT_SOUND_POLICE_BALL_LOOP_L,
	SCRIPT_SOUND_RAVE_LOOP_INDUSTRIAL_S,
	SCRIPT_SOUND_RAVE_LOOP_INDUSTRIAL_L,
	SCRIPT_SOUND_74,
	SCRIPT_SOUND_75,
	SCRIPT_SOUND_POLICE_CELL_BEATING_LOOP_S,
	SCRIPT_SOUND_POLICE_CELL_BEATING_LOOP_L,
	SCRIPT_SOUND_INJURED_PED_MALE_OUCH_S,
	SCRIPT_SOUND_INJURED_PED_MALE_OUCH_L,
	SCRIPT_SOUND_INJURED_PED_FEMALE_OUCH_S,
	SCRIPT_SOUND_INJURED_PED_FEMALE_OUCH_L,
	SCRIPT_SOUND_EVIDENCE_PICKUP,
	SCRIPT_SOUND_UNLOAD_GOLD,
	SCRIPT_SOUND_RAVE_1_LOOP_S,
	SCRIPT_SOUND_RAVE_1_LOOP_L,
	SCRIPT_SOUND_RAVE_2_LOOP_S,
	SCRIPT_SOUND_RAVE_2_LOOP_L,
	SCRIPT_SOUND_RAVE_3_LOOP_S,
	SCRIPT_SOUND_RAVE_3_LOOP_L,
	SCRIPT_SOUND_MISTY_SEX_S,
	SCRIPT_SOUND_MISTY_SEX_L,
	SCRIPT_SOUND_GATE_START_CLUNK,
	SCRIPT_SOUND_GATE_STOP_CLUNK,
	SCRIPT_SOUND_PART_MISSION_COMPLETE,
	SCRIPT_SOUND_CHUNKY_RUN_SHOUT,
	SCRIPT_SOUND_SECURITY_GUARD_AWAY_SHOUT,
	SCRIPT_SOUND_RACE_START_3,
	SCRIPT_SOUND_RACE_START_2,
	SCRIPT_SOUND_RACE_START_1,
	SCRIPT_SOUND_RACE_START_GO,
	SCRIPT_SOUND_SWAT_PED_SHOUT,
	SCRIPT_SOUND_PRETEND_FIRE_LOOP,
	SCRIPT_SOUND_AMMUNATION_CHAT_1,
	SCRIPT_SOUND_AMMUNATION_CHAT_2,
	SCRIPT_SOUND_AMMUNATION_CHAT_3,
	SCRIPT_SOUND_BULLET_HIT_GROUND_1,
	SCRIPT_SOUND_BULLET_HIT_GROUND_2,
	SCRIPT_SOUND_BULLET_HIT_GROUND_3,
	SCRIPT_SOUND_BULLET_HIT_WATER, // no sound
	SCRIPT_SOUND_TRAIN_ANNOUNCEMENT_1,
	SCRIPT_SOUND_TRAIN_ANNOUNCEMENT_2,
	SCRIPT_SOUND_PAYPHONE_RINGING,
	SCRIPT_SOUND_113,
	SCRIPT_SOUND_GLASS_BREAK_L,
	SCRIPT_SOUND_GLASS_BREAK_S,
	SCRIPT_SOUND_GLASS_CRACK,
	SCRIPT_SOUND_GLASS_LIGHT_BREAK,
	SCRIPT_SOUND_BOX_DESTROYED_1,
	SCRIPT_SOUND_BOX_DESTROYED_2,
	SCRIPT_SOUND_METAL_COLLISION,
	SCRIPT_SOUND_TIRE_COLLISION,
	SCRIPT_SOUND_GUNSHELL_DROP,
	SCRIPT_SOUND_GUNSHELL_DROP_SOFT,
	SCRIPT_SOUND_TOTAL,
	SCRIPT_SOUND_INVALID,
};
