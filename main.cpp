#include <windows.h>
#include <stdio.h>

#define EXT_IUTIL
#define EXT_IPLAYER
#define EXT_IOBJECT
#define EXT_IHALOENGINE
#define EXT_ITIMER
#define EXT_HKTIMER
#define EXT_ICOMMAND
#define EXT_ICINIFILE

#define _INC_TIME
#pragma comment(lib, "../Add-on API/Add-on API.lib")
#include "../Add-on API/Add-on API.h"

addon_info EXTPluginInfo = { L"Infection GameType Add-on", L"2.0.0.0",
                                    L"RadWolfie & Wizard",
                                    L"It provide ability simulate a zombie gametype with almost any proper gametype.",
                                    L"Infection Gametype",
                                    L"gametype",
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL };

//#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))

IUtil* pIUtil = 0;
IPlayer* pIPlayer = 0;
IObject* pIObject = 0;
IHaloEngine* pIHaloEngine = 0;
ITimer* pITimer = 0;
ICommand* pICommand = 0;
ICIniFile* pICIniFile = 0;
UINT EAOHashID = 0;

#pragma region //Wizard's Code
#include <string>
#include <vector>
#include <math.h>    //Dumb Microsoft for leaving out round in many versions...
#include <time.h>   //Is needed for srand function.
double roundC(double num) {
    return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
}
#define randomRange(min, max) (min + (rand() % (int)(max - min + 1)))
//#define randomRange(min, max) (int( rand() / (RAND_MAX+1)) * (max-min+1) + min)   //BAD

// Global Variable
//std::wstring default_script_prefix = L"";
e_color_team_index human_team = COLOR_TEAM_RED; // 0 is red, 1 is blue
UINT max_zombie_count = 1; // this caps what the zombie count would be w/ ratio (nil is disable)
UINT time_invis = 1; // In seconds, how long the zombie/human should be invis when they crouch.
int zombie_count = 1; // if value is less than 1 is it used as a percentage, more than or equal to one is absolute count
e_color_team_index zombie_team = COLOR_TEAM_BLUE; // 0 is red, 1 is blue
e_color_team_index join_team = zombie_team; // the team that people will join if a game is currently running
float flashlight_speed = 2.2f; //I wouldn't suggest increasing this, unless you want people to lag.

    // Alpha Zombie Variables
int alphazombie_frag_count = 0; // number of frag nades they spawn with
int alphazombie_plasma_count = 0; // number of plasma nades they spawn with
int alphazombie_clip_count = 0; // number of shots in clip (loaded ammo)
int alphazombie_ammo_count = 0; // backpack ammo they get (unloaded ammo)
float alphazombie_battery_count = 0.0f; // stored as a percent (0 to 1, do NOT go over or under)

    // Zombie Variables
int zombie_ammo_count = 0; // backpack ammo zombies once there are not only alpha zombies (unloaded ammo)
int zombie_clip_count = 0; // number of shots in clip for zombies once there are not only alpha zombies (loaded ammo)
float zombie_battery_count = 0; // stored as a percent (0 to 1, do NOT go over or under)
int zombie_frag_count = 0; // number of frag nades they spawn with
int zombie_plasma_count = 0; // number of plasma nades they spawn with
int zombie_spawn_time = 0; // spawn time for zombies in seconds. Leave "default" for default spawn time of gametype
float zombie_speed = 1.5f; // zombie speed
e_color_index zombie_color = COLOR_TAN;

    // Zombie weapons
// Note: If you decide the player holds a flag or a ball, make sure the secondary, tertiary, and quarternary fields are "".
// DO NOT make zombies hold multiple weapons if you want them to hold an oddball or a flag. If you do it will not work right, and it's entirely your fault.
const char* zombie_weapon[4] = {
    "weapons\\shotgun\\shotgun",    // Primary weapon for zombies.
    "",                             // Secondary weapon for zombies.
    "",                             // Tertiary weapon for zombies.
    ""};                            // Quarternary weapon for zombies.
s_ident zweapon[4] = { 0 };

    // Human weapons
const char* human_weapon[4] = {
    "",                             // Primary weapon for humans.
    "",                             // Secondary weapon for humans.
    "",                             // Tertiary weapon for humans.
    ""};                            // Quarternary weapon for humans.
s_ident hweapon[4] = { 0 };

    // Human Variables
float human_dmgmodifier = 1.0f; // damage modifier for humans.
float human_speed = 1.0f; // speed when not infected
int human_spawn_time = 2; // spawn time for humans in seconds. Leave "default" for default spawn time of gametype
e_color_index human_color = COLOR_RED;

    // Last Man Variables
float lastman_dmgmodifier = 1.35f; // damage modifier for the last man
int lastman_invistime = 20; // in seconds
int lastman_invulnerable = 10; // time (in seconds) the last man is invulnerable for: replace with nil to disable
float lastman_speed = 1.50f; // last man speed

    // Booleans
bool humans_allowed_in_vehis = false; // if this is set to false { humans cannot enter vehicles
bool zombies_allowed_in_vehis = false; // if this is set to false { zombies cannot enter vehicles.
bool humans_invisible_on_crouch = false; // if this is set to true { humans will become invisible when they crouch.
bool zombies_invisible_on_crouch = true; // if this is set to true { zombies will be invisible when they crouch.
bool infect_on_fall = true; // if this is set to true { people who die from fall damage will become zombies.
bool infect_on_guardians = false; // if this is set to true { people who get killed by the guardians will become zombies.
bool infect_on_suicide = true; // if this is set to true { people who kill themselves will become a zombie.
bool infect_on_betrayal = false; // if this is set to true { people who betray their teammates will become a zombie.
bool last_human_next_zombie = true; // if this value is true the last man standing becomes the next zombie, if not it's random
bool use_old_kill_messages = true; // Set this if you like the OLD zombie infection messages with the 059 zombies.

    // Additional variables.
bool team_play = false;
bool starting_equipment_is_generic;
UINT gametype = -1;
static const wchar_t teamRedStr[] = L"rt";
static const wchar_t teamBlueStr[] = L"bt";
UINT infected[16] = { 0 };
bool usedFlashlight[16] = { 0 };
UINT zombie_kills[16] = { 0 };

// Custom messages
ICIniFile* customMsg;
static const wchar_t customMsgStr[] = L"message.ini";

static const wchar_t str1_0[] = L"1.0";
static const wchar_t str1_1[] = L"1.1";
static const wchar_t str1_2[] = L"1.2";
static const wchar_t str1_3[] = L"1.3";
static const wchar_t str1_4[] = L"1.4";
static const wchar_t str1_5[] = L"1.5";
static const wchar_t str1_6[] = L"1.6";
static const wchar_t str1_7[] = L"1.7";
static const wchar_t str1_8[] = L"1.8";
static const wchar_t str1_9[] = L"1.9";
static const wchar_t str1_10[] = L"1.10";
static const wchar_t str1_11[] = L"1.11";
static const wchar_t str1_12[] = L"1.12";
static const wchar_t str1_13[] = L"1.13";
static const wchar_t str1_14[] = L"1.14";

static const wchar_t str2_0[] = L"2.0";
static const wchar_t str2_1[] = L"2.1";
static const wchar_t str2_2[] = L"2.2";
static const wchar_t str2_3[] = L"2.3";
static const wchar_t str2_4[] = L"2.4";
static const wchar_t str2_5[] = L"2.5";
static const wchar_t str2_6[] = L"2.6";
static const wchar_t str2_7[] = L"2.7";
static const wchar_t str2_8[] = L"2.8";
static const wchar_t str2_9[] = L"2.9";
static const wchar_t str2_10[] = L"2.10";
static const wchar_t str2_11[] = L"2.11";
static const wchar_t str2_12[] = L"2.12";
static const wchar_t str2_13[] = L"2.13";

static const wchar_t str3_0[] = L"3.0";
static const wchar_t str3_1[] = L"3.1";
static const wchar_t str3_2[] = L"3.2";
static const wchar_t str3_3[] = L"3.3";
static const wchar_t str3_4[] = L"3.4";
static const wchar_t str3_5[] = L"3.5";
static const wchar_t str3_6[] = L"3.6";
static const wchar_t str3_7[] = L"3.7";
static const wchar_t str3_8[] = L"3.8";
static const wchar_t str3_9[] = L"3.9";

static const wchar_t str4_0[] = L"4.0";
static const wchar_t str4_1[] = L"4.1";
static const wchar_t str4_2[] = L"4.2";
static const wchar_t str4_3[] = L"4.3";
static const wchar_t str4_4[] = L"4.4";
static const wchar_t str4_5[] = L"4.5";
static const wchar_t str4_6[] = L"4.6";
static const wchar_t str4_7[] = L"4.7";
static const wchar_t str4_8[] = L"4.8";
static const wchar_t str4_9[] = L"4.9";

static const wchar_t str5_0[] = L"5.0";
static const wchar_t str5_1[] = L"5.1";
static const wchar_t str5_2[] = L"5.2";
static const wchar_t str5_3[] = L"5.3";
static const wchar_t str5_4[] = L"5.4";
static const wchar_t str5_5[] = L"5.5";
static const wchar_t str5_6[] = L"5.6";
static const wchar_t str5_7[] = L"5.7";
static const wchar_t str5_8[] = L"5.8";
static const wchar_t str5_9[] = L"5.9";

static const wchar_t str6_0[] = L"6.0";
static const wchar_t str6_1[] = L"6.1";
static const wchar_t str6_2[] = L"6.2";
static const wchar_t str6_3[] = L"6.3";
static const wchar_t str6_4[] = L"6.4";
static const wchar_t str6_5[] = L"6.5";
static const wchar_t str6_6[] = L"6.6";
static const wchar_t str6_7[] = L"6.7";
static const wchar_t str6_8[] = L"6.8";
static const wchar_t str6_9[] = L"6.9";


static const wchar_t section_str_hint[] = L"hint";
static const wchar_t section_str_death[] = L"death";
static const wchar_t section_str_complement[] = L"complement";
static const wchar_t section_str_new_team[] = L"new team";
static const wchar_t section_str_misc[] = L"misc";
static const wchar_t section_str_gametype[] = L"gametype";
static const wchar_t section_str_command[] = L"command";
static const wchar_t section_str_general[] = L"general";

        // Hint Messages
wchar_t blockteamchange_message[INIFILEVALUEMAX] = L"Autobalance: You're not allowed to change team.",
        teamkill_message[INIFILEVALUEMAX] = L"Don't team kill...",
        nozombiesleftmessage[INIFILEVALUEMAX] = L"There are no zombies left. Someone needs to change team or someone will be forced to.",
        lastman_message[INIFILEVALUEMAX] = L"{0:s} is the last human alive and is invisible for {1:2d} seconds!",
        rejoin_message[INIFILEVALUEMAX] = L"Please don't leave and rejoin. You've been put back onto your last team.",
        zombieinvis_message[INIFILEVALUEMAX] = L"The zombies are invisible for 20 seconds!",
        speedburst_begin_message[INIFILEVALUEMAX] = L"**FLASHLIGHT SPEEDBURST USED**",
        speedburst_end_message[INIFILEVALUEMAX] = L"**FLASHLIGHT SPEED USED. REGAIN IT ON NEXT SPAWN**",
        block_tree_message[INIFILEVALUEMAX] = L"This tree is blocked.",
        block_spot_message[INIFILEVALUEMAX] = L"Sorry, this spot has been blocked...",
        block_glitch_message[INIFILEVALUEMAX] = L"Glitching is not allowed!",
        nozombiesleft_counter_message[INIFILEVALUEMAX] = L"In {0:2d} seconds a player will be forced to become a zombie.",

        // Death/Infection Messages
        falling_infected_message[INIFILEVALUEMAX] = L"{0:s} has been infected by falling",
        guardian_infected_message[INIFILEVALUEMAX] = L"{0:s} was infected by the guardians",
        kill_infected_message[INIFILEVALUEMAX] = L"{0:s} has infected {1:s}",
        teammate_infected_message[INIFILEVALUEMAX] = L"{0:s} was infected for betraying {1:s}",
        suicide_infected_message[INIFILEVALUEMAX] = L"{0:s} lost the will to live",
        human_kill_message[INIFILEVALUEMAX] = L"{0:s} has killed {1:s}",
        falling_death_message[INIFILEVALUEMAX] = L"{0:s} slipped and fell",
        guardian_death_message[INIFILEVALUEMAX] = L"",
        teammate_death_message[INIFILEVALUEMAX] = L"{0:s} betrayed {1:s}",
        suicide_death_message[INIFILEVALUEMAX] = L"{0:s} has killed themself",
        infected_message[INIFILEVALUEMAX] = L"{0:s} has been infected!",

        // Complement Messages
        timer_team_change_msg[INIFILEVALUEMAX] = L"Thank you. The game will now continue",
        zombie_backtap_message[INIFILEVALUEMAX] = L"Nice backtap!",

        // New Team Messages
        human_message[INIFILEVALUEMAX] = L"YOU'RE A HUMAN. Survive!",
        zombie_message[INIFILEVALUEMAX] = L"YOU'RE A ZOMBIE. FEED ON HUMANS!",

        // Additional Messages
        welcome_message[INIFILEVALUEMAX] = L"Welcome to Zombies",
        koth_additional_welcome_msg[INIFILEVALUEMAX] = L"The hill is a safezone! Use it for quick getaways!",
        slayer_additional_welcome1_msg[INIFILEVALUEMAX] = L"The nav points are not just for decoration!",
        slayer_additional_welcome2_msg[INIFILEVALUEMAX] = L"They will point to the last human surviving!",
        cure_zombie_kill_message[INIFILEVALUEMAX] = L"{0:s} is now a human because they infected 5 times!",
        game_start_message[INIFILEVALUEMAX] = L"The game has started",
        zombie_infect_human_message[INIFILEVALUEMAX] = L"You have transmitted the zombie virus to {0:s}",
        human_infect_begin_message[INIFILEVALUEMAX] = L"YOU ARE INFECTED! Find a healthpack in 45 seconds to survive!",
        human_infect_counter_message[INIFILEVALUEMAX] = L"YOU ARE INFECTED! Find a healthpack in {0:2d} seconds to survive!",
        human_infect_end_message[INIFILEVALUEMAX] = L"{0:s} could not find a cure in time!",
        cure_human_message[INIFILEVALUEMAX] = L"YOU HAVE BEEN CURED!",

        // Gametype Messages
        koth_infect_begin_message[INIFILEVALUEMAX] = L"{0:s} must leave the hill in 10 seconds or they will be infected!",
        koth_infect_end_message[INIFILEVALUEMAX] = L"{0:s} was infected because they were in the hill too long!",
        koth_kill_begin_message[INIFILEVALUEMAX] = L"{0:s} must leave the hill in 10 seconds or they will be killed!",
        koth_kill_end_message[INIFILEVALUEMAX] = L"{0:s} has been killed because they were in the hill too long!",
        koth_kill_counter_message[INIFILEVALUEMAX] = L"You have {0:2d} seconds to leave the hill!";


