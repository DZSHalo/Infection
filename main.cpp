#include <windows.h>
#include <stdio.h>
#define EXT_IPLAYER
#define EXT_IOBJECT
#define EXT_IHALOENGINE
#define EXT_ITIMER
#define EXT_ICOMMAND
#define EXT_ICINIFILE
#pragma comment(lib, "../Add-on API/Add-on API.lib")
#include "../Add-on API/Add-on API.h"

#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))


#pragma region //Wizard's Code
#include <string>
#include <vector>
#include <math.h>    //Dumb Microsoft for leaving out round in many versions...
#include <time.h>   //Is needed for srand function.
double round(double num) {
    return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
}
#define randomRange(min, max) (min + (rand() % (int)(max - min + 1)))
//#define randomRange(min, max) (int( rand() / (RAND_MAX+1)) * (max-min+1) + min)   //BAD

// Global Variable
//std::wstring default_script_prefix = L"";
char human_team = 0; // 0 is red, 1 is blue
int max_zombie_count = 2; // this caps what the zombie count would be w/ ratio (nil is disable)
DWORD time_invis = 1; // In seconds, how long the zombie/human should be invis when they crouch.
int zombie_count = 1; // if value is less than 1 is it used as a percentage, more than or equal to one is absolute count
char zombie_team = 1; // 0 is red, 1 is blue
char join_team = zombie_team; // the team that people will join if a game is currently running

    //Alpha Zombie Variables
int alphazombie_frag_count = 0; // number of frag nades they spawn with
int alphazombie_plasma_count = 0; // number of plasma nades they spawn with
int alphazombie_clip_count = 0; // number of shots in clip (loaded ammo)
int alphazombie_ammo_count = 0; // backpack ammo they get (unloaded ammo)
float alphazombie_battery_count = 0.0f; // stored as a percent (0 to 1, do NOT go over or under)

//Zombie Variables
int zombie_ammo_count = 0; // backpack ammo zombies once there are not only alpha zombies (unloaded ammo)
int zombie_clip_count = 0; // number of shots in clip for zombies once there are not only alpha zombies (loaded ammo)
float zombie_battery_count = 0; // stored as a percent (0 to 1, do NOT go over or under)
int zombie_frag_count = 0; // number of frag nades they spawn with
int zombie_plasma_count = 0; // number of plasma nades they spawn with
int zombie_spawn_time = 1; // spawn time for zombies in seconds. Leave "default" for default spawn time of gametype
float zombie_speed = 1.5f; // zombie speed

//Zombie weapons
// Note: If you decide the player holds a flag or a ball, make sure the secondary, tertiary, and quarternary fields are "".
// DO NOT make zombies hold multiple weapons if you want them to hold an oddball or a flag. If you do it will not work right, and it's entirely your fault.
/*
DISABLED AS THIS ISN'T REALLY NEEDED
zombie_weapon = {} // don't touch this
zombie_weapon[1] = "weapons\\ball\\ball" // Primary weapon for zombies
zombie_weapon[2] = "weapons\\ball\\ball" // Secondary weapon for zombies.
zombie_weapon[3] = "" // Tertiary weapon for zombies.
zombie_weapon[4] = "" // Quarternary weapon for zombies.*/

    // Human Variables
float human_dmgmodifier = 1.55f; // damage modifier for humans.
float human_speed = 1.0f; // speed when not infected
int human_spawn_time = 2; // spawn time for humans in seconds. Leave "default" for default spawn time of gametype

    // Last Man Variables
float lastman_dmgmodifier = 1.75f; // damage modifier for the last man
int lastman_invistime = 20; // in seconds
int lastman_invulnerable = 10; // time (in seconds) the last man is invulnerable for: replace with nil to disable
float lastman_speed = 1.75f; // last man speed

    // Booleans
bool humans_allowed_in_vehis = false; // if this is set to false { humans cannot enter vehicles
bool humans_invisible_on_crouch = false; // if this is set to true { humans will become invisible when they crouch.
bool infect_on_fall = true; // if this is set to true { people who die from fall damage will become zombies.
bool infect_on_guardians = false; // if this is set to true { people who get killed by the guardians will become zombies.
bool infect_on_suicide = true; // if this is set to true { people who kill themselves will become a zombie.
bool infect_on_betrayal = false; // if this is set to true { people who betray their teammates will become a zombie.
bool last_man_next_zombie = true; // if this value is true the last man standing becomes the next zombie, if not it's random
bool zombies_allowed_in_vehis = false; // if this is set to false { zombies cannot enter vehicles.
bool zombies_invisible_on_crouch = true; // if this is set to true { zombies will be invisible when they crouch.

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


static const wchar_t section_str_deaths[] = L"deaths";
static const wchar_t section_str_infected[] = L"infected";
static const wchar_t section_str_misc[] = L"misc";
static const wchar_t section_str_command[] = L"command";
static const wchar_t section_str_welcome[] = L"welcome";
static const wchar_t section_str_general[] = L"general";
/*
// Death / Infect messages
std::wstring blockteamchange_message = L"Autobalance: You're not allowed to change team.";
std::wstring falling_infected_message = L"%s fell to his death...";
std::wstring falling_death_message = L"%s slipped and fell...";
std::wstring kill_infected_message = L"%s has infected %s";
std::wstring suicide_infected_message = L"%s lost the will to live...";
std::wstring suicide_message = L"%s made mistakes...";
std::wstring teammate_infected_message = L"%s was infected for betraying %s";
std::wstring guardian_infected_message = L"%s was infected by an angry ghost!";
std::wstring guardian_death_message = L"";
std::wstring in_hill_too_long_human_msg = L"%s was infected because they were in the hill too long!";
std::wstring in_hill_too_long_zombie_msg = L"%s has been killed because they were in the hill too long!";
std::wstring human_kill_message = L"%s has killed %s";

//Hint Messages
std::wstring teamkill_message = L"Don't team kill...";
std::wstring nozombiesleftmessage = L"There are no zombies left. Someone needs to change team or be forced to.";
std::wstring lastman_message = L"%s is the last human alive and is invisible for %d seconds!";
std::wstring rejoin_message = L"Please don't leave and rejoin. You've been put back onto your last team.";
std::wstring zombieinvis_message = L"The zombies are invisible for 30 seconds!";

// Complement Messages
std::wstring timer_team_change_msg = L"Thank you. The game will now continue";
std::wstring zombie_backtap_message = L"Nice backtap!";

// New Team Messages
std::wstring human_message = L"YOU'RE A HUMAN. Survive!";
std::wstring zombie_message = L"YOU'RE A ZOMBIE. FEED ON HUMANS!";

// Additional Messages
std::wstring welcome_message = L"Welcome to Ash Clan Zombies";
std::wstring koth_additional_welcome_msg = L"The hill is a safezone! Use it for quick getaways!";
//slayer_additional_welcome_msg = "The nav points are not just for decoration!\nThey will point to the last man surviving!";

// Block Messages
std::wstring blocked_tree_message = L"This tree is blocked.";
std::wstring blocked_spot_message = L"Sorry this spot has been blocked...";
std::wstring blocked_glitch_message = L"Glitching is not allowed!";

// KOTH Messages
std::wstring koth_in_hill_human_message = L"%s must leave the hill in 10 seconds or they will be infected!";
std::wstring koth_in_hill_zombie_message = L"%s must leave the hill in 10 seconds or they will be killed!";
std::wstring koth_in_hill_human_warn_message = L"You have %d seconds to leave the hill!";
*/

// Don't modify below variables unless you know what you're doing
//int cur_zombie_count = 0; //TODO: Need to remove extra cur_zombie_count
int cur_zombie_count = 0;
int cur_human_count = 0;
int alpha_zombie_count = 0;
//int human_time = {};
//int cur_players = 0; //Don't use this, intead use pIHaloEngine->globalServer->totalPlayers
IPlayer::PlayerInfo cur_last_man;
//int last_man_name = 0;
int processid = 0;
BYTE game_init = 0;
bool game_enable = 0;
bool allow_change = false;
bool map_reset_boolean = false;
//int flagball_weap = {};
//int last_hill_time = {};    //Not even in used...
//int name_table = {};
//int inhill_time = {};
//int fuelrods = {};
//int activateFlashlight = {};
//int zombie_kills[16] = {0};

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

#define invisCrouch             0
#define HumanTimer              1
#define NewGameTimer            2
#define MsgTimer                3
#define SpeedTimer              4
#define PlayerChangeTimer       5
#define RemoveLastmanProtection 6
#define TOTALTIMERS 7

int playerChangeCounter=0;
int timers[TOTALTIMERS];
IPlayer::PlayerInfo pl_null;
ident oddball_flag_obj[16];
vect3 location;
vect3 velocity_reset;
IPlayer::PlayerInfo plI_ctf;
WeaponS* oddball_flag_relocate=NULL;

//Missing values
int resptime = 0;
IObject::hTagHeader *camouflage_tag_id,
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
        *flame_ammo_id;
#pragma endregion

