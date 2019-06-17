#include "mem.h"
#include "util.h"
#include <sstream>

uint64_t Mem::getModuleStart(std::string module)
{
    uint64_t moduleStart = -1;
    std::string cmd = "cat /proc/" + std::to_string(Util::getPID()) + "/maps | grep " + module + " | head -n1 | cut -d '-' -f1";
    std::string output = Util::exec(cmd.c_str());
    if (output.length() == 0)
    {
        std::cerr << "Error getting CSGO memory map" << std::endl;
        exit(1);
    }
    std::stringstream ss;
    ss << std::hex << output.c_str();
    ss >> moduleStart;
    return moduleStart;
}

bool Mem::readmem(void *address, void *buf, size_t size)
{
    if (PID == 0 || size == 0)
        return false;

    iovec iovLocalAddressSpace;
    iovec iovRemoteAddressSpace;

    iovLocalAddressSpace.iov_base = buf;
    iovLocalAddressSpace.iov_len = size;

    iovRemoteAddressSpace.iov_base = address;
    iovRemoteAddressSpace.iov_len = size;

    ssize_t bytes_count_read = process_vm_readv(PID,
                                                &iovLocalAddressSpace,
                                                1,                      //Size of the local iovec array
                                                &iovRemoteAddressSpace, //Remote iovec array
                                                1,                      //Size of the remote iovec array
                                                0);                     //Flags, unused

    return (bytes_count_read == (ssize_t)size);
}

bool Mem::writemem(void *address, void *buf, size_t size)
{
    if (PID == 0 || size == 0)
        return false;

    iovec iovLocalAddressSpace;
    iovec iovRemoteAddressSpace;

    iovLocalAddressSpace.iov_base = buf;
    iovLocalAddressSpace.iov_len = size;

    iovRemoteAddressSpace.iov_base = address;
    iovRemoteAddressSpace.iov_len = size;

    ssize_t bytes_count_write = process_vm_writev(PID,
                                                  &iovLocalAddressSpace,  //Local iovec array
                                                  1,                      //Size of the local iovec array
                                                  &iovRemoteAddressSpace, //Remote iovec array
                                                  1,                      //Size of the remote iovec array
                                                  0);                     //Flags, unused

    return (bytes_count_write == (ssize_t)size);
}


uint64_t Mem::addr_from_ptr(uint64_t ptr)
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

uint64_t Mem::addr_from_multilvl_ptr(uint64_t ptr, std::vector<unsigned int> offsets)
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

std::vector<unsigned char> Mem::readFromAddr(void *address, size_t size)
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

bool Mem::writeToAddr(void *address, std::vector<unsigned char> buffer)
{
    return Mem::writemem(address, reinterpret_cast<unsigned char *>(buffer.data()), buffer.size());
}
