#include "util.h"
#include <array>
#include <memory>

std::string Util::exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

pid_t Util::getPID()
{
    // enterprise ready
    std::string output = Util::exec("ps aux | grep steam | grep csgo | grep -v grep | awk '{print $2}'");
    if (output.length() == 0)
    {
        std::cerr << "Error getting CSGO PID" << std::endl;
        exit(1);
    }
    pid_t PID = (pid_t)atoi(output.c_str());
    return PID;
}