#pragma region //RadWolfie's code port to H-Ext plugin.
void OnFFATeamChange(IPlayer::PlayerInfo plI, char dest_team, int updatecounters);
void LoadTags() {
    //TODO: Need support for ball/flag to obtain tag id. And for the rest of the tag ids... Is this really necessary?
    /*
    for (int i = 0; i<4; i++) {
        if (zombie_weapon[i] == "weapons\\ball\\ball" || zombie_weapon[i] == "weapons\\flag\\flag") {
            oddball_or_flag = gettagid("weap", zombie_weapon[i])
        }
    }*/
    
    camouflage_tag_id = pIObject->LookupTagTypeName("eqip", "powerups\\active camouflage");
    healthpack_tag_id = pIObject->LookupTagTypeName("eqip", "powerups\\health pack");
    overshield_tag_id = pIObject->LookupTagTypeName("eqip", "powerups\\over shield");
    fragnade_tag_id = pIObject->LookupTagTypeName("eqip", "weapons\\frag grenade\\frag grenade");
    plasmanade_tag_id = pIObject->LookupTagTypeName("eqip", "weapons\\plasma grenade\\plasma grenade");
    fragnade_tag_id = pIObject->LookupTagTypeName("eqip", "weapons\\frag grenade\\frag grenade");
    plasmanade_tag_id = pIObject->LookupTagTypeName("eqip", "weapons\\plasma grenade\\plasma grenade");
    oddball_tag_id = pIObject->LookupTagTypeName("weap", "weapons\\ball\\ball");
    flag_tag_id = pIObject->LookupTagTypeName("weap", "weapons\\flag\\flag");
    rifle_id = pIObject->LookupTagTypeName("weap", "weapons\\assault rifle\\assault rifle");
    needler_id = pIObject->LookupTagTypeName("weap", "weapons\\needler\\mp_needler");
    pistol_id = pIObject->LookupTagTypeName("weap", "weapons\\pistol\\pistol");
    rocket_id = pIObject->LookupTagTypeName("weap", "weapons\\rocket launcher\\rocket launcher");
    shotgun_id = pIObject->LookupTagTypeName("weap", "weapons\\shotgun\\shotgun");
    sniper_id = pIObject->LookupTagTypeName("weap", "weapons\\sniper rifle\\sniper rifle");
    flame_id = pIObject->LookupTagTypeName("weap", "weapons\\flamethrower\\flamethrower");
    rifle_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\assault rifle ammo\\assault rifle ammo");
    needler_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\needler ammo\\needler ammo");
    pistol_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\pistol ammo\\pistol ammo");
    rocket_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\rocket launcher ammo\\rocket launcher ammo");
    shotgun_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\shotgun ammo\\shotgun ammo");
    sniper_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\sniper rifle ammo\\sniper rifle ammo");
    flame_ammo_id = pIObject->LookupTagTypeName("eqip", "powerups\\flamethrower ammo\\flamethrower ammo");
    
}
/*int randomRange(int min, int max) {
    return min + (rand() % (int)(max - min + 1));   //the ONLY effective method, however the first number is always the same... the rest are random, lucky...
    //return (int(double(rand()) / double(RAND_MAX+1)) * (max - min+1)) + min;    //No good, always zero...
}*/
bool object_in_sphere(vect3 objectLoc, vect3 loc, float R) {
    bool Pass = false;
    if (pow(loc.x - objectLoc.x, 2) + pow(loc.y - objectLoc.y, 2) + pow(loc.z - objectLoc.z, 2) <= R) {
        Pass = true;
    }
    return Pass;
}
void all_players_zombies(IPlayer::PlayerInfo plI) {
    // if there is a last man, store their ip
    if (plI.mS) {
        //last_man_name = getname(player);
        // write the ip to file
    }
    // move onto the next map
    pIHaloEngine->Exec("sv_map_next");
}
void takenavsaway() {
    util::dynamicStack<IPlayer::PlayerInfo> plList;
    pIPlayer->StrToPlayerList(L"*", plList, NULL);
    for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
        plI->plS->killInOrderObjective=plI->plS->PlayerIndex;
    }
}
void makehuman(IPlayer::PlayerInfo plI, bool forcekill, int updatecounters=0) {
    //TODO: Is this even neccessary?
    /*if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_SLAYER) {
        pIPlayer->ChangeTeam(plI, human_team, forcekill);
        //writebyte(getplayer(player) + 0x20, human_team)
    }*/
    // change the player's speed
    plI.plS->VelocityMultiplier=human_speed;
    
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (cachePlayer->uniqueID == plI.mS->UniqueID) {
            if (plI.plS->Team==zombie_team) {
                cachePlayer->isZombie=false;
                pIPlayer->changeTeam(plI, human_team, forcekill);
                if (!pIHaloEngine->gameTypeLive->isTeamPlay) {
                    OnFFATeamChange(plI, human_team, updatecounters);
                }
            }
            break;
        }
    }
}
void makezombie(IPlayer::PlayerInfo plI, bool forcekill, int updatecounters=0) {
    //TODO: Is this even neccessary?
    /*if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_SLAYER) {
        plI.plS->Team=zombie_team;
        //writebyte(getplayer(player) + 0x20, zombie_team)
    }*/
    // change the player's speed
    plI.plS->VelocityMultiplier=zombie_speed;
    
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (cachePlayer->uniqueID == plI.mS->UniqueID) {
            if (plI.plS->Team!=zombie_team) {
                cachePlayer->isZombie=true;
                pIPlayer->changeTeam(plI, zombie_team, forcekill);
                if (!pIHaloEngine->gameTypeLive->isTeamPlay) {
                    OnFFATeamChange(plI, zombie_team, updatecounters);
                }
            }
            break;
        }
    }
}
IPlayer::PlayerInfo ChooseRandomPlayer(char excludeTeam) {
    // loop through all 16 possible spots and add to table
    bool isChecked[16] = {0};
    char randNumber;
    IPlayer::PlayerInfo chosen;
    
    int count=0;
    util::dynamicStack<IPlayer::PlayerInfo> plList;
    pIPlayer->StrToPlayerList(L"*", plList, NULL);
    for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
        count++;
    }
    count--;
    util::dynamicStack<IPlayer::PlayerInfo>::iterator plI_begin = plList.begin();
    util::dynamicStack<IPlayer::PlayerInfo>::iterator plI_iter;
    while(1) {
        randNumber = randomRange(0, count);
        //randNumber = rand()%count;
        pIPlayer->sendCustomMsgBroadcast(MSG_INFO, L"Test random number: %d - count:%d", randNumber, count);
        if (!isChecked[randNumber]) {
            plI_iter=plI_begin+randNumber;
            if (plI_iter->plS && plI_iter->plS->Team!=excludeTeam)
                return *plI_iter;
            else
                isChecked[randNumber] = 1;
        }
        randNumber=0;
        for (char i=0; i<=count; i++) {
            if (isChecked[i])
                randNumber+=1;
        }
        if (randNumber==count)
            break;
    }
    chosen = IPlayer::PlayerInfo();
    return chosen;
}
void noZombiesLeft() {
    wchar_t msg[256];
    if (pIHaloEngine->gameTypeLive->isTeamPlay) {
        if (timers[PlayerChangeTimer]==-1) {
            allow_change = true;
            customMsg->ValueGet(str1_2, msg, section_str_misc);
            pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg);//nozombiesleftmessage.c_str());
            timers[PlayerChangeTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 300);    //10 seconds
            //player_change_timer = registertimer(1000, "PlayerChangeTimer")
        }
    } else {
        // pick a human and make them zombie.
        IPlayer::PlayerInfo newZomb = ChooseRandomPlayer(zombie_team);
        if (newZomb.plS) {
            makezombie(newZomb, true);
            customMsg->ValueGet(str1_8, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, newZomb, msg /*zombie_message.c_str()*/);
        }
    }
}
void onlastman() {
    // lookup the last man
    IPlayer::PlayerInfo lastManCheck;
    for(char x=0; x<16; x++) {
        lastManCheck=pIPlayer->getPlayerMindex(x);
        if (lastManCheck.plS) {
            if (lastManCheck.plA->Team!=zombie_team) {
                cur_last_man = lastManCheck;
                //if (pIHaloEngine->gameTypeLive->GameStage==GAMETYPE_SLAYER)
                    //WriteNavsToZombies(); //TODO: Unable to do this since there's no way to set the nav base on Wiz's offset... Need more research...
                //if gametype == "Slayer" then WriteNavsToZombies(x) end
                // give the last man speed and extra ammo
                lastManCheck.plS->VelocityMultiplier=lastman_speed;
                // find the last man's weapons
                BipedS* pl_biped = (BipedS*)pIObject->GetObjectAddress(lastManCheck.plS->CurrentBiped);
                if (!pl_biped)
                    continue;
                for (char give=0; give<4; give++) {
                    WeaponS* pl_weapon = (WeaponS*)pIObject->GetObjectAddress(pl_biped->Equipments[give]);
                    if (!pl_weapon)
                        continue;
                    pl_weapon->BulletCountInRemainingClips = 132;
                    pl_weapon->BulletCountInCurrentClip = 100;
                    pIObject->Update(pl_biped->Equipments[give]);

                }
                break;
            }
        }
    }
    if (cur_last_man.mS) {
        wchar_t msg[256];
        customMsg->ValueGet(str1_3, msg, section_str_misc);
        pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*lastman_message.c_str()*/, cur_last_man.plS->Name, lastman_invistime);
        pIPlayer->applyCamo(cur_last_man, lastman_invistime);
    }
}
void checkgamestate(IPlayer::PlayerInfo player) {
    // check if the game has started yet
    if (game_init > 1) {
        //local human_count, zombie_count = getteamsizes()
        // if no humans, but there are zombies, end the game
        if (cur_human_count == 0 && cur_zombie_count > 0) {
            all_players_zombies(player);
        } else if (cur_human_count > 1 && cur_zombie_count == 0) {
            noZombiesLeft();
        } else if (cur_human_count == 1 && cur_zombie_count > 0 && !cur_last_man.mS) {
            onlastman();
        } else if (cur_last_man.mS && zombie_count == 0) {
            if (cur_last_man.mS) {
                makehuman(cur_last_man, false);
            }
            cur_last_man = IPlayer::PlayerInfo();
        } else if (cur_last_man.mS && cur_human_count > 1) {
            if (pIHaloEngine->gameTypeLive->GameStage==GAMETYPE_SLAYER)
                takenavsaway();
            if (cur_last_man.mS)
                makehuman(cur_last_man, false);
            cur_last_man = IPlayer::PlayerInfo();
        }
    }
    //return false;
}
void OnFFATeamChange(IPlayer::PlayerInfo plI, char dest_team, int updatecounters) {
    // update team counts
    if (!updatecounters) {
        if (dest_team == zombie_team) {
            cur_human_count -= 1;
            cur_zombie_count += 1;
        } else {
            cur_human_count += 1;
            cur_zombie_count -= 1;
        }
    }
    // check if the game has started yet
    if (game_init > 0) {
        checkgamestate(plI);
        //registertimer(200,"checkgamestate",player)
    }
    // update team with
    /*local thisNAME = getname(player)
    name_table[thisNAME] = team*/
}
bool validatePlayers(IPlayer::PlayerInfo plI) {
    bool isNewPlayer = true;
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (_stricmp(cachePlayer->hash.c_str(), plI.plEx->CDHashA)==0) {
            if (game_init > 1) {
                if (cachePlayer->isZombie)
                    makezombie(plI, false);
                else
                    makehuman(plI, false);
                wchar_t msg[256];
                customMsg->ValueGet(str1_4, msg, section_str_misc);
                pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//rejoin_message.c_str());
            }
            cachePlayer->uniqueID = plI.mS->UniqueID;
            isNewPlayer = false;
            break;
        }
    }
    if (isNewPlayer) {
        dataTable newPlayer;
        newPlayer.hash = plI.plEx->CDHashA;
        newPlayer.uniqueID = plI.mS->UniqueID;
        if (game_init > 1)
            newPlayer.isZombie = plI.plA->Team==zombie_team;
        cacheTable.push_back(newPlayer);
    }
    return isNewPlayer;
}
bool PlayerInHill(IPlayer::PlayerInfo plI) {
    bool retBool = false;
    if (plI.plS && pIHaloEngine->gameTypeGlobals->kothGlobal.isInHill[plI.plS->PlayerIndex])
        retBool = true;
    return retBool;
}
int getalphacount() {
    // recalculate how many "alpha" zombies there are
    //int alpha_zombie_count;
    if (zombie_count < 1)
        alpha_zombie_count = (int)round((pIHaloEngine->globalServer->totalPlayers * zombie_count) + 0.5);
    else
        alpha_zombie_count = zombie_count;
    
    if (alpha_zombie_count > max_zombie_count)
        alpha_zombie_count = max_zombie_count;
    
    return alpha_zombie_count;
}
void Init() {
    if (pIHaloEngine->mapCurrent->cam_multi_menu==1) {//This is just in case someone might initialize this while not hosting it.
        LoadTags();
        //reset our variables
        cur_zombie_count = 0;
        cur_human_count = 0;
        resptime = 5;
        cacheTable.clear();
        char* map = pIHaloEngine->mapCurrent->mapName;
        if (_stricmp(map, "putput")==0 || _stricmp(map, "longest")==0 || _stricmp(map, "beavercreek")==0 || _stricmp(map, "carousel")==0 || _stricmp(map, "wizard")==0) {
            zombie_speed = 1.5f;
            lastman_speed = 1.25f;
        } else {
            zombie_speed = 1.5f;
            lastman_speed = 1.75f;
        }
        util::dynamicStack<IPlayer::PlayerInfo> plList;
        pIPlayer->StrToPlayerList(L"*", plList, NULL);
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            validatePlayers(*plI);
        }
        timers[NewGameTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 225); //7.5 seconds
    }
}
void Uninit() {
    if (game_init) {
        game_init=1;
        char i = 0;
        do {
            ident out_objId;
            pIObject->Delete(oddball_flag_obj[i]);
            i++;
        } while(i<16);
        pIHaloEngine->Exec("sv_map_reset");
        game_init=0;
    }
}

CNATIVE dllAPI void EXTOnMapLoad(ident mapTag, const wchar_t map[32]) {
    if (pIHaloEngine->mapCurrent->cam_multi_menu==1 && game_enable) {
        Init();
    }
}
CNATIVE dllAPI void WINAPIC EXTOnEndGame(int stage) {
    if (game_init) {
        game_init = 0;
    }
    if (stage == 1) {//TODO: Need to make timer for Wizard's method. //Wait... is this done?
        for(char i =0; i<TOTALTIMERS; i++) {
            if (timers[i]!=-1)
                addon::pITimer->EXTAddOnTimerDelete(timers[i]);
        }
    }
}
CNATIVE dllAPI bool WINAPIC EXTOnPlayerSpawnColor(IPlayer::PlayerInfo plI, bool isTeamPlay) {
    if (!game_init)
        return 1;    //1 is use default color
    
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (cachePlayer->uniqueID == plI.mS->UniqueID) {
            if (cachePlayer->isZombie && plI.plS->ColorIndex!=COLOR_GRAY) {
                plI.plA->ColorIndex=plI.plS->ColorIndex=COLOR_GRAY;
            } else if (cachePlayer->isZombie)
                break;
            else if (isTeamPlay && !cachePlayer->isZombie && plI.plS->ColorIndex!=COLOR_SAGE)
                plI.plA->ColorIndex=plI.plS->ColorIndex=COLOR_SAGE;
            else if (!isTeamPlay && !cachePlayer->isZombie && game_init==1) {
                bool reservedColors[18] = { 0 };
                reservedColors[COLOR_GRAY]=true;
                util::dynamicStack<IPlayer::PlayerInfo> plList;
                pIPlayer->StrToPlayerList(L"*", plList, NULL);
                for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plIp = plList.begin(); plIp != NULL; plIp++) {
                    reservedColors[plIp->plS->ColorIndex]=1;
                }
                char i=0;
                while(reservedColors[i]) {
                    if (i>17)
                        RaiseException(STATUS_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
                    i++;
                }
                plI.plA->ColorIndex=plI.plS->ColorIndex=i;
            }
            break;
        }
    }
    return !isTeamPlay;    //0 is use custom color even in TeamPlay mode.
}
//This is moved & modified section.
/* DON'T USE THIS!
CNATIVE dllAPI int WINAPIC EXTOnPlayerJoinDefault(MachineS* mS, int cur_team) {
    if (!pIHaloEngine->gameTypeLive->isTeamPlay) {
        // initialize the destination team as the join team
        int dest_team = join_team;
        // make sure the game is started
        if (game_init) {
            // if no zombies make them human
            if (cur_human_count == 0) {
                dest_team = human_team;
            }
        }
        // we need to overwrite the 'team' variable being passed to onplayerjoin
        return dest_team;
    }
    return -1;
}//*/
CNATIVE dllAPI void WINAPIC EXTOnPlayerJoin(IPlayer::PlayerInfo plI) {
    // update the player counts
    //pIHaloEngine->globalServer->totalPlayers += 1;
    //TODO: Don't think need this since it's called from player join default team function.
    // onteamdecision isn't called for ffa gametypes
    /*if (!team_play) {
        // initialize the destination team as the join team
        bool dest_team = join_team;
        // make sure the game is started
        if (game_init) {
            // if no zombies make them human
            if (cur_human_count == 0) {
                dest_team = human_team;
            }
        }
        // we need to overwrite the 'team' variable being passed to onplayerjoin
        team = dest_team;
    }*/
    int thisTeamSize = 0; // used so we don't create empty teams for rejoining players
    if (plI.plA->Team == zombie_team) {
        cur_zombie_count += 1;
        thisTeamSize = cur_zombie_count;
    } else {
        cur_human_count = 1;
        thisTeamSize = cur_human_count;
    }
    alpha_zombie_count = getalphacount();
    //std::wstring thisNAME = plI.plS->Name;
    // check if the player has joined this game previously
    bool isNewPlayer = validatePlayers(plI);
    // make sure the game is started
    if (game_init) {
        // check if the player is a zombie
        wchar_t msg[256];
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                if (cachePlayer->isZombie) {
                    //we don't need to update the counters since they're already on the zombieteam
                    if (!isNewPlayer)
                        makezombie(plI, false, true);
                    //send them the zombie message
                        customMsg->ValueGet(str1_8, msg, section_str_misc);
                    pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//zombie_message.c_str());
                } else {
                    // if we're at last man, make this player a zombie
                    if (cur_last_man.mS) {
                        //make this person a zombie (they're currently a human)
                        if (!isNewPlayer)
                            makezombie(plI, true);
                        //send them the zombie message
                        customMsg->ValueGet(str1_8, msg, section_str_misc);
                        pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//zombie_message.c_str());
                    } else {
                        //make this person a human (they're currently a zombie)
                        if (!isNewPlayer)
                            makehuman(plI, false, true);
                        //send them the human message
                        customMsg->ValueGet(str1_7, msg, section_str_misc);
                        pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//human_message.c_str());
                    }
                }
                //send the player the welcome message
                customMsg->ValueGet(str1_0, msg, section_str_welcome);
                pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//welcome_message.c_str());
                if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_KOTH) {
                        customMsg->ValueGet(str2_0, msg, section_str_welcome);
                        pIPlayer->sendCustomMsg(MSG_BLANK, 0, plI, msg);//koth_additional_welcome_msg.c_str());
                } else if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_SLAYER /*/&& gametype_indicator == 1/*/) {
                    //Nothing here
                }
                break;
            }
        }
        checkgamestate(pl_null);
        //registertimer(200,"checkgamestate",-1);
    } else {
        //registertimer(10000, "MsgTimer", player);
    }
}
CNATIVE dllAPI void WINAPIC EXTOnPlayerQuit(IPlayer::PlayerInfo plI) {
    if (game_init) {
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                //check if they're a zombie
                if (cachePlayer->isZombie) {
                    //take one away from the current zombie count
                    cur_zombie_count -= 1;
                } else {
                    //take one away from the current human count
                    cur_human_count -= 1;
                }
                BipedS* pl_biped = (BipedS*)pIObject->GetObjectAddress(plI.plS->PreviousBiped);
                if (pl_biped) {
                    char i = 0;
                    while (i<4) {
                        if(pl_biped->Equipments[i].Tag!=-1) {
                            WeaponS* weap = (WeaponS*)pIObject->GetObjectAddress(pl_biped->Equipments[i]);
                            if (weap->sObject.GameObject==-1) {
                                if (weap->sObject.ModelTag.Tag != oddball_tag_id->id.Tag)
                                    pIObject->Delete(pl_biped->Equipments[i]);

                            }
                        }
                        i++;
                    }
                    pIObject->Delete(plI.plS->CurrentBiped);
                }
                break;
            }
        }
    }
    //if last man is leaving, reset it
    if (cur_last_man.mS == plI.mS) {
        cur_last_man = IPlayer::PlayerInfo();
    }
    //check the current game state (the player that left might have been the last man or the only zombie)
    checkgamestate(pl_null);
    //registertimer(1000,"checkgamestate",-1);

}

