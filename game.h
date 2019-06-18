#ifndef GAME_H
#define GAME_H

#include "globalsettings.h"
#include "madmath.h"
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

struct PlayerInfo_t;
extern PlayerInfo_t nearestEnemy;
extern std::vector<float> own_location;

// structure to manage the player information that's required for the aimbot
struct PlayerInfo_t
{
    uint64_t addr;
    int team;
    std::vector<float> location;
    int health;
    // distance from own position on the map
    float distance;

    PlayerInfo_t(uint64_t addr, int team, std::vector<float> location, int health) : addr(addr), team(team), location(location), health(health)
    {
        distance = MadMath::get3DDistance(own_location[0], own_location[1], own_location[2], location[0], location[1], location[2]);

        // value has been reset --> set initial value for current round
        if (nearestEnemy.distance == 0)
        {
            nearestEnemy = *this;
        }
        else
        {
            if (distance < nearestEnemy.distance)
            {
                nearestEnemy = *this;
            }
        }
    }
};

class Game
{
public:
    static bool getAddrs();
    static std::vector<float> getPlayerLocation();
    static std::vector<float> getEntityLocation(void *entity_base_addr);
    static std::vector<PlayerInfo_t> getEnemies();
    static bool setAngle(float x, float y);
};

#endif