// Don't modify below variables unless you know what you're doing
int cur_zombie_count = 0;
int cur_human_count = 0;
UINT alpha_zombie_count = 0;
//int human_time = {};
//int cur_players = 0; //Don't use this, intead use pIHaloEngine->serverHeader->totalPlayers
PlayerInfo cur_last_human;
//int last_man_name = 0;
//BYTE game_init = 0;
//bool game_enable = 0;
bool allow_change = false;
bool map_reset_boolean = false;
bool dontModifyDmg = false;

//Modified to improve the Infection gametype flow.

struct dataTable {
    std::string hash;
    int human_time;
    int zombie_kills;
    char isZombie;
    int inhill_time;
    DWORD activateFlashlight;
    DWORD uniqueID;
    dataTable() {
        human_time = 0;
        zombie_kills = 0;
        isZombie = 0;
        inhill_time = -1;
        activateFlashlight = 0;
        uniqueID=-1;
    }
};
std::vector<dataTable> cacheTable;

bool game_started;
UINT human_time[16];
UINT last_hill_time[16];
wchar_t last_human_name[12] = { 0 };
wchar_t name_table[16][12];
UINT inhill_time[16];
UINT gametype_indicator;

int playerChangeCounter=0;
PlayerInfo pl_null;
s_ident oddball_flag_obj[16];
real_vector3d location;
real_vector3d velocity_reset;
PlayerInfo plI_ctf;
s_weapon* oddball_flag_relocate=NULL;

//Missing values
int resptime = 0;
hTagHeader *falling_tag_id,
        *distance_tag_id,
        *camouflage_tag_id,
        *healthpack_tag_id,
        *overshield_tag_id,
        *fragnade_tag_id,
        *plasmanade_tag_id,
//        fragnade_tag_id,    //Found duplicated
//        plasmanade_tag_id,    //Found duplicated
        *oddball_tag_id,
        *flag_tag_id,
        *rifle_id,
        *needler_id,
        *pistol_id,
        *rocket_id,
        *shotgun_id,
        *sniper_id,
        *flame_id,
        *rifle_ammo_id,
        *needler_ammo_id,
        *pistol_ammo_id,
        *rocket_ammo_id,
        *shotgun_ammo_id,
        *sniper_ammo_id,
        *flame_ammo_id,
        *fuel_dmg1_id,
        *fuel_dmg2_id,
        *fuel_dmg3_id,
        *fuel_dmg4_id;
#pragma endregion

#pragma region //RadWolfie's code port to H-Ext plugin.


#pragma region Timers section
typedef enum eTimer {
    eNONE = -1,
    eAssignLeftOverZombieWeapon = 0,
    eAssignLeftOverHumanWeapon,
    eAssignZombieWeapon,
    eAssignHumanWeapon,
    eHaveSpeedTimer,
    eBumpTimer,
    eBumpInfection,
    eCheckGameState,
    eHumanTimer,
    eInvisCrouch,
    eMsgTimer,
    eNewGameTimer,
    ePlayerChangeTimer,
    eRemoveLastManProtection
} eTimer;
typedef struct dTimer {
    UINT TimerID;
    eTimer eFunc;
    int machine_index; //(optional field)
} dTimer;
std::vector<dTimer> vTimer(0x40);

//Additional field checker
bool hasPlayerChangeTimer = false;

s_ident lastman_object;
#pragma endregion

#pragma region Functions section
//RadWolfie - Approved
int getalphacount() {
    // recalculate how many "alpha" zombies there are
    //int alpha_zombie_count;
    if (zombie_count < 1)
        alpha_zombie_count = (int)roundC((pIHaloEngine->serverHeader->totalPlayers * zombie_count) + 0.5);
    else
        alpha_zombie_count = zombie_count;

    if (alpha_zombie_count > max_zombie_count)
        alpha_zombie_count = max_zombie_count;

    return alpha_zombie_count;
}

//RadWolfie - Approved | except patch is excluded and does not work with multiple halo builds due to different code.
void InitializeGame() {
    //RadWolfie - This is just in case someone might initialize this while not hosting the map.
    if (pIHaloEngine->mapCurrent->type == 1) {
        cur_zombie_count = 0;
        cur_human_count = 0;
        alpha_zombie_count = 0;

        for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
            pITimer->m_delete(EAOHashID, iTimer->TimerID);
        }
        vTimer.clear();
        hasPlayerChangeTimer = false;

        UINT i;
        for (i = 0; i < 16; i++) {
            human_time[i] = 0;
        }
        cur_last_human = PlayerInfo();
        //last_human_name[0] = 0;
        game_started = false;
        allow_change = false;
        for (i = 0; i < 16; i++) {
            last_hill_time[i] = -1;
        }
        for (i = 0; i < 16; i++) {
            name_table[i][0] = 0;
        }
        for (i = 0; i < 16; i++) {
            inhill_time[i] = -1;
        }
        for (i = 0; i < 16; i++) {
            zombie_kills[i] = 0;
        }

        // write patches
        /* This is lua's code.
        // Not require - H-Ext already provided patch for CTF's repeated message glitch.
        writebyte(addresses.ctf_msgs_patch, 0xEB)
        // Not require - See EXTOnPlayerSpawnColor hook event for this process
        writebyte(addresses.color_patch, 0xEB)

        // Cannot patch due to variety halo builds are different.
        writebyte(addresses.slayer_score_patch, 0xEB)
        writebyte(addresses.nonslayer_score_patch, 0xEB)
        */

        // set color values - See EXTOnPlayerSpawnColor hook event for this process

        // recalculate team counters
        cur_human_count = pIPlayer->m_get_str_to_player_list(human_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, nullptr, nullptr);
        cur_zombie_count = pIPlayer->m_get_str_to_player_list(zombie_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, nullptr, nullptr);

        // recalculate how many "alpha" zombies there are
        alpha_zombie_count = getalphacount();
        team_play = pIHaloEngine->gameTypeLive->isTeamPlay;
        lastman_object = -1;

        // set gametype specific variables
        gametype = pIHaloEngine->gameTypeLive->game_stage;
        gametype_indicator = pIHaloEngine->gameTypeLive->objective_indicator;
        switch (gametype) {
            case GAMETYPE_CTF: { // CTF
                //writedword(addresses.flag_respawn_addr, 0xFFFFFFFF)
                //TODO: DelayWriteCoords timer is not needed.
                //UINT id = pITimer->m_add(EAOHashID, nullptr, 0); //0 second
                //join_team = zombie_team;
            }
            case GAMETYPE_SLAYER: { // Slayer
                //zombie_team = COLOR_BLUE;
                //human_team = COLOR_RED;
            }
            case GAMETYPE_ODDBALL: // Oddball
                //TODO: Added by RadWolfie - Support oddball anyway.
            case GAMETYPE_KOTH: { // KOTH
                //join_team = zombie_team;
                break;
            }
            default: { // Incompatible gametype
                VARIANT argList[1];
                VARIANTwstr(&argList[0], pIHaloEngine->gameTypeLive->name);
                pIPlayer->m_send_custom_message_broadcast(MF_ERROR, L"Incompatible gametype: ", 1, argList);
                return;
            }
        }

        starting_equipment_is_generic = !pIHaloEngine->gameTypeLive->isCustomEquipment;

        UINT id = pITimer->m_add(EAOHashID, nullptr, 30); //1 second
        if (id)
            vTimer.push_back({ id, eHumanTimer, -1 });

        id = pITimer->m_add(EAOHashID, nullptr, 6); //200 miliseconds
        if (id)
            vTimer.push_back({ id, eInvisCrouch, -1 });

        id = pITimer->m_add(EAOHashID, nullptr, 27); //900 miliseconds
        if (id)
            vTimer.push_back({ id, eBumpTimer, -1 });

        id = pITimer->m_add(EAOHashID, nullptr, 27); //200 milliseconds
        if (id)
            vTimer.push_back({ id, eCheckGameState, -1 });
    }
}

//RadWolfie - Not required.
//UINT getteamsize(e_color_team_index team) {};

// RadWolfie - Approved
// Gets the ammo mapId of the weapon, or nil if there's no ammo for the weapon.
// Accepts the weapon's mapId as an argument.
// Created by Wizard
s_ident getAmmoTagId(hTagHeader* mapId) {
    s_ident ammoTagId = -1;
    if (mapId == rifle_id) {
        ammoTagId = rifle_ammo_id->ident;
    } else if (mapId == needler_id) {
        ammoTagId = needler_ammo_id->ident;
    } else if (mapId == pistol_id) {
        ammoTagId = pistol_ammo_id->ident;
    } else if (mapId == rocket_id) {
        ammoTagId = rocket_ammo_id->ident;
    } else if (mapId == shotgun_id) {
        ammoTagId = shotgun_ammo_id->ident;
    } else if (mapId == sniper_id) {
        ammoTagId = sniper_ammo_id->ident;
    } else if (mapId == flame_id) {
        ammoTagId = flame_ammo_id->ident;
    }
    return ammoTagId;
}

//RadWolfie - Approved
void spawnAmmoForKiller(PlayerInfo killer, PlayerInfo victim) {
    s_biped* m_biped = (s_biped*)pIObject->m_get_address(victim.plS->CurrentBiped);
    if (m_biped) {
        // Don't drop ammo for last killer
        if (!cur_last_human.mS) {

            // Get coordinates to drop the ammo at.
            real_vector3d coord = m_biped->sObject.World;
            coord.z += 1;

            // Get the weapon of the killer so we can check its ammo type.
            s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(m_biped->sObject.Weapon);
            if (m_weapon) {
                hTagHeader* weapHeader = pIObject->m_lookup_tag(m_biped->sObject.Weapon);

                // Get weapon ammo's mapId of the killer's weapon
                s_ident tagId = m_weapon->sObject.ModelTag;
                s_ident ammoTagId = getAmmoTagId(weapHeader);
                if (ammoTagId.Tag != -1) {
                    pIObject->m_create(ammoTagId, s_ident(-1), 60, nullptr, &coord);
                }
            }
        }
    }
}

//RadWolfie - Not required.
//void all_players_zombies(PlayerInfo plI) {};

//RadWolfie - Approved
PlayerInfo ChooseRandomPlayer(e_color_team_index excludeTeam) {
    UINT randNumber;

    UINT count;
    PlayerInfoList plList;
    // loop through all 16 possible spots and add to table
    count = pIPlayer->m_get_str_to_player_list(excludeTeam == COLOR_TEAM_BLUE ? teamRedStr : teamBlueStr, &plList, nullptr);
    if (count == 0)
        return PlayerInfo();

    count--;
    randNumber = randomRange(0, count);
    return plList.plList[randNumber];
}

//RadWolfie - Approved
void destroyweapons(PlayerInfo plI) {
    s_biped* pl_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (pl_biped) {
        for (UINT i = 0; i < 4; i++) {
            if (pl_biped->Weapons[i].Tag!=-1)
                pIObject->m_destroy(pl_biped->Weapons[i]);
        }
    }
}

//RadWolfie - Approved
void AddScore(PlayerInfo plI, UINT amount) {
    //if (!plI.mS)
        //error
    human_time[plI.plR->MachineIndex] += amount;
}

//RadWolfie - Approved
void LoadTags() {
    falling_tag_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "globals\\falling");
    distance_tag_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "globals\\distance");

    camouflage_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\active camouflage");
    healthpack_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\health pack");
    overshield_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\over shield");
    fragnade_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "weapons\\frag grenade\\frag grenade");
    plasmanade_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "weapons\\plasma grenade\\plasma grenade");
    fragnade_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "weapons\\frag grenade\\frag grenade");
    plasmanade_tag_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "weapons\\plasma grenade\\plasma grenade");
    oddball_tag_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\ball\\ball");
    flag_tag_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\flag\\flag");
    rifle_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\assault rifle\\assault rifle");
    needler_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\needler\\mp_needler");
    pistol_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\pistol\\pistol");
    rocket_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\rocket launcher\\rocket launcher");
    shotgun_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\shotgun\\shotgun");
    sniper_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\sniper rifle\\sniper rifle");
    flame_id = pIObject->m_lookup_tag_type_name(TAG_WEAP, "weapons\\flamethrower\\flamethrower");
    rifle_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\assault rifle ammo\\assault rifle ammo");
    needler_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\needler ammo\\needler ammo");
    pistol_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\pistol ammo\\pistol ammo");
    rocket_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\rocket launcher ammo\\rocket launcher ammo");
    shotgun_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\shotgun ammo\\shotgun ammo");
    sniper_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\sniper rifle ammo\\sniper rifle ammo");
    flame_ammo_id = pIObject->m_lookup_tag_type_name(TAG_EQIP, "powerups\\flamethrower ammo\\flamethrower ammo");

    fuel_dmg1_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "weapons\\plasma_cannon\\effects\\plasma_cannon_explosion");
    fuel_dmg2_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "weapons\\plasma_cannon\\effects\\plasma_cannon_misfire");
    fuel_dmg3_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "weapons\\plasma_cannon\\effects\\plasma_cannon_trigger");
    fuel_dmg4_id = pIObject->m_lookup_tag_type_name(TAG_JPT_, "weapons\\plasma_cannon\\impact damage");
}