CNATIVE dllAPI void WINAPIC EXTOnPlayerSpawn(IPlayer::PlayerInfo plI, ident owningObjectId, BipedS* pl_Biped) {
    if (!game_init)
        return;
    //activateFlashlight[player] = nil
    //check if the player is a zombie
    if (plI.plA->Team == zombie_team) {

        //get the player's object ID
        //make sure the player is alive (off chance that there's a glitch in phasor)
        if (plI.plS->CurrentBiped.Tag==-1) {
            return;
        }

        //get the player's object struct
        //local m_object = getobject(m_objectId)
        BipedS* m_object = (BipedS*)pIObject->GetObjectAddress(plI.plS->CurrentBiped);

        //set nade counts
        m_object->grenade0 = zombie_frag_count;
        m_object->grenade1 = zombie_plasma_count;

        //set the ammo
        int clipcount = alphazombie_clip_count;
        int ammocount = alphazombie_ammo_count;
        float batterycount = alphazombie_battery_count;

        //set ammo counts for zombies when others have been infected
        if (cur_zombie_count > alpha_zombie_count) {
            clipcount = zombie_clip_count;
            ammocount = zombie_ammo_count;
            batterycount = zombie_battery_count;
        } else { //set alpha nades
            m_object->grenade0 = alphazombie_frag_count;
            m_object->grenade1 = alphazombie_plasma_count;
        }
        
        //check if the starting equipment is generic
        //Quite honestly... this isn't needed...
        /*if starting_equipment == "Generic" {

            //initialize this boolean as false
            local bool = false
            for i=0,3 do --loop through the player's weapons (primary through quartenary)

                 //get the player's weapon's ID
                    local m_weaponId = readdword(m_object + 0x2F8 + i*4)

                 //check if the zombie weapon is an oddball or a flag
                    if oddball_or_flag {

                     //make sure the boolean is still false and the player's weapon exists.
                        if bool == false and getobject(m_weaponId) {

                         //get the weapon's tagname
                            local tagName = getobjecttag(m_weaponId)

                         //make sure the weapon's tagname is the zombie weapon (oddball/flag)
                            if tagName == oddball_or_flag {

                             //store the weapon's ID in the flagball_weap table
                                flagball_weap[player] = m_weaponId

                             //set bool to true
                                bool = true
                            }
                        }

                 //zombie weapon is not a flag or a ball
                    } else if ( m_weaponId ~= 0xFFFFFFFF {

                     //get the weapon's struct
                        local m_weapon = getobject(m_weaponId)

                     //make sure the weapon exists
                        if m_weapon {

                            //set the ammo
                            writeword(m_weapon + 0x2B6, ammocount)
                            writeword(m_weapon + 0x2B8, clipcount)

                            //set the battery (math.abs is absolute value meaning it takes the opposite of battery count (meaning 0 needs to be 1 and 1 needs to be 0))
                            writefloat(m_weapon + 0x240, math.abs(batterycount - 1))

                            //force it to sync
                            updateammo(m_weaponId)
                        }
                    }
                }

            //check if we still need to assign leftover weapons to zombies (tertiary and quartenary weapons)
            if (zombie_weapon[3] and zombie_weapon[3] ~= "") { //make sure the script user isn't retarded
             //assign the leftover weapons to the player
                local table = {player, clipcount, ammocount, batterycount}
                registertimer(0, "AssignLeftoverZombieWeapons", table)
            }
        else if starting_equipment == "Custom" {
         //assign the correct weapons to the player
            table = {player, clipcount, ammocount, batterycount}
            registertimer(0, "AssignZombieWeapons", table)
        }*/
    }

 //check if the gametype is slayer
    if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_SLAYER) {
     //check if we're at the last_man

        //TODO: Need find out which structure is setting the nat point. (WEIRD, okay it does not seems to work...)
        if (cur_last_man.mS) { //write the navpoint to the current last man
            //writeword(m_player + 0x88, cur_last_man)
        } else {
            //writeword(m_player + 0x88, player) //there is no last man so put a nav above this player's head
        }

    }

}

/*
//basically this timer assigns the tertiary and quarternary weapons to zombies if specified at the top
//this is needed since onweaponassignment isn't called for tertiary and quartenary weapons
function AssignLeftoverZombieWeapons(id, count, player, data)
    local clipcount = data[1]
    local ammocount = data[2]
    local batterycount = data[3]
    local m_object = getplayerobject(player)
    if m_object {
        if zombie_weapon[3] and zombie_weapon[3] ~= "" {
            local m_weaponId = createobject(gettagid("weap", zombie_weapon[3]), 0, 60, false, 5, 2, 2)
            if m_weaponId and m_weaponId ~= 0xFFFFFFFF {
                local m_weapon = getobject(m_weaponId)
                if m_weapon {
                    -- set the ammo
                    writeword(m_weapon + 0x2B6, ammocount)
                    writeword(m_weapon + 0x2B8, clipcount)
                    writefloat(m_weapon + 0x240, math.abs(batterycount - 1))
                    -- force it to sync
                    updateammo(m_weaponId)
                }
            }
            --make sure the script user isn't retarded so we don't get errors
            if zombie_weapon[4] and zombie_weapon[4] ~= "" {
        
                --create the quarternary weapon
                local m_weaponId = createobject(gettagid("weap", zombie_weapon[4]), 0, 60, false, 1, 1, 1)

                --make sure createobject didn't screw up
                if m_weaponId ~= 0xFFFFFFFF {

                    --assign the weapon to the player
                    assignweapon(player, m_weaponId)

                    --make sure we can safely set the ammo
                    local m_weapon = getobject(m_weaponId)
                    if m_weapon {
                        -- set the ammo
                        writeword(m_weapon + 0x2B6, ammocount)
                        writeword(m_weapon + 0x2B8, clipcount)
                        writefloat(m_weapon + 0x240, math.abs(batterycount - 1))
                        -- force it to sync
                        updateammo(m_weaponId)
                    }
                }
            }
        }
    }
    return false
}

function AssignZombieWeapons(id, count, table)
    local player = table[1]
    local clipcount = table[2]
    local ammocount = table[3]
    local batterycount = table[4]
    if not player or not getplayer(player) {
        return false
    }
    local m_object = getplayerobject(player)
    if m_object {
        --count is increased everytime the timer is called
        if count == 1 {
            -- gets rid of any weapons a zombie is holding
            destroyweapons(player)
        }
        local i = count
        if zombie_weapon and zombie_weapon[i] and zombie_weapon[i] ~= "" {
            local m_weaponId = createobject(gettagid("weap", zombie_weapon[i]), 0, 60, false, 1, 1, 1)
            if m_weaponId and m_weaponId ~= 0xFFFFFFFF {
                assignweapon(player, m_weaponId)
                if oddball_or_flag == readdword(getobject(m_weaponId)) {
                    flagball_weap[player] = m_weaponId
                else
                    local m_weapon = getobject(m_weaponId)
                    if m_weapon {
                        -- set the ammo
                        writeword(m_weapon + 0x2B6, ammocount)
                        writeword(m_weapon + 0x2B8, clipcount)
                        writefloat(m_weapon + 0x240, 1)
                        -- force it to sync
                        updateammo(m_weaponId)
                    }
                }
            }
        }
        if count < 4 { return true else return false }
    }
    return false
}
//*/

CNATIVE dllAPI void WINAPIC EXTOnWeaponAssignmentDefault(IPlayer::PlayerInfo plI, ident owningObjectId, IObject::objInfo* curWeapon, DWORD order, ident& newWeaponId) {
    if (game_init && plI.plS) {
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                if (cachePlayer->isZombie) {
                    if (order==0) {
                        //newWeaponId.Tag=oddball_tag_id->id.Tag;
                        pIObject->EquipmentAssign(owningObjectId, oddball_flag_obj[plI.mS->machineIndex]);
                    }// else
                        newWeaponId.Tag=-1;
                }
                break;
            }
        }
    }
}
CNATIVE dllAPI void WINAPIC EXTOnWeaponAssignmentCustom(IPlayer::PlayerInfo plI, ident owningObjectId, ident curWeapon, DWORD order, ident& newWeaponId) {
    if (game_init && plI.plS) {
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                if (cachePlayer->isZombie)
                    newWeaponId.Tag=-1;
                break;
            }
        }
    }
}

//No such thing for EXTOnObjectCreation yet.
/*function OnObjectCreationAttempt(mapId, parentId, player)
    if gametype == "GAMETYPE_CTF" and mapId == flag_tag_id and parentId == nil {
        registertimer(0, "PutUnderMap", readdword(ctf_globals + 0x8))
    }
    --return nil
}*/
CNATIVE dllAPI bool EXTOnObjectInteraction(IPlayer::PlayerInfo plI, ident obj_id, ObjectS* m_object, IObject::hTagHeader* hTag) {
    bool response = true;
    if (game_init > 1) {
        wchar_t msg[256] = { NULL };
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                if (cachePlayer->isZombie) {
                    BipedS* m_biped = (BipedS*)pIObject->GetObjectAddress(plI.plS->CurrentBiped);
                    if (hTag == camouflage_tag_id)  {
                        if (m_biped) {
                            if (m_biped->IsInvisible!=0x51) {
                                rand();
                                //int doInvis = randomRange(1, 11);
                                int doInvis = 1+rand()%10;
                                if (doInvis > 5) {
                                    // make the whole zombie team invis for 30 seconds
                                
                                    util::dynamicStack<IPlayer::PlayerInfo> plList;
                                    pIPlayer->StrToPlayerList(L"*", plList, NULL);
                                    for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
                                        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
                                            if (cachePlayer->uniqueID == plI->mS->UniqueID) {
                                                if (plI->plEx->isInServer && cachePlayer->isZombie) {
                                                    pIPlayer->applyCamo(*plI, 30);
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    customMsg->ValueGet(str1_4, msg, section_str_misc);
                                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg);//zombieinvis_message.c_str());
                                }
                            }
                        }
                    } else if (hTag == overshield_tag_id || hTag == healthpack_tag_id) {
                        //response = nil
                    } else if ((hTag == oddball_tag_id || hTag == flag_tag_id) && m_object->GameObject != -1 && m_object->GameObject!=plI.plA->Team) {
                        if (oddball_flag_obj[plI.mS->machineIndex].Tag==m_biped->sObject.Weapon.Tag ) {
                            WeaponS* m_oddball_flag = (WeaponS*)pIObject->GetObjectAddress(m_biped->sObject.Weapon);
                            ident oddball_flag_id = m_biped->sObject.Weapon;
                            pIObject->EquipmentDropCurrent(plI.plS->CurrentBiped);
                            vect3 loc = vect3(0.0f, 0.0f, 0.0f);
                            pIObject->MoveAndReset(oddball_flag_id, &loc);
                            //pIObject->Update(oddball_flag_id);
                            /*IObject::objManaged managed;
                            managed.rotation=m_oddball_flag->sObject.Rotation;
                            managed.scale=m_oddball_flag->sObject.Scale;
                            managed.velocity=loc;
                            managed.world=loc;
                            pIObject->Move(oddball_flag_id, managed);*/
                            //pIObject->MoveAndReset(obj_id, NULL);
                            //pIObject->EquipmentAssign(plI.plS->CurrentBiped, obj_id);
                            //response = false;
                        }
                    } else// if (obj_id.Tag == fragnade_tag_id->id || obj_id.Tag == plasmanade_tag_id->id) {
                        response = false;
                    //}
                } else {
                    //response = nil
                }
                break;
            }
        }
    }
    return response;
}
CNATIVE dllAPI void WINAPIC EXTOnPlayerDeath(IPlayer::PlayerInfo killerI, IPlayer::PlayerInfo victimI, int mode, bool& showMessage) {
    if (!game_init) return;
    bool response = true;
    std::vector<dataTable>::iterator* cacheKiller = NULL;
    std::vector<dataTable>::iterator* cacheVictim = NULL;
    if (killerI.mS)
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == killerI.mS->UniqueID) {
                cacheKiller = &cachePlayer;
                break;
            }
        }
    if (victimI.mS)
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == victimI.mS->UniqueID) {
                cacheVictim = &cachePlayer;
                break;
            }
        }
    // make sure this kill doesn't add to the score so that the game won't end prematurely
    if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_SLAYER && killerI.plS) {
        pIHaloEngine->gameTypeGlobals->slayerGlobal.playerScore[killerI.plA->PlayerIndex]=0;
    }
    // make sure the game is started
    if (game_init==1) {
        showMessage=false;
        BipedS* z_Biped = (BipedS*)pIObject->GetObjectAddress(victimI.plS->CurrentBiped);
        ObjectS* weapon;
        for (char i = 0; i<4; i++) {
            weapon = pIObject->GetObjectAddress(z_Biped->Equipments[i]);
            if (weapon && weapon->GameObject==-1) {
                if (weapon->ModelTag.Tag == oddball_tag_id->id.Tag)
                    //pIObject->Delete(z_Biped->Equipments[i]);
                    pIObject->Eject(z_Biped->Equipments[i]);
            }
        }
    } else if (game_init > 1) {
        wchar_t msg[256];
        // get the victim's team
        if (victimI.plS && cacheVictim->_Ptr->isZombie) {
            // gets rid of any weapons a zombie is holding
            BipedS* z_Biped = (BipedS*)pIObject->GetObjectAddress(victimI.plS->CurrentBiped);
            ObjectS* weapon;
            for (char i = 0; i<4; i++) {
                weapon = pIObject->GetObjectAddress(z_Biped->Equipments[i]);
                if (weapon && weapon->GameObject==-1) {
                    if (weapon->ModelTag.Tag == oddball_tag_id->id.Tag)
                        //pIObject->Delete(z_Biped->Equipments[i]);
                        pIObject->Eject(z_Biped->Equipments[i]);
                }
            }
        }
        if (victimI.plS) {
            if (resptime) {
                victimI.plS->RespawnTimer = resptime * 33;
            } else if (cacheVictim->_Ptr->isZombie) {
                victimI.plS->RespawnTimer = zombie_spawn_time * 33;
            } else {
                victimI.plS->RespawnTimer = human_spawn_time * 33;
            }
        }
        if (mode == 0) { // server kill
        } else if (mode == 1 && !map_reset_boolean) {// fall damage
            // if this is true end a person who dies from falling will be infected
            if (infect_on_fall) {
                if (!cacheVictim->_Ptr->isZombie) {
                    response = false;
                    customMsg->ValueGet(str1_0, msg, section_str_deaths);
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*falling_infected_message.c_str()*/, victimI.plS->Name);
                    makezombie(victimI, false);
                    //registertimer(100, "makezombiedelay", {victim, true})
                } else if (victimI.plA->Team != human_team) {
                    customMsg->ValueGet(str1_1, msg, section_str_deaths);
                    if (wcslen(msg)) {//falling_death_message.length()) {
                        response = false;
                        customMsg->ValueGet(str1_0, msg, section_str_deaths);
                        pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*falling_death_message.c_str()*/, victimI.plS->Name);
                    }
                }
            } else if (!infect_on_fall) {
                customMsg->ValueGet(str1_1, msg, section_str_deaths);
                if (wcslen(msg)) {//falling_death_message.length()) {
                    response = false;
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*falling_death_message.c_str()*/, victimI.plS->Name);
                }
            }
        } else if (mode == 2) {// killed by guardians
            if (infect_on_guardians) {
                if (!cacheVictim->_Ptr->isZombie) {
                    response = false;
                    customMsg->ValueGet(str4_0, msg, section_str_infected);
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*guardian_infected_message.c_str()*/, victimI.plS->Name);
                    makezombie(victimI, false);
                }
            } else if (!infect_on_guardians) {
                customMsg->ValueGet(str4_0, msg, section_str_infected);
                if (wcslen(msg)) {//guardian_death_message.length()) {
                    response = false;
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*guardian_death_message.c_str()*/, victimI.plS->Name);
                }
            }
        } else if (mode == 3) {// killed by vehicle
        } else if (mode == 4) {// killed by another player
            if (cacheKiller->_Ptr->isZombie && !cacheVictim->_Ptr->isZombie) {
                response = false;
                customMsg->ValueGet(str1_0, msg, section_str_infected);
                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*kill_infected_message.c_str()*/, killerI.plS->Name, victimI.plS->Name);
                makezombie(victimI, false);
                customMsg->ValueGet(str1_8, msg, section_str_misc);
                pIPlayer->sendCustomMsg(MSG_BLANK, 1, victimI, msg /*zombie_message.c_str()*/);
                cacheKiller->_Ptr->human_time+=30;
                if (cacheKiller->_Ptr->zombie_kills) {
                    cacheKiller->_Ptr->zombie_kills+=1;
                    if (cacheKiller->_Ptr->zombie_kills>4) {
                        cacheKiller->_Ptr->zombie_kills=0;
                        customMsg->ValueGet(str1_9, msg, section_str_misc);
                        pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*L"%s is now a human because they infected 5 times!"*/, killerI.plS->Name);
                        makehuman(killerI, true);
                        //registertimer(100, "makehumandelay", {killer, true})
                    }
                } else
                    cacheKiller->_Ptr->zombie_kills=1;
            } else if (!cacheKiller->_Ptr->isZombie && cacheVictim->_Ptr->isZombie) {
                response = false;
                customMsg->ValueGet(str4_0, msg, section_str_deaths);
                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*human_kill_message.c_str()*/, killerI.plS->Name, victimI.plS->Name);
                if (!cur_last_man.plS) {
                    if (victimI.mS && killerI.mS) {
                        ObjectS* cur_weap = pIObject->GetObjectAddress(killerI.plS->CurrentBiped);
                        cur_weap = pIObject->GetObjectAddress(cur_weap->Weapon);
                        if (cur_weap) {
                            IObject::hTagHeader* ammoTag = pIObject->LookupTag(cur_weap->ModelTag);
                            if (ammoTag->tagType1 == IObject::TAG_WEAP) {
                                ident out_objId;
                                vect3 location = victimI.plS->World;
                                location.z+=1;
                                ident parentId;
                                pIObject->Create(cur_weap->ModelTag, parentId, 60, out_objId, &location);
                            }
                        }
                    }
                }
                cacheKiller->_Ptr->human_time+=5;
                //This is not in the code...
                /*if (cur_last_man.mS) {
                    //TODO: This need to be process for spawn weapon or ammo where last survival is at.
                }//*/
            }
        } else if (mode == 5) {// betrayed
            if (infect_on_betrayal) {
                if (!cacheKiller->_Ptr->isZombie) {
                    response = false;
                    customMsg->ValueGet(str3_0, msg, section_str_infected);
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*teammate_infected_message.c_str()*/, killerI.plS->Name, victimI.plS->Name);
                    makezombie(killerI, false);
                } else if (killerI.plA->Team != human_team) {
                    response = false;
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"%s has betrayed %s", killerI.plS->Name, victimI.plS->Name);
                }
            } else {
                response = false;
                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"%s betrayed %s", killerI.plS->Name, victimI.plS->Name);
            }
        } else if (mode == 6) {// suicide
            if (infect_on_suicide) {
                if (!cacheVictim->_Ptr->isZombie) {
                    response = false;
                    customMsg->ValueGet(str2_0, msg, section_str_infected);
                    pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*suicide_infected_message.c_str()*/, victimI.plS->Name);
                    makezombie(victimI, false);
                }
            } else {
                response = false;
                    customMsg->ValueGet(str2_0, msg, section_str_deaths);
                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg /*suicide_message.c_str()*/, victimI.plS->Name);
            }
        }
    }
    //registertimer(200,"checkgamestate",victim)
    showMessage = response;
}


