#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iostream>

class Util
{
public:
    static std::string exec(const char *cmd);
    static pid_t getPID();
};

#endif