//RadWolfie - Extended
bool must_be_readied[4];
bool must_be_picked_up[4];
float melee_dmg_force[4];
float melee_response_force[4];

struct s_meta_weapon {
    BYTE unknown1[0x308];   //0x000
    bool bitfield0 : 1;     //0x308
    bool bitfield1 : 1;
    bool bitfield2 : 1;
    bool must_be_readied : 1;
    bool bitfield4 : 1;
    bool bitfield5 : 1;
    bool bitfield6 : 1;
    bool must_be_picked_up : 1;
    BYTE unknown2[0x8B];    //0x309
};
struct s_meta_melee {
    BYTE unknown1[0x1F4];   //0x000
    float force;            //0x1F4
};

//RadWolfie - Approved | except not sure about bitfield offset is correct. (It also edited the tagset too. So, once unload it remain as effect until map change.)
// This function will set the zombie weapon attributes
// It will loop through all the zombie weapons
// and give a 10x force to the zombie weapon's melee
void setZombieWeaponAttributes() {
    for (int i = 0; i < 4; i++) {
        if (zombie_weapon[i] != nullptr && zombie_weapon[i][0] != 0) {
            hTagHeader* weapon_tag = pIObject->m_lookup_tag_type_name(TAG_WEAP, zombie_weapon[i]);
            if (weapon_tag) {
                zweapon[i] = weapon_tag->ident;
                if (weapon_tag->ident.Tag >= 0xE0000000 && weapon_tag->ident.Tag <= 0xF0000000) {
                    s_meta_weapon* weapon_meta = (s_meta_weapon*)weapon_tag->group_meta_tag;
                    must_be_readied[i] = weapon_meta->must_be_readied;
                    weapon_meta->must_be_readied = 0;
                    must_be_picked_up[i] = weapon_meta->must_be_picked_up;
                    weapon_meta->must_be_picked_up = 1;

                    DWORD melee_dmg_mapId = *(DWORD*)(weapon_meta + 0x394 + 0xC);
                    if (melee_dmg_mapId && melee_dmg_mapId >= 0xD0000000 && melee_dmg_mapId <= 0xF0000000) {
                        hTagHeader* melee_dmg_tag = pIObject->m_lookup_tag(melee_dmg_mapId);
                        s_meta_melee* melee_dmg_meta = (s_meta_melee*)melee_dmg_tag->group_meta_tag;
                        melee_dmg_force[i] = melee_dmg_meta->force;
                        melee_dmg_meta->force = 10.0f;
                    }
                    DWORD melee_response_mapId = *(DWORD*)(weapon_meta + 0x3A4 + 0xC);
                    if (melee_response_mapId && melee_response_mapId >= 0xD0000000 && melee_response_mapId <= 0xF0000000) {
                        hTagHeader* melee_response_tag = pIObject->m_lookup_tag(melee_response_mapId);
                        s_meta_melee* melee_response_meta = (s_meta_melee*)melee_response_tag->group_meta_tag;
                        melee_response_force[i] = melee_response_meta->force;
                        melee_response_meta->force = 10.0f;
                    }
                }
            } else zweapon[i] = -1;
        } else zweapon[i] = 0;
    }
}

void unsetZombieWeaponAttributes() {
    for (int i = 0; i < 4; i++) {
        if (zombie_weapon[i] != nullptr && zombie_weapon[i][0] != 0) {
            hTagHeader* weapon_tag = pIObject->m_lookup_tag_type_name(TAG_WEAP, zombie_weapon[i]);
            if (weapon_tag) {
                zweapon[i] = weapon_tag->ident;
                if (weapon_tag->ident.Tag >= 0xE0000000 && weapon_tag->ident.Tag <= 0xF0000000) {
                    s_meta_weapon* weapon_meta = (s_meta_weapon*)weapon_tag->group_meta_tag;
                    weapon_meta->must_be_readied = must_be_readied[i];
                    weapon_meta->must_be_picked_up = must_be_picked_up[i];

                    DWORD melee_dmg_mapId = *(DWORD*)(weapon_meta + 0x394 + 0xC);
                    if (melee_dmg_mapId && melee_dmg_mapId >= 0xD0000000 && melee_dmg_mapId <= 0xF0000000) {
                        hTagHeader* melee_dmg_tag = pIObject->m_lookup_tag(melee_dmg_mapId);
                        s_meta_melee* melee_dmg_meta = (s_meta_melee*)melee_dmg_tag->group_meta_tag;
                        melee_dmg_meta->force = melee_dmg_force[i];
                    }
                    DWORD melee_response_mapId = *(DWORD*)(weapon_meta + 0x3A4 + 0xC);
                    if (melee_response_mapId && melee_response_mapId >= 0xD0000000 && melee_response_mapId <= 0xF0000000) {
                        hTagHeader* melee_response_tag = pIObject->m_lookup_tag(melee_response_mapId);
                        s_meta_melee* melee_response_meta = (s_meta_melee*)melee_response_tag->group_meta_tag;
                        melee_response_meta->force = melee_response_force[i];
                    }
                }
            } else zweapon[i] = -1;
        } else zweapon[i] = 0;
    }

}

//RadWolfie - Approved
// This function basically 'sets' a human's traits.
// It CAN convert a zombie to a human, or simply set the traits of a human.
void makehuman(PlayerInfo plI, bool forcekill) {
    // change the player's speed to human speed.
    // Should not be in the if statement because if statement is ONLY for changing teams IF they are a zombie.
    plI.plS->VelocityMultiplier = human_speed;

    // Check if we need to change their team.
    if (plI.plR->Team == zombie_team) {

        // Change their team.
        pIPlayer->m_change_team(&plI, human_team, forcekill);

    // Player is already a human, let's assign their color and possibly kill them... slowly...
    } else {
        if (forcekill)
            pIObject->m_kill(plI.plS->CurrentBiped);
    }
}

//RadWolfie - Approved
// This function basically 'sets' a zombie's traits.
// It CAN convert a human to a zombie, or simply set the traits of a zombie.
void makezombie(PlayerInfo plI, bool forcekill) {
    // change the player's speed to zombie speed.
    // Should not be in the if statement because if statement is ONLY for changing teams IF they are a zombie.
    plI.plS->VelocityMultiplier = zombie_speed;

    // Make sure the player is a human before we make them zombie.
    if (plI.plR->Team == human_team) {

        // Change their team.
        pIPlayer->m_change_team(&plI, zombie_team, forcekill);

    // Player is already a zombie, let's assign their color and possibly kill them.
    } else {
        if (forcekill)
            pIObject->m_kill(plI.plS->CurrentBiped);
    }
}

//RadWolfie - Approved
void noZombiesLeft() {
    if (team_play) {
        if (!hasPlayerChangeTimer) {
            allow_change = true;
            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, nozombiesleftmessage, 0, nullptr);
            UINT id = pITimer->m_add(EAOHashID, nullptr, 30);    //1 seconds
            if (id) {
                vTimer.push_back({ id, ePlayerChangeTimer, -1 });
                hasPlayerChangeTimer = true;
            }
        }
    } else {
        // pick a human and make them zombie.
        PlayerInfo newZomb = ChooseRandomPlayer(zombie_team);
        if (newZomb.plS) {
            makezombie(newZomb, true);
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &newZomb, zombie_message, 0, nullptr);
        }
    }
}

//RadWolfie - Approved
bool check_in_sphere(real_vector3d objectLoc, real_vector3d loc, float R) {
    bool Pass = false;
    if (pow(loc.x - objectLoc.x, 2) + pow(loc.y - objectLoc.y, 2) + pow(loc.z - objectLoc.z, 2) <= R) {
        Pass = true;
    }
    return Pass;
}

//RadWolfie - Approved
void WriteNavsToZombies(PlayerInfo plI) {
    PlayerInfoList plList;
    UINT count = pIPlayer->m_get_str_to_player_list(zombie_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, &plList, nullptr);
    for (UINT i = 0; i < count; i++) {
        plList.plList[i].plS->killInOrderObjective.index = plI.plR->PlayerIndex;
        plList.plList[i].plS->killInOrderObjective.salt = plI.plS->PlayerID;
    }
}

//RadWolfie - Approved | except unsure about invulnerable bit offset.
void onlastman() {
    // lookup the last man
    PlayerInfo lastManCheck;
    PlayerInfoList plList;
    s_biped* m_biped;
    s_weapon* m_weapon;
    UINT i = pIPlayer->m_get_str_to_player_list(human_team==COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, &plList, nullptr);
    if (i == 0)
        return;
    i--;
    //for is not required since m_get_str_to_player_list can get team list. Plus it only need to get one human, so "for" isn't even needed.
    //for (UINT i=0; i < count; i++) {
        // set the last human global variable
        cur_last_human = plList.plList[i];

        // Write the navs of zombies to the last man.
        if (gametype == GAMETYPE_SLAYER) { // Slayer
            WriteNavsToZombies(cur_last_human);
        }
        // give the last human speed and extra ammo
        cur_last_human.plS->VelocityMultiplier = lastman_speed;

        // find the last human's weapons
        m_biped = (s_biped*)pIObject->m_get_address(cur_last_human.plS->CurrentBiped);
        if (m_biped) {
            if (lastman_invulnerable) {

                // setup the invulnerable timer
                m_biped->sObject.noCollision = 1;
                lastman_object = cur_last_human.plS->CurrentBiped;
                UINT id = pITimer->m_add(EAOHashID, &cur_last_human, lastman_invulnerable * 33);
                if (id)
                    vTimer.push_back({ id, eRemoveLastManProtection, -1 });
            }

            // Give all weapons infinite ammo
            for (UINT w = 0; w < 4; w++) {
                m_weapon = (s_weapon*)pIObject->m_get_address(m_biped->Weapons[w]);
                if (m_weapon) {
                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = 0x7FFF;
                    m_weapon->BulletCountInCurrentClip = 0x7FFF;
                    pIObject->m_update(m_biped->Weapons[w]);
                }
            }
        }
    //}
    if (cur_last_human.mS) {
        VARIANT argList[2];
        VARIANTwstr(&argList[0], cur_last_human.plS->Name);
        VARIANTint(&argList[1], lastman_invistime);
        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, lastman_message, 2, argList);
        pIPlayer->m_apply_camo(&cur_last_human, lastman_invistime);
    }
}

//RadWolfie - Approved
bool PlayerInHill(PlayerInfo plI) {
    if (plI.plS->CurrentBiped.Tag != -1 && pIHaloEngine->gameTypeGlobals->kothGlobal.isInHill[plI.plR->PlayerIndex]) {
        return true;
    }
    return false;
}

//RadWolfie - Approved
void takenavsaway() {
    PlayerInfoList plList;
    UINT count = pIPlayer->m_get_str_to_player_list(L"*", &plList, nullptr);
    for (UINT i = 0; i < count; i++) {
        //TODO: killInOrderObjective set to -1 method does not show it is working for both PC & CE...Wizard said it is working in PC.
        //plList.plList[i].plS->killInOrderObjective.Tag = -1;
        plList.plList[i].plS->killInOrderObjective.salt = plList.plList[i].plS->PlayerID; //Alternate workaround for now.
        plList.plList[i].plS->killInOrderObjective.index = plList.plList[i].plR->PlayerIndex; //Alternate workaround for now.
    }
}

//RadWolfie - Not required.
//PlayerInfoList getplayertable(excludeTeam) {};

//RadWolfie - Not required.
//bool getteamplay() {};

//RadWolfie - Not required.
//UINT, UINT getteamsizes() {};

//RadWolfie - Not required.
//UINT getcolorval(color) {};

//RadWolfie - Not required.
//UINT getPreferredColor(playerId) {};

//RadWolfie - Not required, it is done internally.
//changeteam(playerId, forcekill) {};

//RadWolfie - Not required.
//getRandomColorTeam(cur_color) {};

//RadWolfie - Not required. These are just extras...
//objectidtoplayer(objectId) {};
//getplayerobject(playerId) {};
//getplayervehicleid(playerId) {};
//getplayervehicle(playerId) {};
//getplayerweaponid(playerId, slot) {};
//getplayerweapon(playerId, slot) {};
#pragma endregion

#pragma region Timers section