CNATIVE dllAPI int WINAPIC EXTOnPlayerJoinDefault(MachineS* mS, int cur_team) {
    if (game_init > 1) {
        if (pIHaloEngine->globalServer->totalPlayers == 0) {// if no zombies make them human
            cur_team = human_team;
        } else if (cur_zombie_count > 0 && cur_human_count == 0) {
            cur_team = human_team;
        } else
            cur_team = zombie_team;
    }
    return cur_team;
}
CNATIVE dllAPI bool WINAPIC EXTOnPlayerChangeTeamAttempt(IPlayer::PlayerInfo plI, int team, bool forceChange) {
    wchar_t msg[256];
    if (!forceChange) {
        if (!allow_change) {
            customMsg->ValueGet(str1_0, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blockteamchange_message.c_str());
        } else if (team == zombie_team) {
            // this is so memory is updated for processing
            // when voluntary is 0 OnTeamChange is called once the changes
            // have been 
            pIPlayer->changeTeam(plI, zombie_team, true);
            plI.plS->VelocityMultiplier = zombie_speed;
        }
        // we don't let people change team
    
    } else { // we can't stop the person changing teams, bein done by an admin
        // update team counts
        if (team == zombie_team) {
            cur_human_count -= 1;
            cur_zombie_count += 1;
        } else if (team != zombie_team) {
            cur_human_count += 1;
            cur_zombie_count -= 1;
        }
        // they're allowed to change if the timer is active, if it is disable it
        if (allow_change && team == zombie_team) {
            allow_change = false;
            //remove change timer
            /*if player_change_timer {
                removetimer(player_change_timer)
                player_change_timer = nil
            }*/
            customMsg->ValueGet(str1_6, msg, section_str_misc);
            pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg);//timer_team_change_msg.c_str());
        }
        // check if the game has started yet
        if (game_init > 1) {
            // set attributes
            if (team == zombie_team) {
                makezombie(plI, true);
            } else {
                makehuman(plI, true);
            }
            checkgamestate(plI);
            //registertimer(200,"checkgamestate",player)
        }
        // update team
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                cachePlayer->isZombie = (team==zombie_team);
                break;
            }
        }
    }
    return allow_change;
}

//function OnDamageApplication(receiving, causing, tagid, hit, backtap)
CNATIVE dllAPI bool EXTOnObjectDamageApplyProcess(const IObject::objDamageInfo& damageInfo, ident& obj_recv, IObject::objHitInfo& hitInfo, bool isBacktap, bool& allowDamage, bool isManaged) {

    IPlayer::PlayerInfo causer = pIPlayer->getPlayerIdent(damageInfo.player_causer);
    IPlayer::PlayerInfo receiver = pIPlayer->getPlayerIdent(obj_recv);
    if (isBacktap && causer.mS && receiver.mS) {
        if (causer.plA->Team == zombie_team && receiver.plA->Team == human_team) {
            wchar_t msg[256];
            customMsg->ValueGet(str5_0, msg, section_str_infected);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, causer, msg);//zombie_backtap_message.c_str());
        }
    }
    return false;
}

//function OnDamageLookup(receiving, causing, mapId, tagdata)
CNATIVE dllAPI bool EXTOnObjectDamageLookupProcess(IObject::objDamageInfo& damageInfo, ident& obj_recv, bool& allowDamage, bool isManaged) {
    IPlayer::PlayerInfo causer = pIPlayer->getPlayerIdent(damageInfo.player_causer);
    IPlayer::PlayerInfo receiver = pIPlayer->getPlayerIdent(obj_recv);
    if (causer.mS && receiver.mS) {
        if (!pIHaloEngine->gameTypeLive->isTeamPlay && causer.mS != receiver.mS && receiver.plA->Team == causer.plA->Team) {
            return false;
        }
        // if it's a human causing the damage
        if (causer.plA->Team != zombie_team) {
            if (cur_last_man.mS) {
                damageInfo.modifier = lastman_dmgmodifier;
            } else {
                damageInfo.modifier = human_dmgmodifier;
            }
        // It's a zombie causing the damage
        } else if (causer.plA->Team == zombie_team) {
            IObject::hTagHeader* tagType = pIObject->LookupTag(damageInfo.tag_id);
            std::string tagName = tagType->tagName;
            // check if it is melee damage
            if (tagName.find("melee", -5)) {
                damageInfo.modifier = 9999;
            }
        }
    }
    return false;
}

CNATIVE dllAPI bool WINAPIC EXTOnVehicleUserEntry(IPlayer::PlayerInfo plI, bool forceEntry) {
    if (forceEntry)
        return true;
    if (game_init > 1) {
        if ((plI.plA->Team == zombie_team && zombies_allowed_in_vehis) || (plI.plA->Team != zombie_team && humans_allowed_in_vehis)) {
            return true;
        }
    }
    return false;
}

CNATIVE dllAPI bool WINAPIC EXTOnPlayerAttemptDropObject(IPlayer::PlayerInfo plI, ident owningObjectId, BipedS* pl_Biped) {
    if (game_init && plI.plS) {
        WeaponS* weap = (WeaponS*)pIObject->GetObjectAddress(pl_Biped->sObject.Weapon);
        if (!weap)
            return 0;
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI.mS->UniqueID) {
                //pIPlayer->sendCustomMsg(MSG_INFO, 1, pl_null, L"");
                if (cachePlayer->isZombie && weap && weap->sObject.GameObject==-1)
                    return 0;
                else if (cachePlayer->isZombie /*&& weap && weap->sObject.GameObject!=-1*/) {
                    pIObject->EquipmentAssign(plI.plS->CurrentBiped, oddball_flag_obj[plI.mS->machineIndex]);
                }
                break;
            }
        }
    }
    return 1;
}


CNATIVE dllAPI bool EXTOnPlayerScoreCTF(IPlayer::PlayerInfo plI, ident cur_weap_id, DWORD team, bool isGameObject) {
    if (!game_init) return true;  //Default to allow score
    if (!isGameObject) return false; //If game is initialized, do NOT allow any false game objects to score.
    /*
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (cachePlayer->uniqueID == plI.mS->UniqueID) {
            if (cachePlayer->isZombie) {
                //pIObject->Create(oddball_tag_id->id, parentId, 60, out_objId, &location);
                //timers[AssignWeaponAfterScore] = addon::pITimer->EXTAddOnTimerAdd(plI, 10);
            }
            break;
        }
    }*/
    pIObject->EquipmentAssign(plI.plS->CurrentBiped, oddball_flag_obj[plI.mS->machineIndex]);
    return true; //Default to allow score IF it is a valid game object.
}
//function OnClientUpdate(player)
CNATIVE dllAPI void WINAPIC EXTOnPlayerUpdate(IPlayer::PlayerInfo plI) {
    if (!game_init) return;

    if (!(plI.plS && plI.plS->CurrentBiped.Tag!=-1))
        return;

    BipedS* pl_biped = (BipedS*)pIObject->GetObjectAddress(plI.plS->CurrentBiped);
    if (!pl_biped)
        return;

    std::vector<dataTable>::iterator* cachePlayer = NULL;
    for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
        if (cachePlayer->uniqueID == plI.mS->UniqueID) {
            if (pl_biped->Flashlight==8 && *pIHaloEngine->mapUpTimeLive >= cachePlayer->activateFlashlight) {
                cachePlayer->activateFlashlight = *pIHaloEngine->mapUpTimeLive + 30*3;
                //TODO: No longer required timer for haveSpeedTimer. #1
                //registertimer(3000, "haveSpeedTimer", player);
                plI.plS->VelocityMultiplier= cachePlayer->isZombie?zombie_speed:human_speed;
            }
            break;
        }
    }
    //TODO: Not really needed for setting the player's score...?
    // this block of code sets the player's score according to what it should be.
    /*if tonumber(human_time[name]) {
        local m_player = getplayer(player)
        if gametype == "KOTH" {
            writewordsigned(m_player + 0xC4, human_time[name]*30)
        } else if ( gametype == "GAMETYPE_CTF" {
            writedwordsigned(m_player + 0xC8, human_time[name])
        } else if ( gametype == "Slayer" {
            writedwordsigned(slayer_globals + 0x40 + player * 4, human_time[name])
        }
    else
        human_time[name] = 0
    }*/
    //this makes sure that people won't camp in unreachable places. and doesnt work
    vect3 curCoord = pl_biped->sObject.World;
    wchar_t msg[256];
    if (plI.plA->Team!=zombie_team)
    if (_stricmp(pIHaloEngine->mapCurrent->mapName, "bloodgulch")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(39.14f, -96.18f, 2.12f), 4)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_0, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_tree_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(92.73f, -96.74f, 9.21f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "damnation")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(-2.11f, 12.05f, 7.82f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_2, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_glitch_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(5.16f, 15.42f, 8.06f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_2, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_glitch_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-1.09f, 13.63f, 7.82f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_2, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_glitch_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-7.25f, 12.93f, 5.60f), 1.5)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "icefields")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(18.47f, -6.61f, 6.55f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(17.17f, -6.64f, 6.61f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(32.68f, -33.29f, 5.43f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-17.13f, 19.32f, 11.58f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-72.06f, 72.52f, 3.92f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-85.53f, 97.83f, 5.27f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "sidewinder")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(-6.47f, -33.76f, 4.41f), 5)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_2, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_glitch_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(4.77f, -33.43f, 4.41f), 5)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_2, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_glitch_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "deathisland")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(-26.3f, -7.72f, 11.7f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-25.98f, -6.25f, 11.7f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(29.73f, 15.29f, 10.47f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(29.57f, -17.1f, 10.99f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "putput")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(-4.59f, -20.67f, 3.3f), 1.5)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-2.78f, -20.84f, 3.3f), 1.5)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    } else if (_stricmp(pIHaloEngine->mapCurrent->mapName, "chillout")==0) {
        if (object_in_sphere(pl_biped->sObject.World, vect3(11.26f, 8.82f, 3.16f), 2)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        } else if (object_in_sphere(pl_biped->sObject.World, vect3(-7.16f, 7.8f, 4.34f), 3)) {
            pIObject->Kill(plI.plS->CurrentBiped);
            customMsg->ValueGet(str2_1, msg, section_str_misc);
            pIPlayer->sendCustomMsg(MSG_BLANK, 1, plI, msg);//blocked_spot_message.c_str());
        }
    }
}

