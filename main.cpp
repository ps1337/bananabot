#include "util.h"
#include "mem.h"
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
unsigned int client_state_view_angle_x_offset = 0x8E98;
unsigned int client_state_view_angle_y_offset = client_state_view_angle_x_offset + 0x04;
uint64_t client_state_view_angle_x_addr = -1;
uint64_t client_state_view_angle_y_addr = -1;

unsigned int glow_manager_so_offset = 0x29e8980;
uint64_t glow_manager_ptr_addr = -1;
uint64_t glow_manager_addr = 0;
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

pid_t PID;

uint64_t addr_from_ptr(uint64_t ptr)
{
    unsigned char buffer[8];
    if (!Mem::readmem((void *)ptr, buffer, sizeof(buffer)))
    {
        std::cerr << "Couldn't read from " << std::hex << ptr << std::endl;
        exit(1);
    }
    ptr = *(uint64_t *)(&buffer);
    return ptr;
}

uint64_t addr_from_multilvl_ptr(uint64_t ptr, std::vector<unsigned int> offsets)
{
    unsigned char buffer[8];
    for (int i = 0; i < offsets.size(); i++)
    {
        ptr += offsets[i];
        if (!Mem::readmem((void *)ptr, buffer, sizeof(buffer)))
        {
            std::cerr << "Couldn't read from " << std::hex << ptr << std::endl;
            exit(1);
        }
        // don't dereference in last round
        if (i == offsets.size() - 1)
        {
            return ptr;
        }
        // dereference where buffer points to
        ptr = *(uint64_t *)(&buffer);
    }
    return ptr;
}

std::vector<unsigned char> readFromAddr(void *address, size_t size)
{
    unsigned char buffer[size] = {0};
    if (!Mem::readmem(address, buffer, size))
    {
        std::cerr << "Couldn't read from " << std::hex << address << std::endl;
        exit(1);
    }
    std::vector<unsigned char> res(buffer, buffer + size);
    return res;
}

bool writeToAddr(void *address, std::vector<unsigned char> buffer)
{
    return Mem::writemem(address, reinterpret_cast<unsigned char *>(buffer.data()), buffer.size());
}

bool getAddrs()
{
    if (PID <= 0)
    {
        return false;
    }
    uint64_t client_panorama_client_base = Mem::getModuleStart("client_panorama_client.so");
    std::cout << "client_panorama_client.so is @ " << std::hex << client_panorama_client_base << std::endl;

    uint64_t engine_client_base = Mem::getModuleStart("engine_client.so");
    std::cout << "engine_client.so is @ " << std::hex << engine_client_base << std::endl;

    player_base_addr = addr_from_multilvl_ptr(client_panorama_client_base, player_base_offsets);

    std::cout << "player_base is @ " << std::hex << player_base_addr << std::endl;
    player_health_addr = player_base_addr + player_base_health_offset;
    player_team_addr = player_base_addr + player_base_team_offset;

    player_is_on_floor_addr = player_base_addr + player_base_is_on_floor_offset;
    player_flash_time_addr = player_base_addr + player_base_flash_time_offset;

    player_base_location_x_addr = player_base_addr + player_base_location_x_offset;
    player_base_location_y_addr = player_base_addr + player_base_location_y_offset;
    player_base_location_z_addr = player_base_addr + player_base_location_z_offset;

    force_jump_addr = addr_from_multilvl_ptr(client_panorama_client_base, force_jump_offsets);
    std::cout << "force_jump is @ " << std::hex << force_jump_addr << std::endl;

    client_state_addr = addr_from_multilvl_ptr(engine_client_base, client_state_offsets);
    std::cout << "client_state is @ " << std::hex << client_state_addr << std::endl;
    client_state_view_angle_x_addr = client_state_addr + client_state_view_angle_x_offset;
    std::cout << "view_angle_x is @ " << std::hex << client_state_view_angle_x_addr << std::endl;
    client_state_view_angle_y_addr = client_state_addr + client_state_view_angle_y_offset;
    std::cout << "view_angle_y is @ " << std::hex << client_state_view_angle_y_addr << std::endl;

    glow_manager_ptr_addr = client_panorama_client_base + glow_manager_so_offset;
    std::cout << "glow_manager_ptr is @ " << std::hex << glow_manager_ptr_addr << std::endl;
    glow_manager_addr = addr_from_multilvl_ptr(glow_manager_ptr_addr, {0, 0});
    std::cout << "glow_manager is @ " << std::hex << glow_manager_addr << std::endl;

    auto tmp = readFromAddr((void *)(glow_manager_ptr_addr + glow_object_count_addr_offset), sizeof(int));

    return true;
}

std::vector<float> getPlayerLocation()
{
    auto x = readFromAddr((void *)(player_base_location_x_addr), sizeof(float));
    auto y = readFromAddr((void *)(player_base_location_y_addr), sizeof(float));
    auto z = readFromAddr((void *)(player_base_location_z_addr), sizeof(float));

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

std::vector<float> getEntityLocation(void *entity_base_addr)
{
    auto x = readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_x_offset), sizeof(float));
    auto y = readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_y_offset), sizeof(float));
    auto z = readFromAddr((void *)(entity_base_addr + entity_m_vec_origin_z_offset), sizeof(float));

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

void getPlayerAddrs()
{
    auto vec = readFromAddr((void *)(glow_manager_ptr_addr + glow_object_count_addr_offset), sizeof(int));
    // TODO value is cut
    int count = vec[0];

    auto x = getPlayerLocation();
    std::cout << "P: " << x[0] << " " << x[1] << " " << x[2] << std::endl;

    for (int i = 0; i < count; i++)
    {
        uint64_t ptr = glow_manager_addr + (glow_object_offset * i);
        uint64_t addr = addr_from_ptr(ptr);
        // empty entity slot
        if (addr == 0)
        {
            continue;
        }

        int entity_team = readFromAddr((void *)(addr + entity_team_offset), sizeof(int))[0];
        int entity_health = readFromAddr((void *)(addr + entity_health_offset), sizeof(int))[0];

        int own_team = readFromAddr((void *)(player_team_addr), sizeof(int))[0];

        if (entity_team != own_team && entity_health > 0)
        {
            auto f = getEntityLocation((void *)addr);
            std::cout << "* " << addr << " " << entity_team << " " << entity_health << "  " << f[0] << ":" << f[1] << ":" << f[2] << std::endl;
        }
    }
}

int main()
{
    std::cout << "CSO:GO 1337 BANANABOT STARTING" << std::endl;

    PID = Util::getPID();
    std::cout << "CSGO Process PID: " << PID << std::endl;

    getAddrs();
    getPlayerAddrs();

    bool bunnyhop = false;
    bool noflash = false;

    while (true)
    {
        if (bunnyhop && readFromAddr((void *)player_is_on_floor_addr, 1)[0] == 1)
        {
            writeToAddr((void *)force_jump_addr, {0x06});
        }

        if (noflash)
            writeToAddr((void *)player_flash_time_addr, {0, 0, 0, 0, 0, 0, 0, 0});

        usleep(200);
    }

    return 0;
}