typedef struct assignData {
    int clipcount;
    int ammocount;
    float batterycount;
} assignData;
assignData AssignLeftoverZombieWeaponsData[16] = { 0 };
//RadWolfie - Approved | This is not needed, use hook func "EXTOnWeaponAssignmentDefault" and "EXTOnWeaponAssignmentCustom" instead.
// basically this timer assigns the tertiary and quarternary weapons to zombies if specified at the top
// this is needed since onweaponassignment isn't called for tertiary and quartenary weapons
void AssignLeftoverZombieWeapons(int index) {
    s_ident weaponId;
    PlayerInfo plI;

    if (!(index >= 0 && index <= 15))
        return;
    assignData* data = AssignLeftoverZombieWeaponsData + index;

    if (!pIPlayer->m_get_m_index(index, &plI, 1))
        return;
    s_biped* m_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (m_biped) {
        if (zweapon[2].Tag) {
            if (pIObject->m_create(zweapon[2], s_ident(-1), 60, &weaponId, &real_vector3d(5, 2, 2))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                if (m_weapon) {

                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = abs(data->batterycount - 1);

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
        }

        // make sure the script user isn't retarded so we don't get errors
        if (zweapon[2].Tag) {
            // create the quarternary weapon
            if (pIObject->m_create(zweapon[3], s_ident(-1), 60, &weaponId, &real_vector3d(1, 1, 1))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                // make sure createobject didn't screw up
                if (m_weapon) {

                    // assign the weapon to the player
                    pIObject->m_equipment_assign(plI.plS->CurrentBiped, weaponId);

                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = abs(data->batterycount - 1);

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
        }
    }
}

assignData AssignLeftoverHumanWeaponsData[16] = { 0 };
//RadWolfie - Approved | This is not needed, use hook func "EXTOnWeaponAssignmentDefault" and "EXTOnWeaponAssignmentCustom" instead.
// basically this timer assigns the tertiary and quarternary weapons to zombies if specified at the top
// this is needed since onweaponassignment isn't called for tertiary and quartenary weapons
void AssignLeftoverHumanWeapons(int index) {
    s_ident weaponId;
    PlayerInfo plI;

    if (!(index >= 0 && index <= 15))
        return;
    assignData* data = AssignLeftoverHumanWeaponsData + index;

    if (!pIPlayer->m_get_m_index(index, &plI, 1))
        return;
    s_biped* m_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (m_biped) {
        if (hweapon[2].Tag) {
            if (pIObject->m_create(hweapon[2], s_ident(-1), 60, &weaponId, &real_vector3d(5, 2, 2))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                if (m_weapon) {

                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = abs(data->batterycount - 1);

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
        }

        // make sure the script user isn't retarded so we don't get errors
        if (hweapon[3].Tag) {
            // create the quarternary weapon
            if (pIObject->m_create(hweapon[3], s_ident(-1), 60, &weaponId, &real_vector3d(1, 1, 1))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                // make sure createobject didn't screw up
                if (m_weapon) {

                    // assign the weapon to the player
                    pIObject->m_equipment_assign(plI.plS->CurrentBiped, weaponId);

                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = abs(data->batterycount - 1);

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
        }
    }
}

assignData AssignZombieWeaponsData[16] = { 0 };
//RadWolfie - Approved | This is not needed, use hook func "EXTOnWeaponAssignmentDefault" and "EXTOnWeaponAssignmentCustom" instead.
bool AssignZombieWeapons(int index, UINT count) {
    PlayerInfo plI;
    s_ident weaponId;

    if (!(index >= 0 && index <= 15))
        return false;
    assignData* data = AssignZombieWeaponsData + index;

    if (!pIPlayer->m_get_m_index(index, &plI, 1))
        return false;
    s_biped* m_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (m_biped) {
        // count is increased everytime the timer is called
        if (count == 1) {
            // gets rid of any weapons a zombie is holding
            destroyweapons(plI);
        }
        if (zweapon[count-1].Tag) {
            if (pIObject->m_create(zweapon[count-1], s_ident(0), 60, &weaponId, &real_vector3d(1, 1, 1))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                if (m_weapon) {

                    // Assign the weapon to the player.
                    pIObject->m_equipment_assign(plI.plS->CurrentBiped, weaponId);


                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = 1.0f;

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
            if (count < 4)
                return true;
            else
                return false;
        }
    }
    return false;
}

assignData AssignHumanWeaponsData[16] = { 0 };
//RadWolfie - Approved | This is not needed, use hook func "EXTOnWeaponAssignmentDefault" and "EXTOnWeaponAssignmentCustom" instead.
bool AssignHumanWeapons(int index, UINT count) {
    PlayerInfo plI;
    s_ident weaponId;

    if (!(index >= 0 && index <= 15))
        return false;
    assignData* data = AssignHumanWeaponsData + index;

    if (!pIPlayer->m_get_m_index(index, &plI, 1))
        return false;
    s_biped* m_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (m_biped) {
        // count is increased everytime the timer is called
        if (count == 1) {
            // gets rid of any weapons a zombie is holding
            destroyweapons(plI);
        }
        if (hweapon[count-1].Tag) {
            if (pIObject->m_create(hweapon[count-1], s_ident(0), 60, &weaponId, &real_vector3d(1, 1, 1))) {
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(weaponId);
                if (m_weapon) {

                    // Assign the weapon to the player.
                    pIObject->m_equipment_assign(plI.plS->CurrentBiped, weaponId);


                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = data->ammocount;
                    m_weapon->BulletCountInCurrentClip = data->clipcount;
                    m_weapon->ammoBattery = 1.0f;

                    // force it to sync
                    pIObject->m_update(weaponId);
                }
            }
            if (count < 4)
                return true;
            else
                return false;
        }
    }
    return false;
}

//RadWolfie - Approved | Need to setup custom input for custom message to client's console.
void haveSpeedTimerFunc(int index) {
    PlayerInfo plI;

    if (!pIPlayer->m_get_m_index(index, &plI, 1))
        return;

    pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, speedburst_end_message, 0, nullptr);
    if (plI.plR->Team == zombie_team) {
        plI.plS->VelocityMultiplier = zombie_speed;
    } else {
        plI.plS->VelocityMultiplier = human_speed;
    }
}

typedef struct bumpInfectionData {
    PlayerInfo virused_player;
    PlayerInfo zombie_player;
} bumpInfectionData;
bumpInfectionData bumpInfectionTimer[16];

//RadWolfie - Approved
void bumpTimerFunc() {
    // Last human cannot be infected by bump virus.
    if (!cur_last_human.mS) {
        PlayerInfoList plList;
        UINT count = pIPlayer->m_get_str_to_player_list(L"*", &plList, nullptr);
        s_biped* m_biped;
        s_biped* m_bump_biped;
        PlayerInfo bumpPlayer;
        VARIANT argList[1];
        for (UINT i = 0; i < count; i++) {
            m_biped = (s_biped*)pIObject->m_get_address(plList.plList[i].plS->CurrentBiped);
            if (m_biped) {
                m_bump_biped = (s_biped*)pIObject->m_get_address(m_biped->bump_objectId);
                if (m_bump_biped) {

                    // Check if object is another biped (player)
                    if (m_bump_biped->sObject.objType == 0) {
                        pIPlayer->m_get_ident(m_bump_biped->sObject.Player, &bumpPlayer);
                        if (bumpPlayer.mS) {

                            // Make sure a zombie is being bumped by a human, or vice versa.
                            // Also make sure a human isn't bumping another human (ffa)
                            if (plList.plList[i].plR->Team != bumpPlayer.plR->Team && (bumpPlayer.plR->Team == zombie_team || plList.plList[i].plR->Team == zombie_team)) {

                                // Only humans can be virused.
                                PlayerInfo virused_player;

                                PlayerInfo zombie_player;

                                // Check if bumped player is a zombie
                                if (bumpPlayer.plR->Team == zombie_team) {
                                    virused_player = plList.plList[i];
                                    zombie_player = bumpPlayer;
                                } else {
                                    virused_player = bumpPlayer;
                                    zombie_player = plList.plList[i];
                                }

                                if (!infected[virused_player.plR->MachineIndex]) {
                                    VARIANTwstr(&argList[0], virused_player.plR->PlayerName);
                                    pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &zombie_player, zombie_infect_human_message, 1, argList);
                                    pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &virused_player, human_infect_begin_message, 0, nullptr);
                                    
                                    UINT id = pITimer->m_add(EAOHashID, &virused_player, 30); // 1 second
                                    if (id)
                                        vTimer.push_back({ id, eBumpInfection, virused_player.plR->MachineIndex });

                                    bumpInfectionTimer[virused_player.plR->MachineIndex].virused_player = virused_player;
                                    bumpInfectionTimer[virused_player.plR->MachineIndex].zombie_player = zombie_player;
                                    infected[virused_player.plR->MachineIndex] = 45;
                                }
                            }

                            // Bump needs to be reset to none.
                            m_biped->bump_objectId = -1;
                            m_bump_biped->bump_objectId = -1;
                        }
                    }
                }
            }
        }
    }
    //Always loop timer.
    //return true;
}

//RadWolfie - Approved
// Known Issue: If the causer leaves, and then another player rejoins with the same playerId
// they will be causing the infection to this player. (will fix probably later)
bool bumpInfection(int index, UINT count) {
    VARIANT argList[1];

    if (!(index >= 0 && index <= 15))
        return false;
    bumpInfectionData* data = bumpInfectionTimer + index;

    UINT* infectCount = &infected[data->virused_player.plR->MachineIndex];
    if (*infectCount) {
        if (count % 5 == 0) {
            *infectCount -= 5;
            if (*infectCount <= 0) {
                *infectCount = 0; //RadWolfie - extra precautious.
                pIPlayer->m_change_team(&data->virused_player, zombie_team, true);
                //makezombie(data->virused_player, true); //RadWolfie - I think this is necessary... change team function shouldn't be happening.
                VARIANTwstr(&argList[0], data->virused_player.plR->PlayerName);
                pIPlayer->m_send_custom_message_broadcast(MF_BLANK, human_infect_end_message, 1, argList);
                return false;
            }
            VARIANTuint(&argList[0], *infectCount);
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &data->virused_player, human_infect_counter_message, 1, argList);
        }
        s_biped* m_biped_infected = (s_biped*)pIObject->m_get_address(data->virused_player.plS->CurrentBiped);
        s_biped* m_biped_zombie = (s_biped*)pIObject->m_get_address(data->zombie_player.plS->CurrentBiped);
        if (m_biped_infected) {
            if (*infectCount == 1) {
                m_biped_infected->sObject.Health = 1.0f;
            }

            //RadWolfie - 2 ^ 5 which is 7 decimal, aka 0.0-0.2 bitfield offset. However, bitfield 0.2 cause instant death.
            // Wizard wants to ignore shield. So, let's go with that instead.
            objDamageFlags flags = { 0 }; 
            flags.ignoreShield = 1;

            if (m_biped_zombie) {
                // We don't want ondamagelookup to use damage modifiers.
                dontModifyDmg = true;

                // Causer infected this player, so make the damage come from the causing playerId.
                pIObject->m_apply_damage_generic(data->virused_player.plS->CurrentBiped, data->zombie_player.plS->CurrentBiped, 0.01f, flags);
                dontModifyDmg = false;
            } else {
                // We don't want ondamagelookup to use damage modifiers.
                dontModifyDmg = true;

                // Causer is dead, do not apply damage from the causer.
                pIObject->m_apply_damage_generic(data->virused_player.plS->CurrentBiped, data->virused_player.plS->CurrentBiped, 0.01f, flags);
                dontModifyDmg = false;

            }
        }
    } else
        return false;
    return true;
}

void checkgamestate(int index) {
    PlayerInfo plI;

    pIPlayer->m_get_m_index(index, &plI, 1);

    // check if the game has started yet
    if (game_started) {

        // if no humans, but there are zombies, end the game
        if (cur_human_count == 0 && cur_zombie_count > 0) {
            if (plI.mS)
                memcpy(last_human_name, plI.plR->PlayerName, 26); // all_players_zombies(plI);
                pIHaloEngine->m_map_next();
            game_started = false;
        } else if (cur_human_count > 1 && cur_zombie_count == 0) {
            noZombiesLeft();
        } else if (cur_human_count == 1 && cur_zombie_count > 0 && !cur_last_human.mS) {
            onlastman();
        } else if (cur_last_human.mS && zombie_count == 0) {
            makehuman(cur_last_human, false);
            cur_last_human = PlayerInfo();
        } else if (cur_last_human.mS && cur_human_count > 1) {
            if (gametype == GAMETYPE_SLAYER) { //Slayer
                takenavsaway();
            }
            makehuman(cur_last_human, false);
            cur_last_human = PlayerInfo();
        }
    }
}

//RadWolfie - Override CTF's flags is not needed, can be used in a gametype.
// void DelayWriteCoords() {}

//RadWolfie - Approved | Need to check the player score system, since there is a patch in Wizard's lua script.
bool HumanTimer() {
    if (map_reset_boolean) {
        map_reset_boolean = false;
    }
    if (game_started) {
        PlayerInfoList plList;
        PlayerInfo* plI;
        s_object* m_object;
        UINT count = pIPlayer->m_get_str_to_player_list(L"*", &plList, nullptr);
        UINT i = 0;
        VARIANT argList[1];
        for (i; i < 16; i++) {
            human_time[i] = 0;
        }
        i = 0;
        for (i; i < count; i++) {
            plI = plList.plList + i;
            m_object = pIObject->m_get_address(plI->plS->CurrentBiped);
            if (m_object && plI->plR->Team == human_team)
                AddScore(*plI, 1);

            // do KOTH-specific stuff here:
            if (gametype == GAMETYPE_KOTH) {
                if (PlayerInHill(plList.plList[i]) && m_object) {
                    if (inhill_time[plI->plR->MachineIndex] == 0) {
                        m_object->noCollision = 1;
                        VARIANTwstr(&argList[0], plI->plR->PlayerName);
                        if (plI->plR->Team == human_team) {
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, koth_infect_begin_message, 1, argList);
                        } else {
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, koth_kill_begin_message, 1, argList);
                        }
                    } else if (inhill_time[plI->plR->MachineIndex] >= 10) {
                        if (plI->plR->Team == human_team) {
                            makezombie(*plI, true);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, koth_infect_end_message, 1, argList);
                        } else {
                            pIObject->m_kill(plI->plS->CurrentBiped);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, koth_kill_end_message, 1, argList);
                        }
                        inhill_time[plI->plR->MachineIndex] = 0;

                    } else if (plI->plR->Team == human_team) {
                            VARIANTuint(&argList[0], 10 - inhill_time[plI->plR->MachineIndex]);
                            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, plI, koth_kill_counter_message, 1, argList);
                    }
                } else if (m_object) {
                    inhill_time[plI->plR->MachineIndex] = 0;
                    m_object->noCollision = 0;
                }
            }

            // Write scores for player.
            if (gametype == GAMETYPE_KOTH) { // KOTH
                plI->plS->FlagStealCount = human_time[plI->plR->MachineIndex] * 30;
            } else if (gametype == GAMETYPE_CTF) { // CTF
                plI->plS->FlagCaptureCount = human_time[plI->plR->MachineIndex];
            } else if (gametype == GAMETYPE_SLAYER) { // Slayer
                pIHaloEngine->gameTypeGlobals->slayerGlobal.playerScore2[plI->plR->PlayerIndex] = human_time[plI->plR->MachineIndex];
            }
        }
    }
    return true;
}

// RadWolfie - Approved
bool invisCrouch() {
    if (!zombies_invisible_on_crouch && !humans_invisible_on_crouch) {
        return false;
    }
    PlayerInfoList plList;
    PlayerInfo* plI;
    s_biped* m_object;
    UINT count = pIPlayer->m_get_str_to_player_list(L"*", &plList, nullptr);
    for (UINT i = 0; i < count; i++) {
        plI = plList.plList + i;
        m_object = (s_biped*)pIObject->m_get_address(plI->plS->CurrentBiped);
        if (m_object && m_object->actionVehicle_crouch_stand == 3) {
            if ((plI->plR->Team == zombie_team && zombies_invisible_on_crouch) || (plI->plR->Team == human_team && humans_invisible_on_crouch)) {
                pIPlayer->m_apply_camo(plI, time_invis);
            }
        }
    }
    return true;
}

//RadWolfie - Approved
void MsgTimer(int index) {
    PlayerInfo plI;

    if (!(index >= 0 && index <= 15))
        return;

    if (pIPlayer->m_get_m_index(index, &plI, 1)) {
        pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, welcome_message, 0, nullptr);
        if (gametype == GAMETYPE_KOTH) { // KOTH
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, koth_additional_welcome_msg, 0, nullptr);
        } else if (gametype == GAMETYPE_SLAYER) { // Slayer
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, slayer_additional_welcome1_msg, 0, nullptr);
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, slayer_additional_welcome2_msg, 0, nullptr);
        }
    }
}