CNATIVE dllAPI void WINAPIC EXTOnMapReset() {
    if (game_init<1) return;

    //What to add here? Wizard want to destroy the objects, however they're being re-used.
}
//*/
/*
#define invisCrouch             0
#define HumanTimer              1
#define NewGameTimer            2
#define MsgTimer                3
#define SpeedTimer              4
#define PlayerChangeTimer       5
#define RemoveLastmanProtection 6
#define ForceSwitchEquip        7
//*/
inline void invisCrouchFunc() {
    if (!zombies_invisible_on_crouch && !humans_invisible_on_crouch) {
        timers[invisCrouch] = -1;
    }
        
    util::dynamicStack<IPlayer::PlayerInfo> plList;
    pIPlayer->StrToPlayerList(L"*", plList, NULL);
    for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
        for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
            if (cachePlayer->uniqueID == plI->mS->UniqueID) {
                BipedS* pl_biped = (BipedS*)pIObject->GetObjectAddress(plI->plS->CurrentBiped);
                if (pl_biped) {
                    if (pl_biped->actionBits.crouching) {
                        if ((cachePlayer->isZombie && zombies_invisible_on_crouch) || (!cachePlayer->isZombie && humans_invisible_on_crouch)) {
                            pIPlayer->applyCamo(*plI, time_invis);
                        }
                    }
                }
                break;
            }
        }
    }
    timers[invisCrouch] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 6); //200 milliseconds
}
inline void HumanTimerFunc() {
    if (map_reset_boolean)
        map_reset_boolean=false;
    if (game_init > 1) {
        util::dynamicStack<IPlayer::PlayerInfo> plList;
        pIPlayer->StrToPlayerList(L"*", plList, NULL);
        wchar_t msg[256];
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
                if (cachePlayer->uniqueID == plI->mS->UniqueID) {
                    if (!cachePlayer->isZombie) {
                        cachePlayer->human_time+=1;
                    }
                    if (pIHaloEngine->gameTypeLive->GameStage == GAMETYPE_KOTH) {
                        ObjectS* pl_biped = pIObject->GetObjectAddress(plI->plS->CurrentBiped);
                        if (pIHaloEngine->gameTypeGlobals->kothGlobal.isInHill[plI->plS->PlayerIndex]) {
                            if (cachePlayer->inhill_time==-1) {
                                cachePlayer->inhill_time=0;
                                pl_biped->unk2=1;
                                //writebit(m_object + 0x10, 7, 1)//This is written to bipedS.
                                if (cachePlayer->isZombie) {
                                    customMsg->ValueGet(str3_1, msg, section_str_misc);
                                    //pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, koth_in_hill_zombie_message.c_str(), plI->plS->Name);
                                } else {
                                    customMsg->ValueGet(str3_0, msg, section_str_misc);
                                    //pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, koth_in_hill_human_message.c_str(), plI->plS->Name);
                                }
                                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg, plI->plS->Name);
                            } else if (cachePlayer->inhill_time>=10) {
                                if (cachePlayer->isZombie) {
                                    pIHaloEngine->Kill(*plI);
                                    customMsg->ValueGet(str5_0, msg, section_str_deaths);
                                    //pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, in_hill_too_long_zombie_msg.c_str(), plI->plS->Name);
                                } else {
                                    makezombie(*plI, true);
                                    customMsg->ValueGet(str6_0, msg, section_str_infected);
                                    //pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, in_hill_too_long_human_msg.c_str(), plI->plS->Name);
                                }
                                pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, msg, plI->plS->Name);
                                cachePlayer->inhill_time=-1;
                            } else if (!cachePlayer->isZombie) {
                                customMsg->ValueGet(str3_2, msg, section_str_misc);
                                pIPlayer->sendCustomMsg(MSG_BLANK, 1, *plI, msg /*koth_in_hill_human_warn_message.c_str()*/, 10-cachePlayer->inhill_time);
                                cachePlayer->inhill_time+=1;
                            } else
                                cachePlayer->inhill_time+=1;
                        } else if (pl_biped) {
                            cachePlayer->inhill_time=-1;
                            pl_biped->unk2=0;
                        }
                    }
                    break;
                }
            }
        }
        timers[HumanTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 30);
    }
}
inline void NewGameTimerFunc() {
    game_init = 0;    //Just to be sure...
    if (pIHaloEngine->globalServer->totalPlayers>1) {
        int newgame_zombie_count = 0;
        // by default make all players human
        util::dynamicStack<IPlayer::PlayerInfo> plList;
        pIPlayer->StrToPlayerList(L"*", plList, NULL);
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            bool isNotListed=1;
            for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
                if (cachePlayer->uniqueID == plI->mS->UniqueID) {
                    if (cachePlayer->isZombie || plI->plS->Team == zombie_team) {
                        pIPlayer->changeTeam(*plI, human_team, true);
                    }
                    isNotListed=0;
                    break;
                }
            }
            if (isNotListed && plI->plS->Team==zombie_team)
                pIPlayer->changeTeam(*plI, human_team, true);
        }
        game_init = 1;
        cacheTable.clear();
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            validatePlayers(*plI);
        }
        int possible_count = pIHaloEngine->globalServer->totalPlayers;
        zombie_count = 0;
        // make last man zombie
        int last_man_unique = -1;
        // check if the last man is to be made a zombie
        // if so find who was last man
        if (last_man_next_zombie == true && pIHaloEngine->globalServer->totalPlayers > 1 && cur_last_man.mS) {
            last_man_unique = cur_last_man.mS->UniqueID;
            makezombie(cur_last_man, true);
            newgame_zombie_count = 1;
        }
        // reset last man
        cur_last_man = IPlayer::PlayerInfo();
        /*if (finalZombies == pIHaloEngine->globalServer->totalPlayers) { // if 0 players they will be human
            finalZombies -= 1;
        } else if (finalZombies > possible_count) { // fix the count
            finalZombies = possible_count;
        } else if (max_zombie_count && finalZombies > max_zombie_count) { // cap the zombie count
            finalZombies = max_zombie_count;
        } else if ( finalZombies < 0) {
            finalZombies = 0;
        }*/
            
        // loop through the players, randomly selecting ones to become
        // zombies
        //while (newgame_zombie_count) {
        if (!newgame_zombie_count) {
            // randomly choose a player
            redrawPlayer:
            IPlayer::PlayerInfo newzomb = ChooseRandomPlayer(zombie_team);
            if ( newzomb.mS->UniqueID != last_man_unique ) {
                makezombie(newzomb, true);
                newgame_zombie_count += 1;
            } else
                goto redrawPlayer;
        }
        // fix the team counters
        cur_zombie_count = newgame_zombie_count;
        cur_human_count = pIHaloEngine->globalServer->totalPlayers - newgame_zombie_count;
        // reset the map
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            EXTOnPlayerSpawnColor(*plI, pIHaloEngine->gameTypeLive->isTeamPlay);
        }
        char i = 0;
        do {
            ident out_objId;
            ident parentId;
            pIObject->Create(oddball_tag_id->id,parentId, 0, oddball_flag_obj[i], &location);
            i++;
        } while(i<16);
        //((h_tag_weap*)(oddball_tag_id->metaData1))->must_be_readied=0;
        //((h_tag_weap*)(oddball_tag_id->metaData1))->prevent_melee_attack=1;
        //((h_tag_weap*)(flag_tag_id->metaData1))->must_be_readied=0;
        pIHaloEngine->Exec("sv_map_reset");
        pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"The game has started!");
        wchar_t msg[256];
        for(util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
            for(std::vector<dataTable>::iterator cachePlayer = cacheTable.begin(); cachePlayer < cacheTable.end(); cachePlayer++) {
                if (cachePlayer->uniqueID == plI->mS->UniqueID) {
                    // check if they're a zombie
                    if (cachePlayer->isZombie)
                        customMsg->ValueGet(str1_8, msg, section_str_misc);
                        //pIPlayer->sendCustomMsg(MSG_BLANK, 1, *plI, zombie_message.c_str());
                    else
                        customMsg->ValueGet(str1_7, msg, section_str_misc);
                        //pIPlayer->sendCustomMsg(MSG_BLANK, 1, *plI, human_message.c_str());
                    pIPlayer->sendCustomMsg(MSG_BLANK, 1, *plI, msg);
                    break;
                }
            }
                
        }
        timers[NewGameTimer]=-1;
        game_init = 2;
        if (timers[HumanTimer]==-1)
            timers[HumanTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 30);

        if (timers[invisCrouch]==-1)
            timers[invisCrouch] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 6); //6 = 200 miliseconds

        if (timers[SpeedTimer]==-1)
            timers[SpeedTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 90); // 3 seconds
        checkgamestate(pl_null);
    } else {
        timers[NewGameTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 225);    //7.5 seconds
    }
    //resptime = 0;
}
inline void MsgTimerFunc() {
    
        /*
bool MsgTimer(id, count, player) {
    if getplayer(player) then
        privatesay(player, welcome_message)
        if gametype == "KOTH" then
            privatesay(player, koth_additional_welcome_msg)
        } else if ( gametype == "Slayer" then
            --local msg = tokenizestring(slayer_additional_welcome_msg, "\n")
            --privatesay(player, msg[1])
            --privatesay(player, msg[2])
        end
    end
    return false;
    */
}
inline void SpeedTimerFunc() {
    util::dynamicStack<IPlayer::PlayerInfo> plList;
    pIPlayer->StrToPlayerList(L"*", plList, NULL);
    for (util::dynamicStack<IPlayer::PlayerInfo>::iterator plI = plList.begin(); plI != NULL; plI++) {
        if (plI->plS) {
            float speed = plI->plS->VelocityMultiplier;
            if (plI->plA->Team==zombie_team && speed < zombie_speed) {
                plI->plS->VelocityMultiplier=zombie_speed;
            } else if (plI->plS == cur_last_man.plS && speed < lastman_speed) {
                plI->plS->VelocityMultiplier=lastman_speed;
            } else {
                plI->plS->VelocityMultiplier=human_speed;
            }
        }
    }
    timers[SpeedTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 30);
}
inline void PlayerChangeTimerFunc() {
    if (playerChangeCounter>0 && pIHaloEngine->gameTypeLive->isTeamPlay) {
        if (!allow_change || cur_zombie_count > 0) {
            allow_change=false;
            pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"Thank you, the game can continue.");
            timers[PlayerChangeTimer]=-1;
        } else
            pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"In %d seconds a player will be forced to become a zombie.", 6 - playerChangeCounter);
    } else { //timer up, force team change
        allow_change=false;
        // pick a human and make them zombie.
        IPlayer::PlayerInfo newZomb = ChooseRandomPlayer(zombie_team);
        if (newZomb.plS) {
            makezombie(newZomb, true);
        } else
            pIPlayer->sendCustomMsgBroadcast(MSG_BLANK, L"PlayerChangeTimer got null");
        timers[PlayerChangeTimer]=-1;
    }
    if (timers[PlayerChangeTimer]!=-1) {
        timers[PlayerChangeTimer] = addon::pITimer->EXTAddOnTimerAdd(pl_null, 30);
        playerChangeCounter-=1;
    } else
        playerChangeCounter=0;
}
inline void RemoveLastmanProtectionFunc() {
    //writebit(m_object + 0x10, 7, 0)
}

CNATIVE dllAPI void WINAPIC EXTOnTimerCancel(DWORD idTimer) {
    if (timers[invisCrouch]=idTimer)
        timers[invisCrouch]=-1;
    else if (timers[HumanTimer]=idTimer)
        timers[HumanTimer]=-1;
    else if (timers[NewGameTimer]=idTimer)
        timers[NewGameTimer]=-1;
    else if (timers[MsgTimer]=idTimer)
        timers[MsgTimer]=-1;
    else if (timers[SpeedTimer]=idTimer)
        timers[SpeedTimer]=-1;
    else if (timers[PlayerChangeTimer]=idTimer)
        timers[PlayerChangeTimer]=-1;
    else if (timers[RemoveLastmanProtection]=idTimer)
        timers[RemoveLastmanProtection]=-1;
}
CNATIVE dllAPI void WINAPIC EXTOnTimerExecute(DWORD idTimer) {
    if (timers[NewGameTimer]==idTimer)
            NewGameTimerFunc();
    else if (game_init>0) {
        if (timers[invisCrouch]==idTimer)
            invisCrouchFunc();
        else if (timers[HumanTimer]==idTimer)
            HumanTimerFunc();
        else if (timers[MsgTimer]==idTimer)
            MsgTimerFunc();
        else if (timers[SpeedTimer]==idTimer)
            SpeedTimerFunc();
        else if (timers[PlayerChangeTimer]==idTimer)
            PlayerChangeTimerFunc();
        else if (timers[RemoveLastmanProtection]==idTimer)
            RemoveLastmanProtectionFunc();
        
    } else
        EXTOnTimerCancel(idTimer);
}

