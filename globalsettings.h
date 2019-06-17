#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include "game.h"
#include <unistd.h>
#include <cstdint>
#include <vector>

extern pid_t PID;

extern std::vector<float> own_location;
struct PlayerInfo_t;
extern PlayerInfo_t nearestEnemy;



// Settings
extern unsigned int player_base_so_offset;
extern unsigned int player_base_ptr_offset;
extern std::vector<unsigned int> player_base_offsets;
extern uint64_t player_base_addr;

extern unsigned int force_jump_so_offset;
extern unsigned int force_jump_ptr_offset;
extern std::vector<unsigned int> force_jump_offsets;
extern uint64_t force_jump_addr;

extern unsigned int client_state_so_offset;
extern unsigned int client_state_ptr_offset;
extern std::vector<unsigned int> client_state_offsets;
extern uint64_t client_state_addr;
extern unsigned int client_state_view_angle_x_offset;
extern unsigned int client_state_view_angle_y_offset;
extern uint64_t client_state_view_angle_x_addr;
extern uint64_t client_state_view_angle_y_addr;

extern unsigned int glow_manager_so_offset;
extern uint64_t glow_manager_ptr_addr;
extern uint64_t glow_manager_addr;
extern unsigned int glow_object_offset;
extern unsigned int glow_object_count_addr_offset;
extern unsigned int glow_object_count;

// members of player_base
extern unsigned int player_base_health_offset;
extern uint64_t player_health_addr;
extern unsigned int player_base_team_offset;
extern uint64_t player_team_addr;
extern unsigned int player_base_is_on_floor_offset;
extern uint64_t player_is_on_floor_addr;
extern unsigned int player_base_flash_time_offset;
extern uint64_t player_flash_time_addr;

extern unsigned int player_base_location_x_offset;
extern unsigned int player_base_location_z_offset;
extern unsigned int player_base_location_y_offset;
extern uint64_t player_base_location_x_addr;
extern uint64_t player_base_location_y_addr;
extern uint64_t player_base_location_z_addr;

extern unsigned int entity_team_offset;
extern unsigned int entity_health_offset;
extern unsigned int entity_m_vec_origin_start;
extern unsigned int entity_m_vec_origin_z_offset;
extern unsigned int entity_m_vec_origin_y_offset;
extern unsigned int entity_m_vec_origin_x_offset;

#endif