//RadWolfie - Approved
void NewGameTimer() {
    if (pIHaloEngine->serverHeader->totalPlayers) {
        UINT newgame_zombie_count = 0;

        PlayerInfoList plList;
        UINT count = pIPlayer->m_get_str_to_player_list(L"*", &plList, nullptr);

        // by default make all players human
        for (UINT i = 0; i < count; i++) {
            makehuman(plList.plList[i], false);
        }

        // make players zombie until the count has been met
        UINT finalZombies = 0;
        if (cur_zombie_count > 0) {
            finalZombies = cur_zombie_count;
        } else { // zombie count is used as a percentage.
            //TODO: RadWolfie - This... does not make sense at all.
            finalZombies = (UINT)ceilf(reinterpret_cast<float&>(pIHaloEngine->serverHeader->totalPlayers) * reinterpret_cast<float&>(cur_zombie_count));
        }

        // make last human zombie
        PlayerInfo last_human_index = PlayerInfo();

        if (last_human_next_zombie && pIHaloEngine->serverHeader->totalPlayers > 1) {
            for (UINT i = 0; i < count; i++) {
                if (wcscmp(plList.plList[i].plR->PlayerName, last_human_name) == 0) {

                    // make them a zombie and save their info
                    makezombie(plList.plList[i], true);
                    newgame_zombie_count = 1;
                    last_human_index = plList.plList[i];
                }
            }
        }

        // reset last human
        last_human_name[0] = 0;

        if (finalZombies == pIHaloEngine->serverHeader->totalPlayers) { // if 0 players they will be human
            finalZombies -= 1;
        } else if (finalZombies > pIHaloEngine->serverHeader->totalPlayers) { // fix the count
            finalZombies = pIHaloEngine->serverHeader->totalPlayers;
        } else if (max_zombie_count && finalZombies > max_zombie_count) { // cap the zombie count
            finalZombies = max_zombie_count;
        } else if (finalZombies < 1) {
            finalZombies = 1;
        }

        PlayerInfo newzomb;
        // loop through the players, randomly selecting ones to become zombies
        while (newgame_zombie_count < finalZombies) {

            // randomly choose a player
            newzomb = ChooseRandomPlayer(zombie_team);
            if (!newzomb.mS) {
                break;
            } else if (newzomb.mS != last_human_index.mS) {
                makezombie(newzomb, false);
                newgame_zombie_count += 1;
            }
        }

        // fix the team counters
        cur_zombie_count = finalZombies;
        cur_human_count = pIHaloEngine->serverHeader->totalPlayers - finalZombies;

        // reset the map
        pIHaloEngine->m_exec_command("sv_map_reset");
        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, game_start_message, 0, nullptr);

        // loop through and tell players which team they're on
        for (UINT i = 0; i < count; i++) {
            if (plList.plList[i].plR->Team == zombie_team) {
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plList.plList[i], zombie_message, 0, nullptr);
            } else {
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plList.plList[i], human_message, 0, nullptr);
            }
        }
    }
    game_started = true;
    resptime = 0;
}

//RadWolfie - Approved
bool PlayerChangeTimer(UINT count) {
    if (pIHaloEngine->serverHeader->totalPlayers == 0) {
        noLoop:
        hasPlayerChangeTimer = false;
        return false;
    }
    if (count != 6 && team_play) {
        UINT zombsize = cur_zombie_count;
        if (allow_change == false || zombsize > 0) {
            allow_change = false;
            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, timer_team_change_msg, 0, nullptr);
            goto noLoop;
        }
        VARIANT argList[1];
        VARIANTuint(&argList[0], 6 - count);
        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, nozombiesleft_counter_message, 1, argList);
        return true;
    } else { // timer up, force team change
        allow_change = false;
        PlayerInfo newZomb = ChooseRandomPlayer(zombie_team);
        if (newZomb.mS) {
            makezombie(newZomb, true);
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &newZomb, zombie_message, 0, nullptr);
        } else {
            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, L"PlayerChangeTimer got null", 0, nullptr);
        }
        goto noLoop;
    }
}

#if 0
//RadWolfie - Approved except does not save flag's ident.
UINT PutUnderMapID = -1;
void PutUnderMap(s_ident objectId) {
    s_object* m_object = pIObject->m_get_address(objectId);
    if (m_object) {
        objManaged moveLoc = { {0, 0, 0} };
        moveLoc.world = m_object->World;
        moveLoc.world.z -= 20;
        pIObject->m_move(objectId, moveLoc);
    }
}
#endif

//RadWolfie - Approved
void RemoveLastHumanProtection() {
    s_object* m_object = pIObject->m_get_address(lastman_object);
    lastman_object = -1;
    if (m_object) {
        m_object->noCollision = 0;
    }
}

#pragma region Timer Hook
//RadWolfie - Approved
CNATIVE dllAPI bool WINAPIC EXTOnTimerExecute(UINT id, UINT count) {
    std::vector<dTimer>::iterator iTimerBegin = vTimer.begin();
    bool reIterator = false;
    reProcessTimer:
    for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
        if (iTimer->TimerID == id) {
            if (reIterator) {
                vTimer.erase(iTimer);
                break;
            }
            //TODO: RadWolfie - This comment section is just to debug timer event onto visual studio's output window for verification.
            /*VARIANT argList[4];
            wchar_t msg[512];
            VARIANTuint(&argList[0], id);
            VARIANTuint(&argList[1], count);
            VARIANTint(&argList[2], iTimer->machine_index);
            VARIANTuint(&argList[3], iTimer->eFunc);
            pIUtil->m_formatVariantW(msg, 512, L"id: {0:4d} | count: {1:4d} | mIndex: {2:4X} | eFunc: {3:2d}\n", 4, argList);
            OutputDebugStringW(msg);*/
            switch (iTimer->eFunc) {
                case eNONE:
                default:
                    pIPlayer->m_send_custom_message_broadcast(MF_ALERT, L"Unknown timer trigger occurred. Please notify developer...", 0, nullptr);
                    break;
                case eAssignLeftOverZombieWeapon:
                    AssignLeftoverZombieWeapons(iTimer->machine_index);
                    break;
                case eAssignLeftOverHumanWeapon:
                    AssignLeftoverHumanWeapons(iTimer->machine_index);
                    break;
                case eAssignZombieWeapon:
                    if (AssignZombieWeapons(iTimer->machine_index, count))
                        return true;
                    break;
                case eAssignHumanWeapon:
                    if (AssignHumanWeapons(iTimer->machine_index, count))
                        return true;
                    break;
                case eHaveSpeedTimer:
                    haveSpeedTimerFunc(iTimer->machine_index);
                    break;
                case eBumpTimer:
                    bumpTimerFunc();
                    return true;
                    break;
                case eBumpInfection:
                    if (bumpInfection(iTimer->machine_index, count))
                        return true;
                    break;
                case eCheckGameState:
                    checkgamestate(iTimer->machine_index);
                    break;
                case eHumanTimer:
                    if (HumanTimer())
                        return true;
                    break;
                case eInvisCrouch:
                    if (invisCrouch())
                        return true;
                    break;
                case eMsgTimer:
                    MsgTimer(iTimer->machine_index);
                    break;
                case eNewGameTimer:
                    NewGameTimer();
                    break;
                case ePlayerChangeTimer:
                    if (PlayerChangeTimer(count))
                        return true;
                    break;
                case eRemoveLastManProtection:
                    RemoveLastHumanProtection();
                    break;
            }
            // RadWolfie - This is needed since it can perform a resize at any time without awareness.
            if (iTimerBegin._Ptr != vTimer.begin()._Ptr) {
                reIterator = true;
                goto reProcessTimer;
            }
            // RadWolfie - This is just a precaution since checkgamestate called map_next (Triggered EXTOnEndGame hook) function which cleared all timers.
            if (vTimer.size())
                vTimer.erase(iTimer);
            break;
        }
    }
    return false;
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnTimerCancel(UINT id) {
    for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
        if (iTimer->TimerID == id) {
            switch (iTimer->eFunc) {
            case eNONE:
            default:
                pIPlayer->m_send_custom_message_broadcast(MF_ALERT, L"Unknown timer trigger occurred. Please notify developer...", 0, nullptr);
                break;
            case eAssignLeftOverZombieWeapon:
                break;
            case eAssignLeftOverHumanWeapon:
                break;
            case eAssignZombieWeapon:
                break;
            case eAssignHumanWeapon:
                break;
            case eHaveSpeedTimer:
                break;
            case eBumpTimer:
                break;
            case eBumpInfection:
                infected[iTimer->machine_index] = 0;
                break;
            case eCheckGameState:
                break;
            case eHumanTimer:
                break;
            case eInvisCrouch:
                break;
            case eMsgTimer:
                break;
            case eNewGameTimer:
                break;
            case ePlayerChangeTimer:
                hasPlayerChangeTimer = false;
                break;
            case eRemoveLastManProtection:
                break;
            }
            vTimer.erase(iTimer);
            break;
        }
    }
}
#pragma endregion

#pragma endregion

#pragma region Hooks section
CNATIVE dllAPI void EXTOnMapLoad(s_ident mapTag, const wchar_t map[32]);

