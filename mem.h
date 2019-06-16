#ifndef MEM_H
#define MEM_H

#include "globalsettings.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdint>
#include <sys/uio.h> // iovec

class Mem
{
public:
    static uint64_t getModuleStart(std::string module);
    static bool readmem(void *address, void *buf, size_t size);
    static bool writemem(void *address, void *buf, size_t size);
};

#endif