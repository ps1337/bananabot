#ifndef MEM_H
#define MEM_H

#include "globalsettings.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdint>
#include<vector>
#include <sys/uio.h> // iovec

class Mem
{
public:
    static uint64_t getModuleStart(std::string module);
    static bool readmem(void *address, void *buf, size_t size);
    static bool writemem(void *address, void *buf, size_t size);
    static uint64_t addr_from_ptr(uint64_t ptr);
    static uint64_t addr_from_multilvl_ptr(uint64_t ptr, std::vector<unsigned int> offsets);
    static std::vector<unsigned char> readFromAddr(void *address, size_t size);
    static bool writeToAddr(void *address, std::vector<unsigned char> buffer);
};

#endif