//RadWolfie - Approved | except need to implement check if client is hosting or not and enable commands support.
//TODO: Need to implement check if client is hosting or not and enable commands support.
CNATIVE dllAPI EAO_RETURN WINAPIC EXTOnEAOLoad(UINT hash) {
    EAOHashID = hash;
    pIUtil = getIUtil(hash);
    if (!pIUtil)
        return EAO_FAIL;
    pITimer = getITimer(hash);
    if (!pITimer)
        return EAO_FAIL;
    pIObject = getIObject(hash);
    if (!pIObject)
        return EAO_FAIL;
    pIPlayer = getIPlayer(hash);
    if (!pIPlayer)
        return EAO_FAIL;
    pIHaloEngine = getIHaloEngine(hash);
    if (!pIHaloEngine)
        return EAO_FAIL;
    pICommand = getICommand(hash);
    if (!pICommand)
        return EAO_FAIL;
    customMsg = getICIniFile(hash);
    if (!customMsg)
        return EAO_FAIL;
    if (pIHaloEngine->haloGameVersion == HV_TRIAL) //Does not support Halo Trial due to sv_map_reset is not featured in Trial version.
        return EAO_FAIL;

    srand((UINT)time(nullptr)); //NOTICE: THIS IS REQUIRED FOR RAND FUNCTION TO WORK!
    pl_null = PlayerInfo();
    hasPlayerChangeTimer = false;

    pICommand->m_reload_level(hash);
    if (!CALL_MEMBER_FN(customMsg, m_open_file, customMsgStr)) {
        if (CALL_MEMBER_FN(customMsg, m_create_file, customMsgStr))
            if (CALL_MEMBER_FN(customMsg, m_open_file, customMsgStr))
                goto successInitCustomMsg;
        CALL_MEMBER_FN(customMsg, m_release);
        return EAO_FAIL;
    }
successInitCustomMsg:
#pragma region Custom Message support
    CALL_MEMBER_FN(customMsg, m_load);

    //Hint Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, blockteamchange_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, blockteamchange_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, teamkill_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, teamkill_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_2, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_2, nozombiesleftmessage, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_2, nozombiesleftmessage, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_3, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_3, lastman_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_3, lastman_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_4, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_4, rejoin_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_4, rejoin_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_5, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_5, zombieinvis_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_5, zombieinvis_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_6, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_6, speedburst_begin_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_6, speedburst_begin_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_7, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_7, speedburst_end_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_7, speedburst_end_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_8, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_8, block_tree_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_8, block_tree_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_9, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_9, block_spot_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_9, block_spot_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_10, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_10, block_glitch_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_10, block_glitch_message, section_str_hint);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_11, section_str_hint)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_11, nozombiesleft_counter_message, section_str_hint);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_11, nozombiesleft_counter_message, section_str_hint);
    }



    // Death/Infection Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, falling_infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, falling_infected_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, guardian_infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, guardian_infected_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_2, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_2, kill_infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_2, kill_infected_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_3, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_3, teammate_infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_3, teammate_infected_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_4, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_4, suicide_infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_4, suicide_infected_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_5, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_5, human_kill_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_5, human_kill_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_6, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_6, falling_death_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_6, falling_death_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_7, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_7, guardian_death_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_7, guardian_death_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_8, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_8, teammate_death_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_8, teammate_death_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_9, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_9, suicide_death_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_9, suicide_death_message, section_str_death);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_10, section_str_death)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_10, infected_message, section_str_death);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_10, infected_message, section_str_death);
    }

    // Complement Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_complement)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, timer_team_change_msg, section_str_complement);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, timer_team_change_msg, section_str_complement);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_complement)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, zombie_backtap_message, section_str_complement);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, zombie_backtap_message, section_str_complement);
    }

    // New Team Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_new_team)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, human_message, section_str_new_team);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, human_message, section_str_new_team);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_new_team)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, zombie_message, section_str_new_team);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, zombie_message, section_str_new_team);
    }

    // Additional Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, welcome_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, welcome_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, koth_additional_welcome_msg, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, koth_additional_welcome_msg, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_2, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_2, slayer_additional_welcome1_msg, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_2, slayer_additional_welcome1_msg, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_3, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_3, slayer_additional_welcome2_msg, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_3, slayer_additional_welcome2_msg, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_4, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_4, cure_zombie_kill_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_4, cure_zombie_kill_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_5, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_5, game_start_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_5, game_start_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_6, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_6, zombie_infect_human_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_6, zombie_infect_human_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_7, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_7, human_infect_begin_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_7, human_infect_begin_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_8, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_8, human_infect_counter_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_8, human_infect_counter_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_9, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_9, human_infect_end_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_9, human_infect_end_message, section_str_misc);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_10, section_str_misc)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_10, cure_human_message, section_str_misc);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_10, cure_human_message, section_str_misc);
    }

    // Gametype Messages
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_gametype)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_0, koth_infect_begin_message, section_str_gametype);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, koth_infect_begin_message, section_str_gametype);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_gametype)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_1, koth_infect_end_message, section_str_gametype);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, koth_infect_end_message, section_str_gametype);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_2, section_str_gametype)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_2, koth_kill_begin_message, section_str_gametype);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_2, koth_kill_begin_message, section_str_gametype);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_3, section_str_gametype)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_3, koth_kill_end_message, section_str_gametype);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_3, koth_kill_end_message, section_str_gametype);
    }
    if (CALL_MEMBER_FN(customMsg, m_key_exist, str1_4, section_str_gametype)) {
        CALL_MEMBER_FN(customMsg, m_value_get, str1_4, koth_kill_counter_message, section_str_gametype);
    } else {
        CALL_MEMBER_FN(customMsg, m_value_set, str1_4, koth_kill_counter_message, section_str_gametype);
    }

    /*TODO: Commands are currently disabled for now.
    //General section, 1.x = Variety messages for the commands section
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_0, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_0, L"disable", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_1, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_1, L"enable", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_2, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_2, L"Infection gametype", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_3, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_3, L"red team", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_4, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_4, L"blue team", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_5, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_5, L"Zombie", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_6, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_6, L"Human", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_7, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_7, L"Last man", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_8, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_8, L"forbidden", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_9, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_9, L"permitted", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_10, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_10, L"Infect by fall", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_11, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_11, L"Infect by guardian", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_12, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_12, L"Infect by suicide", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_13, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_13, L"Infect by betrayal", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_14, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str1_14, L"Invisible zombie crouching", section_str_general);

    //if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_, section_str_general))
    //CALL_MEMBER_FN(customMsg, m_value_set, str1_, , section_str_general);

    //Commands section, 2.x = Variety messages combo
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_0, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_0, L"{0:s} is currently {1:s}.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_1, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_1, L"{0:s} is now set to {1:s}.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_2, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_2, L"{0:s} is already {1:s}.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_3, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_3, L"{0:s} is currently on {1:s}.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_4, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_4, L"{0:s} is already on {1:s}.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_5, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_5, L"{0:s} is currently at {1:d} second(s).", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_6, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_6, L"{0:s} is now set to {1:d} second(s).", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_7, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_7, L"{0:s} is already set to {1:d} second(s).", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_8, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_8, L"{0:s} is currently at x{1:f} speed.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_9, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_9, L"{0:s} is now set to x{1:f} speed.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_10, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_10, L"{0:s} is already set to x{1:f} speed.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_11, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_11, L"{0:s} team are currently {1:s} to enter the vehicle.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_12, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_12, L"{0:s} team are set {1:s} to enter the vehicle.", section_str_general);
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_13, section_str_general))
        CALL_MEMBER_FN(customMsg, m_value_set, str2_13, L"{0:s} team are already set {1:s} to enter the vehicle.", section_str_general);
    */
    /*if (!CALL_MEMBER_FN(customMsg, m_key_exist, str2_, section_str_general))
    CALL_MEMBER_FN(customMsg, m_value_set, str2_, , section_str_general);*/
    /*if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_, ))
    CALL_MEMBER_FN(customMsg, m_value_set, str1_, , );
    if (!CALL_MEMBER_FN(customMsg, m_key_exist, str1_, ))
    CALL_MEMBER_FN(customMsg, m_value_set, str1_, , );*/
    CALL_MEMBER_FN(customMsg, m_save);
    CALL_MEMBER_FN(customMsg, m_close);
#pragma endregion

    /*TODO: Commands are currently disabled for now.
    pICommand->m_add(EAOHashID, eao_infection_enable_str, eaoGametypeInfectionEnable, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_zombie_team_str, eaoGametypeInfectionZombieTeam, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_zombie_respawn_str, eaoGametypeInfectionZombieRespawn, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_human_respawn_str, eaoGametypeInfectionHumanRespawn, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_zombie_speed_str, eaoGametypeInfectionZombieSpeed, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_human_speed_str, eaoGametypeInfectionHumanSpeed, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_last_man_speed_str, eaoGametypeInfectionLastManSpeed, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_last_man_next_zombie_str, eaoGametypeInfectionLastManNextZombie, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_zombie_allow_in_vehicle_str, eaoGametypeInfectionZombieAllowInVehicle, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_human_allow_in_vehicle_str, eaoGametypeInfectionHumanAllowInVehicle, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_infect_on_fall_str, eaoGametypeInfectionInfectOnFall, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_infect_on_guardians_str, eaoGametypeInfectionInfectOnGuardians, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_infect_on_suicide_str, eaoGametypeInfectionInfectOnSuicide, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_infect_on_betrayal_str, eaoGametypeInfectionInfectOnBetrayal, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    pICommand->m_add(EAOHashID, eao_infection_zombie_invisible_on_crouch_str, eaoGametypeInfectionZombieInvisibleOnCrouch, EXTPluginInfo.sectors.sect_name1, 1, 2, false, modeAll);
    */

    //Wizard's code
    if ((*pIHaloEngine->mapStatus)->upTime >= 30 && pIHaloEngine->mapCurrent->type == 1) { //TODO: pIHaloEngine->mapCurrent->type is a temporary solution atm.
        wchar_t map_name[32] = { 0 };
        pIUtil->m_toCharW(pIHaloEngine->mapCurrent->name, 32, map_name);
        EXTOnMapLoad(0, map_name);
    } else {
        game_started = false;
    }
    pIPlayer->m_send_custom_message(MF_INFO, MP_RCON, nullptr, L"Infection GameType support added.", 0, nullptr);
    return EAO_CONTINUE;
}

//RadWolfie - Approved | except need to implement correct uninitialize method since it can be unload in middle of gameplay.
//TODO: Need to implement correct uninitialize method since it can be unload in middle of gameplay.
CNATIVE dllAPI void WINAPIC EXTOnEAOUnload() {
    //Uninit();
    unsetZombieWeaponAttributes();
    CALL_MEMBER_FN(customMsg, m_release);

    /*TODO: Commands are currently disabled for now.
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionEnable, eao_infection_enable_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionZombieTeam, eao_infection_zombie_team_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionZombieRespawn, eao_infection_zombie_respawn_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionHumanRespawn, eao_infection_human_respawn_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionZombieSpeed, eao_infection_zombie_speed_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionHumanSpeed, eao_infection_human_speed_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionLastManSpeed, eao_infection_last_man_speed_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionLastManNextZombie, eao_infection_last_man_next_zombie_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionZombieAllowInVehicle, eao_infection_zombie_allow_in_vehicle_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionHumanAllowInVehicle, eao_infection_human_allow_in_vehicle_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionInfectOnFall, eao_infection_infect_on_fall_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionInfectOnGuardians, eao_infection_infect_on_guardians_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionInfectOnSuicide, eao_infection_infect_on_suicide_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionInfectOnBetrayal, eao_infection_infect_on_betrayal_str);
    pICommand->m_delete(EAOHashID, eaoGametypeInfectionZombieInvisibleOnCrouch, eao_infection_zombie_invisible_on_crouch_str);
    */

    pIPlayer->m_send_custom_message(MF_INFO, MP_RCON, nullptr, L"Infection GameType support removed.", 0, nullptr);
}

//RadWolfie - Approved | except need check if running a game or not.
//TODO: Need implement support to check if client is hosting PLUS is not idling.
CNATIVE dllAPI void EXTOnMapLoad(s_ident mapTag, const wchar_t map[32]) {
        InitializeGame();
        LoadTags();
        setZombieWeaponAttributes();
        for (UINT i = 0; i < 4; i++) {
            hTagHeader* weaponTag =  pIObject->m_lookup_tag_type_name(TAG_WEAP, human_weapon[i]);
            if (weaponTag)
                hweapon[i] = weaponTag->ident;
            else
                hweapon[i] = -1;
        }

        // reset our variables
        cur_zombie_count = 0;
        cur_human_count = 0;
        //cur_players = 0; //TODO: Not required
        resptime = 5;
        //Map = map
        //name_table = {}

        // the game hasn't started yet, will once timer runs down
        game_started = false;
        UINT id = pITimer->m_add(EAOHashID, nullptr, 225); // 7.5 seconds
        if (id)
            vTimer.push_back({ id, eNewGameTimer, -1 });

        if (wcscmp(map, L"putput") == 0 || wcscmp(map, L"longest") == 0 || wcscmp(map, L"beavercreek") == 0 || wcscmp(map, L"carousel") == 0 || wcscmp(map, L"wizard") == 0) {
            zombie_speed = 1.5f;
            lastman_speed = 1.25f;
        }
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnEndGame(int stage) {
    game_started = false;
    if (stage == 1) { //For safety purpose.
        for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
            pITimer->m_delete(EAOHashID, iTimer->TimerID);
        }
        vTimer.clear();
    }
}