//Commands Section Begin
static const wchar_t eao_infection_enable_str[] = L"eao_gametype_infection_enable";
toggle eaoGametypeInfectionEnable(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setInfectionEnable = util::StrToBooleanW(arg[1]);
        if (setInfectionEnable==-1) {
            return CMDFAIL;
        } else if (setInfectionEnable==1 && game_enable==0) {
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
            game_enable=1;
            if (pIHaloEngine->mapCurrent->cam_multi_menu==1) {
                Init();
            }
        } else if (setInfectionEnable==0 && game_enable==1) {
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
            game_enable=0;
            Uninit();
        } else {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_2, msg[1], section_str_general);
    customMsg->ValueGet((game_enable?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}

static const wchar_t eao_infection_zombie_team_str[] = L"eao_gametype_infection_zombie_team";
toggle eaoGametypeInfectionZombieTeam(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    toggle setZombieTeam = util::StrToTeamW(arg[1]);
    if (arg.argc==1) {
        customMsg->ValueGet(str2_3, msg[0], section_str_general);
    } else if (setZombieTeam==-1) {
        return CMDFAIL;
    } else {
        if (setZombieTeam==zombie_team) {
            customMsg->ValueGet(str2_4, msg[0], section_str_general);
        } else {
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
            join_team = zombie_team = setZombieTeam;
            human_team = !zombie_team;
        }
    }
    customMsg->ValueGet(str1_5, msg[1], section_str_general);
    customMsg->ValueGet((zombie_team?str1_4:str1_3), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}

static const wchar_t eao_infection_zombie_respawn_str[] = L"eao_gametype_infection_zombie_respawn";
toggle eaoGametypeInfectionZombieRespawn(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    if (!util::isnumberW(arg[1]))
        return CMDFAIL;
    wchar_t msg[2][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_5, msg[0], section_str_general);
    } else {
        int setZombieRespawn=_wtoi(arg[1]);
        if (setZombieRespawn<0)
            return CMDFAIL;
        if (zombie_spawn_time==setZombieRespawn) {
            customMsg->ValueGet(str2_7, msg[0], section_str_general);
        } else {
            zombie_spawn_time = setZombieRespawn;
            customMsg->ValueGet(str2_6, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_5, msg[1], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], zombie_spawn_time);
    return CMDSUCC;
}
static const wchar_t eao_infection_human_respawn_str[] = L"eao_gametype_infection_human_respawn";
toggle eaoGametypeInfectionHumanRespawn(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[2][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_5, msg[0], section_str_general);
    } else {
        if (!util::isnumberW(arg[1]))
            return CMDFAIL;
        int setHumanRespawn=_wtoi(arg[1]);
        if (setHumanRespawn<0)
            return CMDFAIL;
        if (human_spawn_time==setHumanRespawn) {
            customMsg->ValueGet(str2_7, msg[0], section_str_general);
        } else {
            human_spawn_time = setHumanRespawn;
            customMsg->ValueGet(str2_6, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_6, msg[1], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], human_spawn_time);
    return CMDSUCC;
}

static const wchar_t eao_infection_zombie_speed_str[] = L"eao_gametype_infection_zombie_speed";
toggle eaoGametypeInfectionZombieSpeed(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    if (!util::isfloatW(arg[1]))
        return CMDFAIL;
    float setZombieSpeed=(float)_wtof(arg[1]);
    wchar_t msg[2][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_8, msg[0], section_str_general);
    } else {
        if (zombie_speed==setZombieSpeed) {
            customMsg->ValueGet(str2_10, msg[0], section_str_general);
        } else {
            zombie_speed = setZombieSpeed;
            customMsg->ValueGet(str2_9, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_5, msg[1], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], zombie_speed);
    return CMDSUCC;
}
static const wchar_t eao_infection_human_speed_str[] = L"eao_gametype_infection_human_speed";
toggle eaoGametypeInfectionHumanSpeed(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    if (!util::isfloatW(arg[1]))
        return CMDFAIL;
    float setHumanSpeed=(float)_wtof(arg[1]);
    wchar_t msg[2][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_8, msg[0], section_str_general);
    } else {
        if (human_speed==setHumanSpeed) {
            customMsg->ValueGet(str2_10, msg[0], section_str_general);
        } else {
            human_speed = setHumanSpeed;
            customMsg->ValueGet(str2_9, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_6, msg[1], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], human_speed);
    return CMDSUCC;
}
static const wchar_t eao_infection_last_man_speed_str[] = L"eao_gametype_infection_last_man_speed";
toggle eaoGametypeInfectionLastManSpeed(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    if (!util::isfloatW(arg[1]))
        return CMDFAIL;
    float setLastManSpeed=(float)_wtof(arg[1]);
    wchar_t msg[2][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_8, msg[0], section_str_general);
    } else {
        if (lastman_speed==setLastManSpeed) {
            customMsg->ValueGet(str2_10, msg[0], section_str_general);
        } else {
            lastman_speed = setLastManSpeed;
            customMsg->ValueGet(str2_9, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_7, msg[1], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], lastman_speed);
    return CMDSUCC;
}

static const wchar_t eao_infection_last_man_next_zombie_str[] = L"eao_gametype_infection_last_man_next_zombie";
toggle eaoGametypeInfectionLastManNextZombie(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setLastManNextZombie=util::StrToBooleanW(arg[1]);
        if (setLastManNextZombie==-1) {
            return CMDFAIL;
        } else if (last_man_next_zombie==(setLastManNextZombie!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            last_man_next_zombie = (setLastManNextZombie!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_7, msg[1], section_str_general);
    customMsg->ValueGet((last_man_next_zombie?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}

static const wchar_t eao_infection_zombie_allow_in_vehicle_str[] = L"eao_gametype_infection_zombie_allow_in_vehicle";
toggle eaoGametypeInfectionZombieAllowInVehicle(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_11, msg[0], section_str_general);
    } else {
        toggle setZombieAllowInVehicle=util::StrToBooleanW(arg[1]);
        if (setZombieAllowInVehicle==-1) {
            return CMDFAIL;
        } else if (zombies_allowed_in_vehis==(setZombieAllowInVehicle!=0)) {
            customMsg->ValueGet(str2_13, msg[0], section_str_general);
        } else {
            zombies_allowed_in_vehis = (setZombieAllowInVehicle!=0);
            customMsg->ValueGet(str2_12, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_5, msg[1], section_str_general);
    customMsg->ValueGet((zombies_allowed_in_vehis?str1_9:str1_8), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}
static const wchar_t eao_infection_human_allow_in_vehicle_str[] = L"eao_gametype_infection_human_allow_in_vehicle";
toggle eaoGametypeInfectionHumanAllowInVehicle(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_11, msg[0], section_str_general);
    } else {
        toggle setHumanAllowInVehicle=util::StrToBooleanW(arg[1]);
        if (setHumanAllowInVehicle==-1) {
            return CMDFAIL;
        } else if (humans_allowed_in_vehis==(setHumanAllowInVehicle!=0)) {
            customMsg->ValueGet(str2_13, msg[0], section_str_general);
        } else {
            humans_allowed_in_vehis = (setHumanAllowInVehicle!=0);
            customMsg->ValueGet(str2_12, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_6, msg[1], section_str_general);
    customMsg->ValueGet((humans_allowed_in_vehis?str1_9:str1_8), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}


static const wchar_t eao_infection_infect_on_fall_str[] = L"eao_gametype_infection_infect_on_fall";
toggle eaoGametypeInfectionInfectOnFall(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setInfectOnFall=util::StrToBooleanW(arg[1]);
        if (setInfectOnFall==-1) {
            return CMDFAIL;
        } else if (infect_on_fall==(setInfectOnFall!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            infect_on_fall = (setInfectOnFall!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_10, msg[1], section_str_general);
    customMsg->ValueGet((infect_on_fall?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}
static const wchar_t eao_infection_infect_on_guardians_str[] = L"eao_gametype_infection_infect_on_guardians";
toggle eaoGametypeInfectionInfectOnGuardians(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setInfectOnGuardians=util::StrToBooleanW(arg[1]);
        if (setInfectOnGuardians==-1) {
            return CMDFAIL;
        } else if (infect_on_guardians==(setInfectOnGuardians!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            infect_on_guardians = (setInfectOnGuardians!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_11, msg[1], section_str_general);
    customMsg->ValueGet((infect_on_guardians?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}
static const wchar_t eao_infection_infect_on_suicide_str[] = L"eao_gametype_infection_infect_on_suicide";
toggle eaoGametypeInfectionInfectOnSuicide(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setInfectOnSuicide=util::StrToBooleanW(arg[1]);
        if (setInfectOnSuicide==-1) {
            return CMDFAIL;
        } else if (infect_on_suicide==(setInfectOnSuicide!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            infect_on_suicide = (setInfectOnSuicide!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_12, msg[1], section_str_general);
    customMsg->ValueGet((infect_on_suicide?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}
static const wchar_t eao_infection_infect_on_betrayal_str[] = L"eao_gametype_infection_infect_on_betrayal";
toggle eaoGametypeInfectionInfectOnBetrayal(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc==1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setInfectOnBetrayal=util::StrToBooleanW(arg[1]);
        if (setInfectOnBetrayal==-1) {
            return CMDFAIL;
        } else if (infect_on_betrayal==(setInfectOnBetrayal!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            infect_on_betrayal = (setInfectOnBetrayal!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_13, msg[1], section_str_general);
    customMsg->ValueGet((infect_on_betrayal?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}

static const wchar_t eao_infection_zombie_invisible_on_crouch_str[] = L"eao_gametype_infection_zombie_invisible_on_crouch";
toggle eaoGametypeInfectionZombieInvisibleOnCrouch(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    if (game_init>0) {
        return CMDFAIL;
    }
    wchar_t msg[3][256];
    if (arg.argc == 1) {
        customMsg->ValueGet(str2_0, msg[0], section_str_general);
    } else {
        toggle setZombieInvisibleOnCrouch=util::StrToBooleanW(arg[1]);
        if (setZombieInvisibleOnCrouch==-1) {
            return CMDFAIL;
        } else if (zombies_invisible_on_crouch==(setZombieInvisibleOnCrouch!=0)) {
            customMsg->ValueGet(str2_2, msg[0], section_str_general);
        } else {
            zombies_invisible_on_crouch = (setZombieInvisibleOnCrouch!=0);
            customMsg->ValueGet(str2_1, msg[0], section_str_general);
        }
    }
    customMsg->ValueGet(str1_14, msg[1], section_str_general);
    customMsg->ValueGet((zombies_invisible_on_crouch?str1_1:str1_0), msg[2], section_str_general);
    pIPlayer->sendCustomMsg(MSG_BLANK, chatRconRemote, plI, msg[0], msg[1], msg[2]);
    return CMDSUCC;
}
//Commands Section End

static addon::addonInfo eaoInfo = { L"Infection GameType Add-on", L"1.0.0.0",
                            L"RadWolfie & Wizard",
                            L"It provide ability simulate a zombie gametype with almost any proper gametype.",
                            L"Infection Gametype",
                            L"gametype",
                            NULL,
                            NULL,
                            NULL,
                            NULL};


CNATIVE dllAPI addon::addonInfo* EXTPluginInfo = &eaoInfo;

CNATIVE dllAPI toggle WINAPIC EXTOnEAOLoad() {
    if (pIHaloEngine->haloGameVersion==HALO_TRIAL) //Does not support Halo Trial due to sv_map_reset is not featured in Trial version.
        return EAOFAIL;
    srand (time_t()); //NOTICE: THIS IS REQUIRED FOR RAND FUNCTION TO WORK!
    cacheTable.resize(32);
    cacheTable.clear();
    cur_zombie_count = 0;
    cur_human_count = 0;
    alpha_zombie_count = 0;
    timers[6]=timers[5]=timers[4]=timers[3]=timers[2]=timers[1]=timers[0]=-1;  // { -1 } does not do as expected! Curse you MVS!
    cur_last_man = IPlayer::PlayerInfo();
    game_init = 0;
    game_enable = false;
    allow_change = false;
    velocity_reset.z=velocity_reset.y=velocity_reset.x=0;
    pl_null = IPlayer::PlayerInfo();
    pICommand->ReloadLevel();
    customMsg = getICIniFile();
    if (!customMsg)
        return EAOFAIL;
    if (!customMsg->Open(customMsgStr)) {
        if (customMsg->Create(customMsgStr))
            if (customMsg->Open(customMsgStr))
                goto successInitCustomMsg;
        customMsg->Release();
        return EAOFAIL;
    }
    successInitCustomMsg:
    customMsg->Load();
    //Death section, 1.x = player "fall" deaths
    if (!customMsg->ValueExist(str1_0, section_str_deaths))
        customMsg->ValueSet(str1_0, L"%s fell to his death...", section_str_deaths);
    if (!customMsg->ValueExist(str1_1,section_str_deaths))
        customMsg->ValueSet(str1_1, L"%s slipped and fell...", section_str_deaths);

    //Death section, 2.x = player's suicide notes (does not get to be infected.)
    if (!customMsg->ValueExist(str2_0, section_str_deaths))
        customMsg->ValueSet(str2_0, L"%s made mistakes...", section_str_deaths);
   // if (!customMsg->ValueExist(str1_, section_str_deaths))
   //     customMsg->ValueSet(str1_, , section_str_deaths);

    //Death section, 3.x = player betrayal
    if (!customMsg->ValueExist(str3_0, section_str_deaths))
        customMsg->ValueSet(str3_0, L"%s was infected for betraying %s", section_str_deaths);

    //Death section, 4.x = player kills
    if (!customMsg->ValueExist(str4_0, section_str_deaths))
        customMsg->ValueSet(str4_0, L"%s has killed %s", section_str_deaths);

    //Death section, 5.x = zombie stayed in hill too long (KOTH gametype mixture)
    if (!customMsg->ValueExist(str5_0, section_str_deaths))
        customMsg->ValueSet(str5_0, L"%s has been killed because they were in the hill too long!", section_str_deaths);


    //Infected section, 1.x = zombie infecting players
    if (!customMsg->ValueExist(str1_0, section_str_infected))
        customMsg->ValueSet(str1_0, L"%s has infected %s", section_str_infected);

    //Infected section, 2.x = player's sucicide notes (does get to be infected.)
    if (!customMsg->ValueExist(str2_0, section_str_infected))
        customMsg->ValueSet(str2_0, L"%s lost the will to live...", section_str_infected);

    //Infected section, 3.x = player betrayal
    if (!customMsg->ValueExist(str3_0, section_str_infected))
        customMsg->ValueSet(str3_0, L"%s was infected for betraying %s", section_str_infected);

    //Infected section, 4.x = ai kills (aka guardian kills)
    if (!customMsg->ValueExist(str4_0, section_str_infected))
        customMsg->ValueSet(str4_0, L"%s was infected by an angry ghost!", section_str_infected);

    //Infected section, 5.x = complement backtap messages
    if (!customMsg->ValueExist(str5_0, section_str_infected))
        customMsg->ValueSet(str5_0, L"Nice backtap!", section_str_infected);

    //Infected section, 6.x = player stayed in hill too long (KOTH gametype mixture)
    if (!customMsg->ValueExist(str6_0, section_str_infected))
        customMsg->ValueSet(str6_0, L"%s was infected because they were in the hill too long!", section_str_infected);

    //Misc section, 1.x = Info messages
    if (!customMsg->ValueExist(str1_0, section_str_misc))
        customMsg->ValueSet(str1_0, L"Autobalance: You're not allowed to change team.", section_str_misc);
    if (!customMsg->ValueExist(str1_1, section_str_misc))
        customMsg->ValueSet(str1_1, L"Don't team kill...", section_str_misc);
    if (!customMsg->ValueExist(str1_2, section_str_misc))
        customMsg->ValueSet(str1_2, L"There are no zombies left. Someone needs to change team or be forced to.", section_str_misc);
    if (!customMsg->ValueExist(str1_3, section_str_misc))
        customMsg->ValueSet(str1_3, L"%s is the last human alive and is invisible for %d seconds!", section_str_misc);
    if (!customMsg->ValueExist(str1_4, section_str_misc))
        customMsg->ValueSet(str1_4, L"Please don't leave and rejoin. You've been put back onto your last team.", section_str_misc);
    if (!customMsg->ValueExist(str1_5, section_str_misc))
        customMsg->ValueSet(str1_5, L"The zombies are invisible for 30 seconds!", section_str_misc);
    if (!customMsg->ValueExist(str1_6, section_str_misc))
        customMsg->ValueSet(str1_6, L"Thank you. The game will now continue", section_str_misc);
    if (!customMsg->ValueExist(str1_7, section_str_misc))
        customMsg->ValueSet(str1_7, L"YOU'RE A HUMAN. Survive!", section_str_misc);
    if (!customMsg->ValueExist(str1_8, section_str_misc))
        customMsg->ValueSet(str1_8, L"YOU'RE A ZOMBIE. FEED ON HUMANS!", section_str_misc);
    if (!customMsg->ValueExist(str1_9, section_str_misc))
        customMsg->ValueSet(str1_9, L"%s is now a human because they infected 5 times!", section_str_misc);

    //Misc section, 2.x = Blocked message
    if (!customMsg->ValueExist(str2_0, section_str_misc))
        customMsg->ValueSet(str2_0, L"This tree is blocked.", section_str_misc);
    if (!customMsg->ValueExist(str2_1, section_str_misc))
        customMsg->ValueSet(str2_1, L"Sorry this spot has been blocked...", section_str_misc);
    if (!customMsg->ValueExist(str2_2, section_str_misc))
        customMsg->ValueSet(str2_2, L"Glitching is not allowed!", section_str_misc);

    //Misc section, 3,x = KOTH message
    if (!customMsg->ValueExist(str3_0, section_str_misc))
        customMsg->ValueSet(str3_0, L"%s must leave the hill in 10 seconds or they will be infected!", section_str_misc);
    if (!customMsg->ValueExist(str3_1, section_str_misc))
        customMsg->ValueSet(str3_1, L"%s must leave the hill in 10 seconds or they will be killed!", section_str_misc);
    if (!customMsg->ValueExist(str3_2, section_str_misc))
        customMsg->ValueSet(str3_2, L"You have %d seconds to leave the hill!", section_str_misc);
    //if (!customMsg->ValueExist(str3_, section_str_misc))
    //    customMsg->ValueSet(str3_, , section_str_misc);

    //Welcome section, 1.x = Variety messages (randomly chosen)
    if (!customMsg->ValueExist(str1_0, section_str_welcome))
        customMsg->ValueSet(str1_0, L"Welcome to Ash Clan Zombies", section_str_welcome);

    //Welcome section, 2.x = Variety gametype messages
    if (!customMsg->ValueExist(str2_0, section_str_welcome))
        customMsg->ValueSet(str2_0, L"The hill is a safezone! Use it for quick getaways!", section_str_welcome);

    //General section, 1.x = Variety messages for the commands section
    if (!customMsg->ValueExist(str1_0, section_str_general))
        customMsg->ValueSet(str1_0, L"disable", section_str_general);
    if (!customMsg->ValueExist(str1_1, section_str_general))
        customMsg->ValueSet(str1_1, L"enable", section_str_general);
    if (!customMsg->ValueExist(str1_2, section_str_general))
        customMsg->ValueSet(str1_2, L"Infection gametype", section_str_general);
    if (!customMsg->ValueExist(str1_3, section_str_general))
        customMsg->ValueSet(str1_3, L"red team", section_str_general);
    if (!customMsg->ValueExist(str1_4, section_str_general))
        customMsg->ValueSet(str1_4, L"blue team", section_str_general);
    if (!customMsg->ValueExist(str1_5, section_str_general))
        customMsg->ValueSet(str1_5, L"Zombie", section_str_general);
    if (!customMsg->ValueExist(str1_6, section_str_general))
        customMsg->ValueSet(str1_6, L"Human", section_str_general);
    if (!customMsg->ValueExist(str1_7, section_str_general))
        customMsg->ValueSet(str1_7, L"Last man", section_str_general);
    if (!customMsg->ValueExist(str1_8, section_str_general))
        customMsg->ValueSet(str1_8, L"forbidden", section_str_general);
    if (!customMsg->ValueExist(str1_9, section_str_general))
        customMsg->ValueSet(str1_9, L"permitted", section_str_general);
    if (!customMsg->ValueExist(str1_10, section_str_general))
        customMsg->ValueSet(str1_10, L"Infect by fall", section_str_general);
    if (!customMsg->ValueExist(str1_11, section_str_general))
        customMsg->ValueSet(str1_11, L"Infect by guardian", section_str_general);
    if (!customMsg->ValueExist(str1_12, section_str_general))
        customMsg->ValueSet(str1_12, L"Infect by suicide", section_str_general);
    if (!customMsg->ValueExist(str1_13, section_str_general))
        customMsg->ValueSet(str1_13, L"Infect by betrayal", section_str_general);
    if (!customMsg->ValueExist(str1_14, section_str_general))
        customMsg->ValueSet(str1_14, L"Invisible zombie crouching", section_str_general);

    /*if (!customMsg->ValueExist(str1_, section_str_general))
        customMsg->ValueSet(str1_, , section_str_general);*/

    //Commands section, 2.x = Variety messages combo
    if (!customMsg->ValueExist(str2_0, section_str_general))
        customMsg->ValueSet(str2_0, L"%s is currently %s.", section_str_general);
    if (!customMsg->ValueExist(str2_1, section_str_general))
        customMsg->ValueSet(str2_1, L"%s is now set to %s.", section_str_general);
    if (!customMsg->ValueExist(str2_2, section_str_general))
        customMsg->ValueSet(str2_2, L"%s is already %s.", section_str_general);
    if (!customMsg->ValueExist(str2_3, section_str_general))
        customMsg->ValueSet(str2_3, L"%s is currently on %s.", section_str_general);
    if (!customMsg->ValueExist(str2_4, section_str_general))
        customMsg->ValueSet(str2_4, L"%s is already on %s.", section_str_general);
    if (!customMsg->ValueExist(str2_5, section_str_general))
        customMsg->ValueSet(str2_5, L"%s is currently at %d second(s).", section_str_general);
    if (!customMsg->ValueExist(str2_6, section_str_general))
        customMsg->ValueSet(str2_6, L"%s is now set to %d second(s).", section_str_general);
    if (!customMsg->ValueExist(str2_7, section_str_general))
        customMsg->ValueSet(str2_7, L"%s is already set to %d second(s).", section_str_general);
    if (!customMsg->ValueExist(str2_8, section_str_general))
        customMsg->ValueSet(str2_8, L"%s is currently at x%f speed.", section_str_general);
    if (!customMsg->ValueExist(str2_9, section_str_general))
        customMsg->ValueSet(str2_9, L"%s is now set to x%f speed.", section_str_general);
    if (!customMsg->ValueExist(str2_10, section_str_general))
        customMsg->ValueSet(str2_10, L"%s is already set to x%f speed.", section_str_general);
    if (!customMsg->ValueExist(str2_11, section_str_general))
        customMsg->ValueSet(str2_11, L"%s team are currently %s to enter the vehicle.", section_str_general);
    if (!customMsg->ValueExist(str2_12, section_str_general))
        customMsg->ValueSet(str2_12, L"%s team are set %s to enter the vehicle.", section_str_general);
    if (!customMsg->ValueExist(str2_13, section_str_general))
        customMsg->ValueSet(str2_13, L"%s team are already set %s to enter the vehicle.", section_str_general);
    /*if (!customMsg->ValueExist(str2_, section_str_general))
        customMsg->ValueSet(str2_, , section_str_general);*/
    /*if (!customMsg->ValueExist(str1_, ))
        customMsg->ValueSet(str1_, , );
    if (!customMsg->ValueExist(str1_, ))
        customMsg->ValueSet(str1_, , );*/
    customMsg->Save();
    customMsg->Close();

    pICommand->Add(eao_infection_enable_str, eaoGametypeInfectionEnable, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_zombie_team_str, eaoGametypeInfectionZombieTeam, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_zombie_respawn_str, eaoGametypeInfectionZombieRespawn, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_human_respawn_str, eaoGametypeInfectionHumanRespawn, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_zombie_speed_str, eaoGametypeInfectionZombieSpeed, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_human_speed_str, eaoGametypeInfectionHumanSpeed, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_last_man_speed_str, eaoGametypeInfectionLastManSpeed, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_last_man_next_zombie_str, eaoGametypeInfectionLastManNextZombie, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_zombie_allow_in_vehicle_str, eaoGametypeInfectionZombieAllowInVehicle, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_human_allow_in_vehicle_str, eaoGametypeInfectionHumanAllowInVehicle, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_infect_on_fall_str, eaoGametypeInfectionInfectOnFall, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_infect_on_guardians_str, eaoGametypeInfectionInfectOnGuardians, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_infect_on_suicide_str, eaoGametypeInfectionInfectOnSuicide, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_infect_on_betrayal_str, eaoGametypeInfectionInfectOnBetrayal, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);
    pICommand->Add(eao_infection_zombie_invisible_on_crouch_str, eaoGametypeInfectionZombieInvisibleOnCrouch, eaoInfo.sectors.sect_name1, 1, 2, false, util::modeAll);

    haloOutput(0, "Infection GameType support added.");
    return EAOCONTINUE;
}
CNATIVE dllAPI void WINAPIC EXTOnEAOUnload() {
    Uninit();
    customMsg->Release();
    
    pICommand->Del(eaoGametypeInfectionEnable, eao_infection_enable_str);
    pICommand->Del(eaoGametypeInfectionZombieTeam, eao_infection_zombie_team_str);
    pICommand->Del(eaoGametypeInfectionZombieRespawn, eao_infection_zombie_respawn_str);
    pICommand->Del(eaoGametypeInfectionHumanRespawn, eao_infection_human_respawn_str);
    pICommand->Del(eaoGametypeInfectionZombieSpeed, eao_infection_zombie_speed_str);
    pICommand->Del(eaoGametypeInfectionHumanSpeed, eao_infection_human_speed_str);
    pICommand->Del(eaoGametypeInfectionLastManSpeed, eao_infection_last_man_speed_str);
    pICommand->Del(eaoGametypeInfectionLastManNextZombie, eao_infection_last_man_next_zombie_str);
    pICommand->Del(eaoGametypeInfectionZombieAllowInVehicle, eao_infection_zombie_allow_in_vehicle_str);
    pICommand->Del(eaoGametypeInfectionHumanAllowInVehicle, eao_infection_human_allow_in_vehicle_str);
    pICommand->Del(eaoGametypeInfectionInfectOnFall, eao_infection_infect_on_fall_str);
    pICommand->Del(eaoGametypeInfectionInfectOnGuardians, eao_infection_infect_on_guardians_str);
    pICommand->Del(eaoGametypeInfectionInfectOnSuicide, eao_infection_infect_on_suicide_str);
    pICommand->Del(eaoGametypeInfectionInfectOnBetrayal, eao_infection_infect_on_betrayal_str);
    pICommand->Del(eaoGametypeInfectionZombieInvisibleOnCrouch, eao_infection_zombie_invisible_on_crouch_str);
    haloOutput(0, "Infection GameType support removed.");
}
#pragma endregion


/*

Task list:

 * Need to clean up the oddballs/flags before the unload process, seems didn't clean up right or is missing. (Oh, it was commented out.)
 * Finish the commands + custom message for the command outputs.
 * What other things need to be fix?

Known Bugs:
 * Speed modifier does not work effectively for just loaded & turn on.
 * Navpoints does not show correctly due to client-side need H-Ext's fixed bug.
 * The player's FFA color does not sync to client's hud. (Need to research this later, very complex network packet to do.)

*/




#if 0
bool hasZombie = 0;
IObject::hTagHeader* ball = NULL;
IObject::hTagHeader* flag = NULL;
//expand for zombie gametype.
bool allowScoreCTF = 0;
bool gameStart = 0;
bool zombieEnable = 0;
bool invalidGameType = 0;
short maxPlayersIntialize = 2;
char countDown=0;
float maxShield = 0.0f;
float maxHealth = 600.0f;
float maxSpeed = 1.5f;
DWORD idTimer = -1;
bool human=0;
char lastChose = 0;
static wchar_t zombieWin[] = L"And the winner is...ZOMBIES!!! Congraduation!";
static wchar_t humanWin[] = L"And the winner is...HUMANS!!! Congraduation!";
static wchar_t zombieKill[] = L"%s has pulled %s into the graveyard.";
static wchar_t humanKill[] = L"%s has put %s back in the graveyard.";
static wchar_t disgraceKill[] = L"%s has been disgraced by kill %s.";
static wchar_t guardianKill[] = L"Guardian pulled %s into the graveyard.";
static wchar_t deathKill[] = L"%s has fallen into the graveyard.";
static wchar_t vehicleKill[] = L"%s has been run over by a vehicle.";
static wchar_t serverKill[] = L"%s has been killed by mother nature.";
static wchar_t welcomeMsg[] = L"Welcome to alpha version of Zombie Gametype, be aware this is not a final stage of development.";
static const wchar_t zombieStr[] = L"eao_gametype_zombie";
static char countDown10[] = "10", countDown5[] = "5", countDown4[] = "4",  countDown3[] = "3", countDown2[] = "2", countDown1[] = "1";


static addon::addonInfo eaoInfo = { L"Zombie GameType Add-on", L"1.0.0.0",
                            L"RadWolfie & Wizard",
                            L"It provide ability simulate a zombie gametype with almost any proper gametype provided.",
                            L"Zombie Gametype",
                            L"gametype",
                            NULL,
                            NULL,
                            NULL,
                            NULL};
CNATIVE dllAPI addon::addonInfo* EXTPluginInfo = &eaoInfo;

/*
struct timerStruct {
    DWORD idTimer;
    IPlayer::PlayerInfo plI;


};
util::dynamicStack<timerStruct> timerList;//*/

struct scoreBoard {
    bool zombie[16];
    scoreBoard() {
        zombie[0] = 0;
        zombie[1] = 0;
        zombie[2] = 0;
        zombie[3] = 0;
        zombie[4] = 0;
        zombie[5] = 0;
        zombie[6] = 0;
        zombie[7] = 0;
        zombie[8] = 0;
        zombie[9] = 0;
        zombie[10] = 0;
        zombie[11] = 0;
        zombie[12] = 0;
        zombie[13] = 0;
        zombie[14] = 0;
        zombie[15] = 0;
    }
};
scoreBoard stats = scoreBoard();
bool checkZombies() {
    char countZombies = 0;
    for (char i=0; i<16; i++) {
        if (stats.zombie[i]) countZombies++;
    }
    if (countZombies==0) {
        /*IPlayer::PlayerInfo plI;
        for (char i=0; i<16; i++) {
            plI = pIPlayer->getPlayerMindex(i);
            if (plI.plEx && plI.plEx->isInServer) {
                stats.zombie[i]=1;
                plI.plS->iTeam=TEAM_BLUE;
                plI.plS->Team=TEAM_BLUE;
                if (plI.plS->CurrentBiped.Tag!=-1)
                    pIObject->Delete(plI.plS->CurrentBiped);
                break;
                //TODO need to set as zombie.
            }
        }*/
    } else return 1;
    return 0;
}
short countPlayers() {
    char i = 0;
    short count = 0;
    IPlayer::PlayerInfo plI;
    for (i; i<16; i++) {
        plI = pIPlayer->getPlayerMindex(i);
        if (plI.plEx && plI.plEx->isInServer) count++;
    }
    return count;
}
void deleteBiped(ident& biped_Tag) {
    BipedS* pl_Biped = (BipedS*)pIObject->GetObjectAddress(3, biped_Tag);
    if (!pl_Biped) return;
    if (pl_Biped->sObject.Vehicle.Tag!=-1)
        pIObject->Eject(biped_Tag);
    if (pl_Biped->Equipments[0].Tag!=-1)
        pIObject->Delete(pl_Biped->Equipments[1]);
    if (pl_Biped->Equipments[1].Tag!=-1)
        pIObject->Delete(pl_Biped->Equipments[1]);
    if (pl_Biped->Equipments[2].Tag!=-1)
        pIObject->Delete(pl_Biped->Equipments[2]);
    if (pl_Biped->Equipments[3].Tag!=-1)
        pIObject->Delete(pl_Biped->Equipments[3]);
    pl_Biped->grenade0=0;
    pl_Biped->grenade1=0;
    pIObject->Delete(biped_Tag);
}
//TODO need make settings and wait system for ready to start gametype. Plus random chosen player to be zombie.
//TODO need to real-time modify for biped's color to take effect, the colors set is already done.
//TODO need map live up time for server lock.
//TODO need ban hook for server lock?
//TODO need assign gameobject as current and ability to drop it.
//TODO need add commands
//TODO need to check players is in vehicle to force eject before delete?
//TODO need to not to select same player for next game
//TODO what else?
void getOddballAndFlagHeader() {
    ball = pIObject->LookupTagTypeName("weap", "weapons\\ball\\ball");
    flag = pIObject->LookupTagTypeName("weap", "weapons\\flag\\flag");
}
void setZombie(IPlayer::PlayerInfo& plI) {
    stats.zombie[plI.plS->MachineIndex]=1;
    plI.plS->iTeam=TEAM_BLUE;
    plI.plS->Team=TEAM_BLUE;
    if (plI.plS->CurrentBiped.Tag!=-1)
        deleteBiped(plI.plS->CurrentBiped);
    IPlayer::PlayerInfo human;
    for (char i=0; i<16; i++) {
        human = pIPlayer->getPlayerMindex(i);
        if (human.plS && plI.plS->MachineIndex!=human.plS->MachineIndex) {
            human.plS->iTeam=TEAM_RED;
            human.plS->Team=TEAM_RED;
            if (human.plS->CurrentBiped.Tag!=-1)
                deleteBiped(human.plS->CurrentBiped);
        }
    }
}
void randomZombie() {
    IPlayer::PlayerInfo plI;
    char randNum = (*pIHaloEngine->mapStatus)->upTime % 15;
    short countPl = countPlayers();
    for (;;randNum++) {
        if (countPl>1 && randNum==lastChose)
            randNum++;
        if (randNum>15)
            randNum=0;
        plI = pIPlayer->getPlayerMindex(randNum);
        if (plI.plEx && plI.plEx->isInServer) {
            setZombie(plI);
            break;
        }
    }
}
void WINAPIC TimerEvent(DWORD id, void* param) {
    idTimer=-1;
    if (countDown>0 && maxPlayersIntialize<=countPlayers()) {
        countDown--;
        IPlayer::PlayerInfo plI;
        rconData cdRcon("");
        switch(countDown) {
            case 10: {
                cdRcon = rconData(countDown10);
                break;
            }
            case 5: {
                cdRcon = rconData(countDown5);
                break;
            }
            case 4: {
                cdRcon = rconData(countDown4);
                break;
            }
            case 3: {
                cdRcon = rconData(countDown3);
                break;
            }
            case 2: {
                cdRcon = rconData(countDown2);
                break;
            }
            case 1: {
                cdRcon = rconData(countDown1);
                break;
            }
            default: {
                goto skipCountDown;
            }
        }
        for (char i = 0; i<16; i++) {
            plI = pIPlayer->getPlayerMindex(i);
            if (plI.plEx && plI.plEx->isInServer)
                pIHaloEngine->DispatchRcon(cdRcon, plI);
        }
skipCountDown:
        idTimer = addon::pITimer->EXTAddOnTimerAdd(plI, 30);
    } else if (countDown==0 && countPlayers()>0) {
        gameStart=1;
        randomZombie();
    }
}
#endif
#if 0
toggle WINAPIC eaoGametypeZombie(IPlayer::PlayerInfo plI, util::ArgContainer& arg, char chatRconRemote, DWORD idTimer, bool* showChat) {
    toggle gameMode = util::StrToBooleanW(arg[1]);
    if (gameMode==1) {
        stats = scoreBoard();
        if (!ball)
            getOddballAndFlagHeader();
        zombieEnable=1;
        if (countPlayers()>=maxPlayersIntialize) {
            countDown=11;
            if (idTimer>-1)
                addon::pITimer->EXTAddOnTimerDelete(idTimer);
            idTimer = addon::pITimer->EXTAddOnTimerAdd(plI, 30);
        }
        /*for (char i=0; i<16; i++) {
            plI = pIPlayer->getPlayerMindex(i);
            if (plI.plS) {
                if (checkZombies()) {
                    plI.plS->iTeam=TEAM_RED;
                    plI.plS->Team=TEAM_RED;
                    if (plI.plS->CurrentBiped.Tag!=-1)
                        pIObject->Delete(plI.plS->CurrentBiped);
                }
            }
        }*/
    } else if (gameMode==0) {
        IPlayer::PlayerInfo plI;
        zombieEnable=0;
        if (gameStart) {
            gameStart=0;
            if (idTimer>-1)
                addon::pITimer->EXTAddOnTimerDelete(idTimer);
            for (char i=0; i<16; i++) {
                plI = pIPlayer->getPlayerMindex(i);
                if (plI.plS) {
                    plI.plS->iTeam=human?TEAM_RED:TEAM_BLUE;
                    plI.plS->Team=human?TEAM_RED:TEAM_BLUE;
                    plI.plS->ColorIndex=human?COLOR_RED:COLOR_BLUE;
                    if (plI.plS->CurrentBiped.Tag!=-1)
                        deleteBiped(plI.plS->CurrentBiped);
                    human = !human;
                }
            }
        }
    } else {
        haloOutput(0, "Error, must be boolean value to initatize the zombie gametype");
        return CMDFAIL;
    }
    return CMDSUCC;
}
#endif
#if 0
CNATIVE dllAPI void WINAPIC EXTOnPlayerJoin(IPlayer::PlayerInfo plI) { //TODO need add compatible for oddball gametype due to oddball assign cause crash.
    if (zombieEnable && !gameStart && countPlayers()>=maxPlayersIntialize && countDown==0) {
        countDown=11;
        idTimer = addon::pITimer->EXTAddOnTimerAdd(plI, 30);
    }
    if (!gameStart)
        return;
    else if (!ball) getOddballAndFlagHeader();
    chatData msg;
    msg.player=plI.plS->PlayerIndex;
    msg.type=3;
    msg.msg=welcomeMsg;
    pIHaloEngine->DispatchPlayer(msg, wcslen(welcomeMsg), plI);
    //checkZombies();
}
CNATIVE dllAPI void WINAPIC EXTOnPlayerQuit(IPlayer::PlayerInfo plI) {
    if (!gameStart || !plI.plS)
        return;
    deleteBiped(plI.plS->PreviousBiped);
    stats.zombie[plI.plS->MachineIndex] = 0;
    if (countPlayers()<2) {
        gameStart=0;
        countDown=0;
        for (char i=0; i<16; i++) {
            plI = pIPlayer->getPlayerMindex(i);
            if (plI.plS) {
                plI.plS->iTeam=human?TEAM_RED:TEAM_BLUE;
                plI.plS->Team=human?TEAM_RED:TEAM_BLUE;
                plI.plS->ColorIndex=human?COLOR_RED:COLOR_BLUE;
                if (plI.plS->CurrentBiped.Tag!=-1)
                    pIObject->Delete(plI.plS->CurrentBiped);
                human = !human;
            }
        }
        return;
    } else if (!checkZombies()) {
        randomZombie();
    }
}
#endif
#if 0
CNATIVE dllAPI bool WINAPIC EXTOnObjectInteraction(IPlayer::PlayerInfo plI, ident m_ObjId, ObjectS* objectStruct, IObject::hTagHeader* hTag) {
    if (!gameStart)
        return 1;
    else if (!ball) getOddballAndFlagHeader();
    if (!stats.zombie[plI.plS->MachineIndex]) {
        if (ball && ball->id==hTag->id && objectStruct->GameObject==-1)
            return 0;
    } else {
        if (ball && ball->id==hTag->id)
            return 1;
        if (flag && flag->id==hTag->id && allowScoreCTF && objectStruct->GameObject!=-1)
            return 1;
        return 0;
    }
    if (flag && flag->id==hTag->id)
        if (allowScoreCTF && objectStruct->GameObject!=-1)
            return 1;
        else
            return 0;
    return 1;
}
#endif
#if 0
CNATIVE dllAPI void WINAPIC EXTOnPlayerDeath(IPlayer::PlayerInfo killerI, IPlayer::PlayerInfo victimI, int mode, int& showMessage) {
    /*
     * Mode info
     * -1 = Don't show message.
     * 0 = custom kill with no message
     * 1 = fall damage or server kill
     * 2 = killed by guardians
     * 3 = killed by a vehicle
     * 4 = killed by another player
     * 5 = betrayed by same team
     * 6 = suicide
     */
    if (!gameStart)
        return;
    wchar_t* printMsg = new wchar_t[256];
    printMsg[0]=NULL;
    chatData msg;
    msg.player=victimI.plS->PlayerIndex;
    msg.type=3;
    msg.msg=NULL;
    if (!stats.zombie[victimI.plS->MachineIndex]) {//if is humans
        switch(mode) {
            case 0: {
                swprintf_s(printMsg, 256, serverKill, victimI.plS->Name);
                goto skipZombieChange;
            }
            case 1: {
                swprintf_s(printMsg, 256, deathKill, victimI.plS->Name);
                break;
            }
            case 2: {
                swprintf_s(printMsg, 256, guardianKill, victimI.plS->Name);
                break;
            }
            case 3: {
                swprintf_s(printMsg, 256, vehicleKill, victimI.plS->Name);
                goto skipZombieChange;
            }
            case 4: {
                if (killerI.plS && stats.zombie[killerI.plS->MachineIndex]) {//Zombie kills
                    swprintf_s(printMsg, 256, zombieKill, killerI.plS->Name, victimI.plS->Name);
                    break;
                } else {//Human kills
                    swprintf_s(printMsg, 256, disgraceKill, killerI.plS->Name, victimI.plS->Name);
                    goto skipZombieChange;
                }
            }
            case 5: {
                swprintf_s(printMsg, 256, disgraceKill, killerI.plS->Name, victimI.plS->Name);
                break;
            }
            case 6: {
                swprintf_s(printMsg, 256, deathKill, victimI.plS->Name);
                break;
            }
        }
    } else {
        BipedS* zombieBiped = (BipedS*)pIObject->GetObjectAddress(3, victimI.plS->CurrentBiped);
        zombieBiped->grenade0=0;
        zombieBiped->grenade1=0;
        IObject::hTagHeader* tagH;
        ObjectS* weapon = pIObject->GetObjectAddress(3, zombieBiped->Equipments[0]);
        if (weapon && weapon->GameObject==-1) {
            tagH = pIObject->LookupTag(weapon->ModelTag);
            if (tagH == ball)
                pIObject->Delete(zombieBiped->Equipments[0]);
        }
        weapon = pIObject->GetObjectAddress(3, zombieBiped->Equipments[1]);
        if (weapon && weapon->GameObject==-1) {
            tagH = pIObject->LookupTag(weapon->ModelTag);
            if (tagH == ball)
                pIObject->Delete(zombieBiped->Equipments[1]);
        }
        weapon = pIObject->GetObjectAddress(3, zombieBiped->Equipments[2]);
        if (weapon && weapon->GameObject==-1) {
            tagH = pIObject->LookupTag(weapon->ModelTag);
            if (tagH == ball)
                pIObject->Delete(zombieBiped->Equipments[2]);
        }
        weapon = pIObject->GetObjectAddress(3, zombieBiped->Equipments[3]);
        if (weapon && weapon->GameObject==-1) {
            tagH = pIObject->LookupTag(weapon->ModelTag);
            if (tagH == ball)
                pIObject->Delete(zombieBiped->Equipments[3]);
        }
        switch(mode) {
            case 0: {
                swprintf_s(printMsg, 256, serverKill, victimI.plS->Name);
                break;
            }
            case 1: {
                swprintf_s(printMsg, 256, deathKill, victimI.plS->Name);
                break;
            }
            case 2: {
                swprintf_s(printMsg, 256, guardianKill, victimI.plS->Name);
                break;
            }
            case 3: {
                swprintf_s(printMsg, 256, vehicleKill, victimI.plS->Name);
                break;
            }
            case 4: {
                swprintf_s(printMsg, 256, humanKill, killerI.plS->Name, victimI.plS->Name);
                break;
            }
            case 5: {
                swprintf_s(printMsg, 256, disgraceKill, killerI.plS->Name, victimI.plS->Name);
                break;
            }
            case 6: {
                swprintf_s(printMsg, 256, disgraceKill, killerI.plS->Name, victimI.plS->Name);
                break;
            }
        }
        goto skipZombieChange;
    }
    stats.zombie[victimI.plS->MachineIndex]=1;
    victimI.plS->iTeam=TEAM_BLUE;
    victimI.plS->Team=TEAM_BLUE;
    short sumZombies = 0;
    for (short i = 0; i<16; i++) {
        if (stats.zombie[i]) sumZombies++;
    }
    if (sumZombies==pIHaloEngine->globalServer->totalPlayers)
        pIHaloEngine->MapNext();
skipZombieChange:
    showMessage=0;
    short len = wcslen(printMsg);
    if (len) {
        msg.msg = printMsg;
        pIHaloEngine->DispatchGlobal(msg, wcslen(printMsg));
    }
    delete[] printMsg;
}
#endif
#if 0
CNATIVE dllAPI bool WINAPIC EXTOnPlayerChangeTeamAttempt(IPlayer::PlayerInfo plI, int team) {
    if (!gameStart)
        return 1;
    return 0;
}
CNATIVE dllAPI int WINAPIC EXTOnPlayerJoinDefault(MachineS* mS, int cur_team) {
    if (!gameStart)
        return -1;
    else if (checkZombies()) {
        return TEAM_RED;
    } else {
        stats.zombie[mS->machineIndex]=1;
        return TEAM_BLUE;
    }
}
CNATIVE dllAPI ident WINAPIC EXTOnWeaponAssignmentDefault(IPlayer::PlayerInfo plI, ident owningObjectId, IObject::objInfo* curWeapon, DWORD order, ident newWeaponId) {
    if (gameStart && plI.plS) {
        if (stats.zombie[plI.plS->MachineIndex]) {
            if (order==1) {
                newWeaponId.Tag=-1;
                return newWeaponId;
            }
            if (ball)
                newWeaponId.Tag=ball->id;
        }
    }
    return newWeaponId;
}
CNATIVE dllAPI ident WINAPIC EXTOnWeaponAssignmentCustom(IPlayer::PlayerInfo plI, ident owningObjectId, DWORD curWeapon, DWORD order, ident newWeaponId) {
    if (gameStart && plI.plS && stats.zombie[plI.plS->MachineIndex]) {
        newWeaponId.Tag=-1;
    }
    return newWeaponId;
}
CNATIVE dllAPI bool WINAPIC EXTOnPlayerScoreCTF(IPlayer::PlayerInfo plI, ident curWeapon, DWORD team, bool isGameObject) {
    if (gameStart && allowScoreCTF && isGameObject)
        return 1;
    return 0;
}
#endif
#if 0
CNATIVE dllAPI bool WINAPIC EXTOnPlayerDropObject(IPlayer::PlayerInfo plI, ident owningObjectId, BipedS* pl_Biped) {
    if (gameStart) {
        if (plI.plS) {
            WeaponS* weap = (WeaponS*)pIObject->GetObjectAddress(3, pl_Biped->Equipments[pl_Biped->sObject.Weapon.index]);
            if (stats.zombie[plI.plS->MachineIndex])// && weap && weap->sObject.GameObject==-1 && weap->sObject.ModelTag.Tag == ball->id)
                return 0;
        }
        if (gameStart && allowScoreCTF && (pl_Biped->sObject.Weapon.index==0 || pl_Biped->sObject.Weapon.index==1)) {
            //TODO: Need to go more depth here...
            return 1;
        }
    }
    return 1;
}
CNATIVE dllAPI void WINAPIC EXTOnPlayerSpawn(IPlayer::PlayerInfo plI, ident owningObjectId, BipedS* pl_Biped) {
    if (gameStart && plI.plS && stats.zombie[plI.plS->MachineIndex]) {
        pl_Biped->grenade0=0;
        pl_Biped->grenade1=0;
        if (maxSpeed)
            plI.plS->VelocityMultiplier=maxSpeed;
        if (maxHealth)
            pl_Biped->sObject.HealthMax=maxHealth;
        if (maxShield)
            pl_Biped->sObject.ShieldMax=maxShield;
    }
}
CNATIVE dllAPI bool WINAPIC EXTOnVehicleUserEntry(IPlayer::PlayerInfo plI) {
    if (!gameStart)
        return 1;
    //else if (plI.plS && !stats.zombie[plI.plS->MachineIndex])
        //return 1;
    rconData msg("Vehicle usage is not allow due to zombie unable to kill human in vehicle.");
    pIHaloEngine->DispatchRcon(msg, plI);
    return 0;
}
#endif
#if 0
CNATIVE dllAPI bool WINAPIC EXTOnPlayerSpawnColor(IPlayer::PlayerInfo plI, bool isTeamPlay) {
    if (!gameStart)
        return 1;
    else if (stats.zombie[plI.plS->MachineIndex] && plI.plS->ColorIndex!=COLOR_GRAY)
        plI.plS->ColorIndex=COLOR_GRAY;
    else if (!stats.zombie[plI.plS->MachineIndex] && plI.plS->ColorIndex!=COLOR_SAGE)
        plI.plS->ColorIndex=COLOR_SAGE;
    return 0;
}
#endif
#if 0
CNATIVE dllAPI void WINAPIC EXTOnEndGame(int mode) {
    if (gameStart && mode==2) {
        ball=NULL;
        flag=NULL;
        invalidGameType=0;
        chatData p;
        p.player = -1;
        p.type = 3;
        if (pIHaloEngine->gameTypeLive->isTeamPlay) {
            char sumZombies = 0;
            for (char i = 0; i<16; i++) {
                if (stats.zombie[i]) sumZombies++;
            }
            if (sumZombies<pIHaloEngine->globalServer->totalPlayers)
                p.msg=humanWin;
            else p.msg=zombieWin;
        }
        pIHaloEngine->DispatchGlobal(p, wcslen(p.msg));
        gameStart=0;
        //TODO add a message to see who won or something...
    }
}
CNATIVE dllAPI void WINAPIC EXTOnNewGame(wchar_t* mode) {
    getOddballAndFlagHeader();
    stats = scoreBoard();
    if (pIHaloEngine->gameTypeLive->GameStage==ODDBALL)
        invalidGameType=1;
    gameStart=0;
}
#endif
#if 0
CNATIVE dllAPI toggle WINAPIC EXTOnEAOLoad() {
    pICommand->Add(zombieStr, eaoGametypeZombie, eaoInfo.sectors.sect_name1, 2, 2, 0, util::modeHost);
    haloOutput(0, "Zombie GameType support added.");
    return EAOCONTINUE;
}

CNATIVE dllAPI void WINAPIC EXTOnEAOUnload() {
    pICommand->Del(eaoGametypeZombie, zombieStr);
    haloOutput(0, "Zombie GameType support removed.");
}


CNATIVE dllAPI void WINAPIC EXTOnTimerExecute(DWORD idTimer) {
    //idTimer=-1;
    if (countDown>0 && maxPlayersIntialize<=countPlayers()) {
        countDown--;
        IPlayer::PlayerInfo plI;
        rconData cdRcon("");
        switch(countDown) {
            case 10: {
                cdRcon = rconData(countDown10);
                break;
            }
            case 5: {
                cdRcon = rconData(countDown5);
                break;
            }
            case 4: {
                cdRcon = rconData(countDown4);
                break;
            }
            case 3: {
                cdRcon = rconData(countDown3);
                break;
            }
            case 2: {
                cdRcon = rconData(countDown2);
                break;
            }
            case 1: {
                cdRcon = rconData(countDown1);
                break;
            }
            default: {
                goto skipCountDown;
            }
        }
        for (char i = 0; i<16; i++) {
            plI = pIPlayer->getPlayerMindex(i);
            if (plI.plEx && plI.plEx->isInServer)
                pIHaloEngine->DispatchRcon(cdRcon, plI);
        }
skipCountDown:
        idTimer = addon::pITimer->EXTAddOnTimerAdd(plI, 30);
    } else if (countDown==0 && countPlayers()>0) {
        gameStart=1;
        randomZombie();
    }
}
CNATIVE dllAPI void WINAPIC EXTOnTimerCancel(DWORD idTimer) {
    /*for(util::dynamicStack<timerStruct>::iterator timer = timerList.begin(); timer!=NULL;timer++) {
        if (timer->idTimer==idTimer) {
            timerList.erase(timer);
            break;
        }
    }//*/
}
#endif
#include "..\Add-on API\expChecker.h"