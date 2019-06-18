#include "game.h"
#include "mem.h"
#include "util.h"
#include <sstream>

bool Game::getAddrs()
{
    if (PID <= 0)
    {
        return false;
    }
    uint64_t client_panorama_client_base = Mem::getModuleStart("client_panorama_client.so");
    std::cout << "client_panorama_client.so is @ " << std::hex << client_panorama_client_base << std::endl;

    uint64_t engine_client_base = Mem::getModuleStart("engine_client.so");
    std::cout << "engine_client.so is @ " << std::hex << engine_client_base << std::endl;

    player_base_addr = Mem::addr_from_multilvl_ptr(client_panorama_client_base, player_base_offsets);

    std::cout << "player_base is @ " << std::hex << player_base_addr << std::endl;
    player_health_addr = player_base_addr + player_base_health_offset;
    player_team_addr = player_base_addr + player_base_team_offset;

    player_is_on_floor_addr = player_base_addr + player_base_is_on_floor_offset;
    player_flash_time_addr = player_base_addr + player_base_flash_time_offset;

    player_base_location_x_addr = player_base_addr + player_base_location_x_offset;
    player_base_location_y_addr = player_base_addr + player_base_location_y_offset;
    player_base_location_z_addr = player_base_addr + player_base_location_z_offset;

    force_jump_addr = Mem::addr_from_multilvl_ptr(client_panorama_client_base, force_jump_offsets);
    std::cout << "force_jump is @ " << std::hex << force_jump_addr << std::endl;

    client_state_addr = Mem::addr_from_multilvl_ptr(engine_client_base, client_state_offsets);
    std::cout << "client_state is @ " << std::hex << client_state_addr << std::endl;
    client_state_view_angle_x_addr = client_state_addr + client_state_view_angle_x_offset;
    std::cout << "view_angle_x is @ " << std::hex << client_state_view_angle_x_addr << std::endl;
    client_state_view_angle_y_addr = client_state_addr + client_state_view_angle_y_offset;
    std::cout << "view_angle_y is @ " << std::hex << client_state_view_angle_y_addr << std::endl;

    glow_manager_ptr_addr = client_panorama_client_base + glow_manager_so_offset;
    std::cout << "glow_manager_ptr is @ " << std::hex << glow_manager_ptr_addr << std::endl;
    glow_manager_addr = Mem::addr_from_multilvl_ptr(glow_manager_ptr_addr, {0, 0});
    std::cout << "glow_manager is @ " << std::hex << glow_manager_addr << std::endl;

    return true;
}

std::vector<float> Game::getPlayerLocation()
{
    auto x = Mem::readFromAddr((void *)(player_base_location_x_addr), sizeof(float));
    auto y = Mem::readFromAddr((void *)(player_base_location_y_addr), sizeof(float));
    auto z = Mem::readFromAddr((void *)(player_base_location_z_addr), sizeof(float));

    float result_x = 0;
    float result_y = 0;
    float result_z = 0;

    // convert bytes to float
    std::copy(reinterpret_cast<const char *>(&x[0]),
              reinterpret_cast<const char *>(&x[4]),
              reinterpret_cast<unsigned char *>(&result_x));

    std::copy(reinterpret_cast<const char *>(&y[0]),
              reinterpret_cast<const char *>(&y[4]),
              reinterpret_cast<unsigned char *>(&result_y));

    std::copy(reinterpret_cast<const char *>(&z[0]),
              reinterpret_cast<const char *>(&z[4]),
              reinterpret_cast<unsigned char *>(&result_z));

    return {result_x, result_y, result_z};
}

std::vector<float> Game::getEntityLocation(uint64_t entity_base_addr)
{
    auto x = Mem::readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_x_offset), sizeof(float));
    auto y = Mem::readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_y_offset), sizeof(float));
    auto z = Mem::readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_z_offset), sizeof(float));

    float result_x = 0;
    float result_y = 0;
    float result_z = 0;

    std::copy(reinterpret_cast<const char *>(&x[0]),
              reinterpret_cast<const char *>(&x[4]),
              reinterpret_cast<unsigned char *>(&result_x));

    std::copy(reinterpret_cast<const char *>(&y[0]),
              reinterpret_cast<const char *>(&y[4]),
              reinterpret_cast<unsigned char *>(&result_y));

    std::copy(reinterpret_cast<const char *>(&z[0]),
              reinterpret_cast<const char *>(&z[4]),
              reinterpret_cast<unsigned char *>(&result_z));

    return {result_x, result_y, result_z};
}

std::vector<PlayerInfo_t> Game::getEnemies()
{
    std::vector<PlayerInfo_t> res;
    // get amount of entities managed by the glow manager instance
    auto cnt_read = Mem::readFromAddr((void *)(glow_manager_ptr_addr + glow_object_count_addr_offset), sizeof(int));
    int count = cnt_read[0];

    // reset
    nearestEnemy.distance = 0.0;
    for (int i = 0; i < count; i++)
    {
        // get the pointer to the current entity
        uint64_t ptr = glow_manager_addr + (glow_object_offset * i);
        uint64_t addr = Mem::addr_from_ptr(ptr);
        // empty entity slot
        if (addr == 0)
        {
            continue;
        }

        int entity_team = Mem::readFromAddr((void *)(addr + entity_team_offset), sizeof(int))[0];
        int entity_health = Mem::readFromAddr((void *)(addr + entity_health_offset), sizeof(int))[0];

        int own_team = Mem::readFromAddr((void *)(player_team_addr), sizeof(int))[0];

        // only if it's a player entity that's alive
        if (entity_team != own_team && entity_health > 0)
        {
            auto location = Game::getEntityLocation(addr);
            PlayerInfo_t player_info(addr, entity_team, location, entity_health);
            res.push_back(player_info);
        }
    }
    return res;
}

bool Game::setAngle(float x, float y)
{
    // convert floats to bytes
    std::vector<unsigned char> x_buf(sizeof(x));
    std::vector<unsigned char> y_buf(sizeof(y));
    std::memcpy(x_buf.data(), &x, sizeof(x));
    std::memcpy(y_buf.data(), &y, sizeof(y));
    auto write_x = Mem::writeToAddr((void *)client_state_view_angle_x_addr, x_buf);
    auto write_y = Mem::writeToAddr((void *)client_state_view_angle_y_addr, y_buf);

    return write_x && write_y;
}