//RadWolfie - Approved | Except need implement full support for rejoiner player.
CNATIVE dllAPI void WINAPIC EXTOnPlayerJoin(PlayerInfo plI) {
    // update the player counts
    //cur_players += 1; //TODO: Not required.
    if (plI.plR->Team == zombie_team) {
        cur_zombie_count += 1;
    } else {
        cur_human_count += 1;
    }

    // onteamdecision isn't called for ffa gametypes
    // Not require

    alpha_zombie_count = getalphacount();
    //UINT thisTeamSize = pIPlayer->m_get_str_to_player_list(plI.plR->Team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, nullptr, nullptr);
    bool alreadyExists = false;

    // check if the player has joined this game previously
    // Do nothing if we're on the last human stage.
    if (!cur_last_human.plEx) {
        // check if the player has joined this game previously
        // Do nothing if we're on the last human stage.
        //TODO: Need implement support (relative to name_table)
        /*for k, v in pairs(name_table) do
        if name == k then
        if thisTeamSize >= 1 then
        privatesay(playerId, rejoin_message)
        team = v
        end
        alreadyExists = true
        break
        end
        end*/
    }
    // add team entry for this name
    // Always false if we're on the last human stage.
    if (!alreadyExists) {
        //name_table[name] = team
    }
    // make sure the game is started
    if (game_started) {
        // check if the player is a zombie
        // OR if the game is at last_human(We dont want anyone joining as humans during the last human stage.)
        if (plI.plR->Team == zombie_team || cur_last_human.plEx) {
            // make the player a zombie (or at least set their traits)
            makezombie(plI, false);

            // send them a zombie message
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, zombie_message, 0, nullptr);
        } else {
            // make the player a human (or at least set their traits)
            makehuman(plI, false);

            // send them a human message
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, human_message, 0, nullptr);
        }

        // send the player the welcome message
        pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, welcome_message, 0, nullptr);
        if (gametype == GAMETYPE_KOTH) {
            pIPlayer->m_send_custom_message(MF_BLANK, MP_CHAT, &plI, koth_additional_welcome_msg, 0, nullptr);
        } else if (gametype == GAMETYPE_SLAYER && gametype_indicator==1) {
            pIPlayer->m_send_custom_message(MF_BLANK, MP_CHAT, &plI, slayer_additional_welcome1_msg, 0, nullptr);
        }
        UINT id = pITimer->m_add(EAOHashID, &plI, 7); // 200 milliseconds
        if (id)
            vTimer.push_back({ id, eCheckGameState, -1 });
    } else {
        UINT id = pITimer->m_add(EAOHashID, &plI, 300); // 10 seconds
        if (id)
            vTimer.push_back({ id, eMsgTimer, plI.plR->MachineIndex });
    }
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnPlayerQuit(PlayerInfo plI) {
    if (game_started) {
        // check if they're a zombie
        if (plI.plR->Team == zombie_team) {
            // take one away from the current zombie count
            cur_zombie_count -= 1;
        } else {
            // take one away from the current human count
            cur_human_count -= 1;
        }
    }
    // if last human is leaving, reset it
    if (plI.mS == cur_last_human.mS) {
        cur_last_human = PlayerInfo();
    }

    // minus one from current players
    //cur_players -= 1; //TODO: Not required

    // check the current game state (the player that left might have been the last human or the only zombie)
    UINT id = pITimer->m_add(EAOHashID, nullptr, 30); // 1 second
    if (id)
        vTimer.push_back({ id, eCheckGameState, -1 });

    // update team within table
    //TODO: update team within table
    //name_table[name] = team
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnPlayerSpawn(PlayerInfo plI, s_ident ident, s_biped* pl_biped) {

    // get the player's team (Not needed)
    //local team = getteam(playerId)
    usedFlashlight[plI.plR->MachineIndex] = false;
    UINT id;

    if (plI.plR->Team == zombie_team) {

        // set nade counts
        pl_biped->grenade0 = zombie_frag_count;
        pl_biped->grenade1 = zombie_plasma_count;

        // set shields to nothing
        pl_biped->sObject.Shield1 = 0.0f;  // Shields to 0.
        pl_biped->sObject.ShieldMax = 0.0f; // Max shields to 0.

        // set health to 2x
        pl_biped->sObject.Health *= 2;

        // initiate ammo variables
        unsigned short clipcount = alphazombie_clip_count;
        unsigned short ammocount = alphazombie_ammo_count;
        float batterycount = alphazombie_battery_count;

        // initiate ammo variables
        if (cur_zombie_count > reinterpret_cast<int&>(alpha_zombie_count)) {
            clipcount = zombie_clip_count;
            ammocount = zombie_ammo_count;
            batterycount = zombie_battery_count;
        } else { // set alpha nades
            pl_biped->grenade0 = alphazombie_frag_count;
            pl_biped->grenade1 = alphazombie_plasma_count;
        }
        // check if the starting equipment is generic
        if (starting_equipment_is_generic) {
            int i = 0;
            while (i < 4) { // loop through the player's weapons (primary through quartenary)

                // get the player's weapon
                s_weapon* m_weapon = (s_weapon*)pIObject->m_get_address(pl_biped->Weapons[i]);

                // make sure a weapon exists
                if (m_weapon) {

                    // set the ammo
                    m_weapon->BulletCountInRemainingClips = ammocount;
                    m_weapon->BulletCountInCurrentClip = clipcount;

                    // set the battery (math.abs is absolute value meaning it takes the opposite of battery count (meaning 0 needs to be 1 and 1 needs to be 0))
                    m_weapon->ammoBattery = abs(batterycount - 1.0f);

                    // force it to sync
                    pIObject->m_update(pl_biped->Weapons[i]);
                }
                i++;
            }
            if (plI.plR->Team == zombie_team) {
                // check if we still need to assign leftover weapons to zombies (tertiary and quartenary weapons)
                if (zweapon[3].Tag) { // make sure the script user isn't retarded
                    // assign the leftover weapons to the player
                    AssignLeftoverZombieWeaponsData[plI.plR->MachineIndex] = {
                        clipcount,
                        ammocount,
                        batterycount };
                    id = pITimer->m_add(EAOHashID, &plI, 0);
                    if (id)
                        vTimer.push_back({ id, eAssignLeftOverZombieWeapon, plI.plR->MachineIndex });
                }
            } else {
                // check if we still need to assign leftover weapons to humans (tertiary and quartenary weapons)
                if (hweapon[3].Tag) { // make sure the script user isn't retarded
                    // assign the leftover weapons to the player
                    AssignLeftoverHumanWeaponsData[plI.plR->MachineIndex] = {
                        clipcount,
                        ammocount,
                        batterycount };
                    id = pITimer->m_add(EAOHashID, &plI, 0);
                    if (id)
                        vTimer.push_back({ id, eAssignLeftOverHumanWeapon, plI.plR->MachineIndex });
                }
            }
        } else {
            // assign the correct weapons to the player
            if (plI.plR->Team == zombie_team) {
                AssignZombieWeaponsData[plI.plR->MachineIndex] = {
                    clipcount,
                    ammocount,
                    batterycount };

                id = pITimer->m_add(EAOHashID, &plI, 0);
                if (id)
                    vTimer.push_back({ id, eAssignZombieWeapon, plI.plR->MachineIndex });
            } else {

                AssignHumanWeaponsData[plI.plR->MachineIndex] = {
                    clipcount,
                    ammocount,
                    batterycount };

                id = pITimer->m_add(EAOHashID, &plI, 0);
                if (id)
                    vTimer.push_back({ id, eAssignHumanWeapon, plI.plR->MachineIndex });
            }
        }
    }

    // check if the gametype is slayer
    if (gametype == GAMETYPE_SLAYER) {
        if (cur_last_human.mS) { // write the navpoint to the current last human
            plI.plS->killInOrderObjective.index = cur_last_human.plR->PlayerIndex;
            plI.plS->killInOrderObjective.salt = cur_last_human.plS->PlayerID;
        } else { // there is no last human so put a nav above this player's head
            //TODO: killInOrderObjective set to -1 method does not show it is working for both PC & CE...Wizard said it is working in PC.
            //plI.plS->killInOrderObjective.Tag = -1;
            plI.plS->killInOrderObjective.index = plI.plR->PlayerIndex; //Alternate workaround for now.
            plI.plS->killInOrderObjective.salt = plI.plS->PlayerID;
        }
    }
}

//RadWolfie - Does not exist
//TODO: RadWolfie - OnPlayerSpawnEnd does not exist to port in H-Ext... And I think it is require for better management.
//PS. It is only for last human given infinite ammo.
// EXTOnPlayerSpawnEnd (N/A)

//RadWolfie - Approved | except it can be improvise with EXTOnWeaponAssignmentCustom include support
CNATIVE dllAPI void WINAPIC EXTOnWeaponAssignmentDefault(PlayerInfo plI, s_ident ownerObjId, s_tag_reference* cur_weap_id, unsigned int order, s_ident* new_weap_id) {
    if (plI.plR->Team == zombie_team) {
        if (order == 0 && zweapon[0].Tag) {
            *new_weap_id = zweapon[0].Tag;
        } else if (order == 1 && zweapon[1].Tag) {
            *new_weap_id = zweapon[1].Tag;
        }
    }
}

#if 0
//RadWolfie - Approved except does not save the ident of the flag.
CNATIVE dllAPI bool WINAPIC EXTOnObjectCreationAttempt(PlayerInfo plOwner, objCreationInfo object_creation, objCreationInfo* change_object, bool isOverride) {
    if (gametype == GAMETYPE_CTF && object_creation.map_id.Tag == flag_tag_id->ident.Tag) {
        PutUnderMapID = pITimer->m_add(EAOHashID, nullptr, 0);
    }
}
#endif

//RadWolfie - Approved
CNATIVE dllAPI bool WINAPIC EXTOnObjectInteraction(PlayerInfo plI, s_ident obj_id, s_object* m_object, hTagHeader* tagId) {
    // Allow any object interaction by default.
    bool response = true;

    // Make sure the game is started before we do anything.
    if (!game_started) {
        return true;
    }

    // We don't want ANYONE picking up oddballs or flags.
    if (tagId == oddball_tag_id || tagId == flag_tag_id) {
        return false;
    }

    // Check if player is on the zombie team.
    if (plI.plR->Team == zombie_team) {

        // Set default return value to false, because we don't want them picking up any weapons
        // other than the ones listed here.
        response = false;

        // Check if the player is trying to pick up a camo.
        if (tagId == camouflage_tag_id) {

            // Check if the player is invisible (otherwise this will spam)
            s_biped* pl_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
            if (pl_biped->isInvisible != 0x51) {

                // 50% chance of making the entire zombie team invisible.
                int doInvis = randomRange(1, 10);
                if (doInvis > 5) {
                    // Make entire zombie team invisible for 30 seconds.
                    PlayerInfoList plList;
                    int count = pIPlayer->m_get_str_to_player_list(zombie_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, &plList, nullptr);
                    for (int i = 0; i < count; i++) {
                        pIPlayer->m_apply_camo(&plList.plList[i], 30);
                    }
                    pIPlayer->m_send_custom_message_broadcast(MF_BLANK, zombieinvis_message, 0, nullptr);

                    //RadWolfie - Bugfixes
                    pIObject->m_destroy(obj_id);
                } else {
                    response = true;
                }
                    //RadWolfie - Bugfigxes end
            }
            // We want to allow zombies to pick up overshields and health packs.
        } else if (tagId == overshield_tag_id || tagId == healthpack_tag_id) {
            response = true;
        }
        // I was too lazy to implement onweaponpickup, this will do just fine though.
    } else if (cur_last_human.mS == plI.mS) {
        if (m_object->objType == 2) { // check if this is a weapon
            // if so, give it infinite ammo
            ((s_weapon*)m_object)->BulletCountInRemainingClips = 0x7FFF; // lasthuman ammocount
            ((s_weapon*)m_object)->BulletCountInCurrentClip = 0x7FFF; // lasthuman clipcount
        }
        // Player is infected and trying to pick up a health pack to cure them.
    } else if (plI.plR->Team == human_team && tagId == healthpack_tag_id) {
        if (infected[plI.plR->MachineIndex]) {
            for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
                if (iTimer->eFunc == eBumpInfection && iTimer->machine_index == plI.plR->MachineIndex) {
                    pITimer->m_delete(EAOHashID, iTimer->TimerID);
                    vTimer.erase(iTimer);
                    break;
                }
            }
            infected[plI.plR->MachineIndex] = 0;
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, cure_human_message, 0, nullptr);
        }
    }
    return response;
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnPlayerDeath(PlayerInfo killer, PlayerInfo victim, int mode, bool* showMessage) {
    // This variable determines if the kill message is sent, or blocked.
    // We're setting it to the default (nil) which sets to Phasor's default (true)
    bool response = true;

    // This will be 'true' if this function infects a player.
    bool infecting = true;

    // Can't have bump infection if player is already dead.
    infected[victim.plR->MachineIndex] = 0;

    // Make sure the game is started before we do anything.
    if (game_started) {

        if (resptime) {
            victim.plS->RespawnTimer = resptime * 33;
        } else if (human_spawn_time && victim.plR->Team == human_team) {
            victim.plS->RespawnTimer = human_spawn_time * 33;
        } else if (zombie_spawn_time && victim.plR->Team == zombie_team) {
            victim.plS->RespawnTimer = zombie_spawn_time * 33;
        }
        VARIANT argList[2];

        switch (mode) {
            //case 0: //server kill
            //case 3: //killed by vehicle
            default:
                break;
            case 1: {
                // Apparently people 'fall' to their death when the map resets.
                if (map_reset_boolean)
                    break;
                // If this is true then a person who dies from falling will be infected
                if (infect_on_fall) {

                    // We need to check the victim's team to see if we
                    // need to make them a zombie (if they're human)
                    if (victim.plR->Team == human_team) {
                        infecting = true;
                        if (falling_infected_message[0]) {
                            response = false;
                            VARIANTwstr(&argList[0], victim.plS->Name);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, falling_infected_message, 1, argList);
                        }
                        makezombie(victim, false);
                    }
                }

                // Check if a kill message has been serversaid.
                // If not, let's serversay one.
                if (response) {
                    if (falling_death_message[0]) {
                        response = false;
                        VARIANTwstr(&argList[0], victim.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, falling_death_message, 1, argList);
                    }
                }
                break;
            }
            case 2: { // killed by guardians

                // If this is true then a person who dies from the guardians will be infected
                if (infect_on_guardians) {

                    // We need to check the victim's team to see if we
                    // need to make them a zombie (if they're human)
                    if (victim.plR->Team == human_team) {
                        infecting = true;
                        if (guardian_infected_message[0]) {
                            response = false;
                            VARIANTwstr(&argList[0], victim.plS->Name);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, guardian_infected_message, 1, argList);
                        }
                        makezombie(victim, false);
                    }
                }

                // Check if a kill message has been serversaid.
                // If not, let's serversay one.
                if (response) {
                    if (guardian_death_message[0]) {
                        response = false;
                        VARIANTwstr(&argList[0], victim.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, guardian_death_message, 1, argList);
                    }
                }
                break;
            }
            case 4:
            case 5: { // killed by another player

                // Check if zombie is killing a human.
                // EVENT: Zombie infecting a human.
                if (killer.plR->Team == zombie_team && victim.plR->Team == human_team) {
                    infecting = true;
                    if (kill_infected_message[0]) {
                        response = false;
                        VARIANTwstr(&argList[0], killer.plS->Name);
                        VARIANTwstr(&argList[1], victim.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, kill_infected_message, 2, argList);
                    }
                    makezombie(victim, false);

                    // Add 30 to the zombie's score to reward their infection of a human.
                    AddScore(killer, 30);

                    // Check if the zombie has gotten enough kills to become a human again.
                    zombie_kills[killer.plR->MachineIndex] += 1;
                    if (zombie_kills[killer.plR->MachineIndex] >= 5) {
                        VARIANTwstr(&argList[0], killer.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, cure_zombie_kill_message, 1, argList);
                        zombie_kills[killer.plR->MachineIndex] = 0;
                        makehuman(killer, true);

                        // send killer a human message.
                        pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &killer, human_message, 0, nullptr);
                    }
                // FFA CHECK: Make sure human isn't trying to kill another human.
                // EVENT: Betrayal.
                } else if (killer.plR->Team == victim.plR->Team) {
                    if (infect_on_betrayal) {

                        // We don't care about betrayals on the zombie team.
                        if (killer.plR->Team == human_team) {

                            // Infect this jerk for betraying a teammate.
                            infecting = true;
                            if (teammate_infected_message[0]) {
                                response = false;
                                VARIANTwstr(&argList[0], killer.plS->Name);
                                VARIANTwstr(&argList[1], victim.plS->Name);
                                pIPlayer->m_send_custom_message_broadcast(MF_BLANK, teammate_infected_message, 2, argList);
                            }
                            makezombie(killer, true);
                        }
                    }

                    // Check if a betrayal message has been serversaid.
                    // If not, let's serversay one.
                    if (response) {
                        if (teammate_death_message[0]) {
                            response = false;
                            VARIANTwstr(&argList[0], killer.plS->Name);
                            VARIANTwstr(&argList[1], victim.plS->Name);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, teammate_death_message, 2, argList);
                        }
                    }
                // BEWARE: This WILL pass in FFA if human is killing another human.
                // EVENT: A non-zombie is killing someone.
                } else if (killer.plR->Team == human_team) {
                    if (human_kill_message[0]) {
                        response = false;
                        VARIANTwstr(&argList[0], killer.plS->Name);
                        VARIANTwstr(&argList[1], victim.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, human_kill_message, 2, argList);
                    }

                    // Add 5 to the human's score to reward their kill.
                    AddScore(killer, 5);

                    spawnAmmoForKiller(killer, victim);
                }
                break;
            }
            case 6: { // suicide

                      // If this is true then a person who dies from committing suicide will be infected
                if (infect_on_suicide) {

                    // We need to check the victim's team to see if we
                    // need to make them a zombie (if they're human)
                    if (victim.plR->Team == human_team) {
                        infecting = true;
                        if (suicide_infected_message[0]) {
                            response = false;
                            VARIANTwstr(&argList[0], victim.plS->Name);
                            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, suicide_infected_message, 1, argList);
                        }
                        makezombie(victim, false);
                    }
                }

                // Check if a kill message has been serversaid.
                // If not, let's serversay one.
                if (response) {
                    if (suicide_death_message[0]) {
                        response = false;
                        VARIANTwstr(&argList[0], victim.plS->Name);
                        pIPlayer->m_send_custom_message_broadcast(MF_BLANK, suicide_death_message, 1, argList);
                    }
                }
                break;
            }
        }
        if (victim.plR->Team == zombie_team) {

            // don't let the zombie drop weapons on death:
            destroyweapons(victim);
        }
        UINT id = pITimer->m_add(EAOHashID, &victim, 6); // 200 milliseconds - checkgamestate for victim
        if (id)
            vTimer.push_back({ id, eCheckGameState, victim.plR->MachineIndex });

        // Use normal infection messages from old zombies, if no replacement message is used.
        if (infecting && response) {
            //CALL_MEMBER_FN(customMsg, m_value_get, str?_?, msg, section_str_death);
            //if (msg[0]) {
            response = false;
            VARIANTwstr(&argList[0], victim.plS->Name);
            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, infected_message, 1, argList);
            //}
        }
        *showMessage = response;
    }
}

