#include "util.h"
#include "mem.h"
#include "game.h"
#include "globalsettings.h"

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

// Settings
unsigned int player_base_so_offset = 0x214AEF0;
unsigned int player_base_ptr_offset = 0xC;
std::vector<unsigned int> player_base_offsets = {player_base_so_offset, player_base_ptr_offset};
uint64_t player_base_addr = -1;

unsigned int force_jump_so_offset = 0x029F3C18;
unsigned int force_jump_ptr_offset = 0x110;
std::vector<unsigned int> force_jump_offsets = {force_jump_so_offset, force_jump_ptr_offset};
uint64_t force_jump_addr = -1;

unsigned int client_state_so_offset = 0x00E20D08;
unsigned int client_state_ptr_offset = 0x08;
std::vector<unsigned int> client_state_offsets = {client_state_so_offset, client_state_ptr_offset};
uint64_t client_state_addr = -1;
// x and y coordinates are swapped in memory
unsigned int client_state_view_angle_y_offset = 0x8E98;
unsigned int client_state_view_angle_x_offset = client_state_view_angle_y_offset + 0x04;
uint64_t client_state_view_angle_x_addr = -1;
uint64_t client_state_view_angle_y_addr = -1;

unsigned int glow_manager_so_offset = 0x29e8980;
uint64_t glow_manager_ptr_addr = -1;
uint64_t glow_manager_addr = -1;
unsigned int glow_object_offset = 0x40;
unsigned int glow_object_count_addr_offset = 0x10;
unsigned int glow_object_count = -1;

// members of player_base
unsigned int player_base_health_offset = 0x12c;
uint64_t player_health_addr = -1;
unsigned int player_base_team_offset = 0x124;
uint64_t player_team_addr = -1;
unsigned int player_base_is_on_floor_offset = 0x130;
uint64_t player_is_on_floor_addr = -1;
unsigned int player_base_flash_time_offset = 0xACF8;
uint64_t player_flash_time_addr = -1;

unsigned int player_base_location_x_offset = 0xCC;
unsigned int player_base_location_z_offset = 0xD0;
unsigned int player_base_location_y_offset = 0xD4;
uint64_t player_base_location_x_addr = -1;
uint64_t player_base_location_y_addr = -1;
uint64_t player_base_location_z_addr = -1;

unsigned int entity_team_offset = 0x130;
unsigned int entity_health_offset = 0x138;
unsigned int entity_m_vec_origin_start = 0xE0;
unsigned int entity_m_vec_origin_z_offset = entity_m_vec_origin_start + 0x08;
unsigned int entity_m_vec_origin_y_offset = entity_m_vec_origin_start;
unsigned int entity_m_vec_origin_x_offset = entity_m_vec_origin_start + 0x04;

// these values are calculated in every iteration of the main function
// to always have correct values
std::vector<float> own_location = {-1, -1, -1};
PlayerInfo_t nearestEnemy(0, 0, {0}, 0);

pid_t PID;

int main()
{
    std::cout << "*PENG PENG* CS:GO 1337 BANANABOT STARTING" << std::endl;

    PID = Util::getPID();
    std::cout << "CSGO Process PID: " << PID << std::endl;

    Game::getAddrs();

    bool bunnyhop = false;
    bool noflash = true;
    bool aimbot = true;

    while (true)
    {
        own_location = Game::getPlayerLocation();
        if (aimbot)
        {
            // this also sets `nearestEnemy`
            auto enemies = Game::getEnemies();
            if (nearestEnemy.distance != 0.0)
            {
                // calculate angle to aim at the nearest enemy
                auto angle = MadMath::calcAngle(own_location, nearestEnemy.location);
                //std::cout << "Aiming @ " << angle[1] << "/" << angle[0] << " (" << nearestEnemy.location[0] << "/" << nearestEnemy.location[1] << "/" << nearestEnemy.location[2] << ")" << std::endl;
                //std::cout << "Own location: " << own_location[0] << "/" << own_location[1] << "/" << own_location[2] << std::endl << std::endl;
                if (!(std::isnan(angle[0]) || std::isnan(angle[1])))
                {
                    // x and y are swapped in memory
                    if (!Game::setAngle(angle[1], angle[0]))
                    {
                        std::cerr << "Couldn't set angle to " << angle[1] << "/" << angle[0] << std::endl;
                    }
                }
                else
                {
                    std::cerr << "WARNING: Angle IsNAN" << std::endl;
                }
            }
        }

        if (bunnyhop && Mem::readFromAddr((void *)player_is_on_floor_addr, 1)[0] == 1)
        {
            // 0x06 --> Jump for one frame and reset afterwards
            Mem::writeToAddr((void *)force_jump_addr, {0x06});
        }

        if (noflash)
        {
            // overwrite remaining flash time and flash alpha value
            Mem::writeToAddr((void *)player_flash_time_addr, {0, 0, 0, 0, 0, 0, 0, 0});
        }

        usleep(150);
    }

    return 0;
}