//RadWolfie - Approved
CNATIVE dllAPI e_color_team_index WINAPIC EXTOnPlayerJoinDefault(s_machine_slot* mS, e_color_team_index cur_team) {
    e_color_team_index dest_team = join_team;
    if (game_started) {
        if (pIHaloEngine->serverHeader->totalPlayers) {
            dest_team = human_team;
        } else if (cur_zombie_count > 0 && cur_human_count == 0) {
            dest_team = human_team;
        }
    }
    return dest_team;
}

//RadWolfie - Approved | except for name_table is not included atm.
CNATIVE dllAPI bool WINAPIC EXTOnPlayerChangeTeamAttempt(PlayerInfo plI, e_color_team_index dest_team, bool forceChange) {
    if (!forceChange) {
        if (!allow_change) {
            pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, blockteamchange_message, 0, nullptr);
        } else
            pIPlayer->m_change_team(&plI, dest_team, true);
    } else { // we can't stop the person changing teams, being done by an admin/the script
        /*// update team counts
        if (dest_team == zombie_team) {
            cur_human_count -= 1;
            cur_zombie_count += 1;
        } else {
            cur_human_count += 1;
            cur_zombie_count -= 1;
        }*/
        //TODO: RadWolfie - This is just a fix portion, might be temporary solution for now.
        // recalculate team counters
        cur_human_count = pIPlayer->m_get_str_to_player_list(human_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, nullptr, nullptr);
        cur_zombie_count = pIPlayer->m_get_str_to_player_list(zombie_team == COLOR_TEAM_BLUE ? teamBlueStr : teamRedStr, nullptr, nullptr);

        // they're allowed to change if the timer is active, if it is disable it
        if (allow_change && dest_team == zombie_team) {
            allow_change = false;
            // remove change timer
            for (std::vector<dTimer>::iterator iTimer = vTimer.begin(); iTimer != vTimer.end(); iTimer++) {
                if (iTimer->eFunc == ePlayerChangeTimer) {
                    pITimer->m_delete(EAOHashID, iTimer->TimerID);
                    vTimer.erase(iTimer);
                    hasPlayerChangeTimer = false;
                    break;
                }
            }
            pIPlayer->m_send_custom_message_broadcast(MF_BLANK, timer_team_change_msg, 0, nullptr);
        }

        // check if the game has started yet
        if (game_started) {
            // set attributes
            if (dest_team == zombie_team) {
                makezombie(plI, false);
            } else {
                makehuman(plI, false);
            }
            //registertimer(200, "checkgamestate", playerId)
            UINT id = pITimer->m_add(EAOHashID, &plI, 6); // 200 milliseconds - checkgamestate for victim
            if (id)
                vTimer.push_back({ id, eCheckGameState, plI.plR->MachineIndex });
        }

        // Update team
        /*TODO: Need to implement support for store team changed data if player decide to quit.
        local thisNAME = getname(playerId)
        name_table[thisNAME] = team
        */
    }
    return allow_change;
}

//RadWolfie - Approved
CNATIVE dllAPI bool EXTOnObjectDamageApplyProcess(const objDamageInfo* damageInfo, s_ident* obj_recv, objHitInfo* hitInfo, bool isBacktap, bool* allowDamage, bool isManaged) {

    // If this is set then we do NOT want this function to modify damage.
    if (dontModifyDmg)
        return true;

    PlayerInfo causer;
    PlayerInfo receiver;
    pIPlayer->m_get_ident(damageInfo->player_causer, &causer);
    s_biped* reciever_biped = (s_biped*)pIObject->m_get_address(*obj_recv);
    if (reciever_biped) {
        pIPlayer->m_get_ident(reciever_biped->PlayerOwner, &receiver);
    }

    if (isBacktap && causer.mS && receiver.mS) {
        if (causer.plR->Team == zombie_team) {
            if (receiver.plR->Team == human_team) {
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &causer, zombie_backtap_message, 0, nullptr);
            } else { //Don't allow zombies to backtap each other
                return false;
            }
        }
    } else if ((DWORD)hitInfo->desc == 'head') {

        //RadWolfie - 2 ^ 5 which is 7 decimal, aka 0.0-0.2 bitfield offset. However, bitfield 0.2 cause instant death.
        // Wizard wants to ignore shield. So, let's go with that instead.
        objDamageFlags flags = { 0 };
        flags.ignoreShield = 1;

        pIObject->m_apply_damage_generic(*obj_recv, damageInfo->player_causer, 5.0f, flags); // More damage for headshot.
    }
    return true;
}

//RadWolfie - Approved
CNATIVE dllAPI bool EXTOnObjectDamageLookupProcess(objDamageInfo* damageInfo, s_ident* obj_recv, bool* allowDamage, bool isManaged) {
    PlayerInfo causer;
    PlayerInfo receiver;

    // If this is set then we do NOT want this function to modify damage.
    if (dontModifyDmg)
        return true;
    if (damageInfo->tag_id.Tag == falling_tag_id->ident.Tag || damageInfo->tag_id.Tag == distance_tag_id->ident.Tag)
        return true;
    if (damageInfo->causer.Tag == -1)
        return true;

    pIPlayer->m_get_ident(damageInfo->player_causer, &causer);
    s_biped* reciever_biped = (s_biped*)pIObject->m_get_address(*obj_recv);
    if (reciever_biped) {
        pIPlayer->m_get_ident(reciever_biped->PlayerOwner, &receiver);
    }
    if (causer.mS && causer.plR->Team == zombie_team) {
        damageInfo->modifier = 9999;
    // Otherwise use human modifier
    } else if (causer.mS) {

        // else use normal modifier;
        float modifier = cur_last_human.mS ? lastman_dmgmodifier : human_dmgmodifier;

        // Check if melee/explosive/grenade damage
        hTagHeader* tagHeader = pIObject->m_lookup_tag(damageInfo->tag_id);
        PBYTE tagdata = (PBYTE)tagHeader->group_meta_tag;
        short dmg_damage_category = *(short*)(tagdata + 0x1C6);

        // I HATE the fuelrod.
        if (damageInfo->tag_id.Tag == fuel_dmg1_id->ident.Tag || damageInfo->tag_id.Tag == fuel_dmg2_id->ident.Tag || damageInfo->tag_id.Tag == fuel_dmg3_id->ident.Tag || damageInfo->tag_id.Tag == fuel_dmg4_id->ident.Tag) {
            struct s_bitfields {
                bool isAirborne : 1;
                char unknown : 7;
            };
            s_bitfields bitfields = *(s_bitfields*)((char*)reciever_biped + 0x4CC);

            modifier = bitfields.isAirborne ? 0.5f : 1.0f;
        } else {
            switch (dmg_damage_category) {
                default: 
                    break;
                case 3: // Check for explosive/grenade damage.
                case 4: // Check for explosive/grenade damage.
                    modifier = 9999.0f;
                    break;
                // Check if flame damage.
                case 7:

                    // We don't want them to be able to damage themself with the flamethrower.
                    if (obj_recv->Tag == damageInfo->causer.Tag)
                        return false;

                    // Triple the flamethrower damage.
                    modifier = 3.0f;
                    break;
                case 5:
                    // Triple sniper damage for humans.
                    modifier = 3.0f;
                    break;
            }
        }
        damageInfo->flags.ignoreShield = 1; // Ignore shields
        damageInfo->modifier = modifier; // Set human damage to zombies.
    }
    //RadWolfie - To prevent crash if no receiver or causer has been found.
    if (!(receiver.mS != nullptr && causer.mS != nullptr)) {

    } else if (receiver.mS != causer.mS && causer.plR->Team == human_team && receiver.plR->Team == human_team) {
        return false;
    } else if (causer.plR->Team == zombie_team && receiver.plR->Team == zombie_team) {
        damageInfo->modifier = 0.0001f;
    }


    return false;
}

//RadWolfie - Approved
CNATIVE dllAPI bool WINAPIC EXTOnVehicleUserEntry(PlayerInfo plI, bool forceEntry) {
    if (game_started && !forceEntry) {
        if ((plI.plR->Team == zombie_team && zombies_allowed_in_vehis) || (plI.plR->Team == human_team && humans_allowed_in_vehis)) {
            return true;
        }
    }
    return false;
}

//RadWolfie - Approved
CNATIVE dllAPI void WINAPIC EXTOnPlayerUpdate(PlayerInfo plI) {

    if (!(plI.plS && plI.plS->CurrentBiped.Tag != -1))
        return;

    s_biped* pl_biped = (s_biped*)pIObject->m_get_address(plI.plS->CurrentBiped);
    if (!pl_biped)
        return;
    if (pl_biped->Flashlight && !usedFlashlight[plI.plR->MachineIndex]) {
        usedFlashlight[plI.plR->MachineIndex] = true;
        UINT id = pITimer->m_add(EAOHashID, &plI, 90); // 3000 seconds
        if (id)
            vTimer.push_back({ id, eHaveSpeedTimer, plI.plR->MachineIndex });
        plI.plS->VelocityMultiplier = flashlight_speed;
        //CALL_MEMBER_FN(customMsg, m_value_get, str?_?, msg, section_str_misc);
        pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, speedburst_begin_message, 0, nullptr);
    }
    real_vector3d curCoord = pl_biped->sObject.World;
    char* map_name = pIHaloEngine->mapCurrent->name;

    // this makes sure that people won't camp in unreachable places.
    if (plI.plR->Team == human_team)
        if (_stricmp(map_name, "bloodgulch") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(39.14f, -96.18f, 2.12f), 4)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_tree_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(92.73f, -96.74f, 9.21f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "damnation") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-2.11f, 12.05f, 7.82f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_glitch_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(5.16f, 15.42f, 8.06f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_glitch_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-1.09f, 13.63f, 7.82f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_glitch_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-7.25f, 12.93f, 5.60f), 1.5)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "icefields") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(18.47f, -6.61f, 6.55f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(17.17f, -6.64f, 6.61f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(32.68f, -33.29f, 5.43f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-17.13f, 19.32f, 11.58f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-72.06f, 72.52f, 3.92f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-85.53f, 97.83f, 5.27f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "sidewinder") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-6.47f, -33.76f, 4.41f), 5)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_glitch_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(4.77f, -33.43f, 4.41f), 5)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_glitch_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "deathisland") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-26.3f, -7.72f, 11.7f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-25.98f, -6.25f, 11.7f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(29.73f, 15.29f, 10.47f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(29.57f, -17.1f, 10.99f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "putput") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-4.59f, -20.67f, 3.3f), 1.5)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-2.78f, -20.84f, 3.3f), 1.5)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        } else if (_stricmp(map_name, "chillout") == 0) {
            if (check_in_sphere(pl_biped->sObject.World, real_vector3d(11.26f, 8.82f, 3.16f), 2)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            } else if (check_in_sphere(pl_biped->sObject.World, real_vector3d(-7.16f, 7.8f, 4.34f), 3)) {
                pIObject->m_kill(plI.plS->CurrentBiped);
                pIPlayer->m_send_custom_message(MF_BLANK, MP_RCON, &plI, block_spot_message, 0, nullptr);
            }
        }
}

//RadWolfie - Extended
CNATIVE dllAPI void WINAPIC EXTOnMapReset() {
    map_reset_boolean = true;
}

//RadWolfie - Extended
CNATIVE dllAPI bool WINAPIC EXTOnPlayerSpawnColor(PlayerInfo plI, bool isTeamPlay) {
    if (plI.plR->Team == zombie_team) {
        plI.plR->ColorIndex = zombie_color;
        plI.plS->ColorIndex = zombie_color;
    } else {
        plI.plR->ColorIndex = human_color;
        plI.plS->ColorIndex = human_color;
    }
    return 0;
}
#pragma endregion

#include "..\Add-on API\C\expChecker.h"
