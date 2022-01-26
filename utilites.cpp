#include "utilites.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <array>

Utilites::Utilites()
{
}

std::string Utilites::sysInfoStr() {
    struct sysinfo status;

    int ret_code = sysinfo(&status);

    if (ret_code != 0) {
        return ":warning: error: sysinfo() returned code " + std::to_string(ret_code) + " !";
    }

    return "up " + std::to_string(status.uptime) + ", " + std::to_string(status.freeram) + " bytes free out of " + std::to_string(status.totalram);
}

// pasted
std::string Utilites::exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::chrono::milliseconds Utilites::epochTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

std::string Utilites::addressToString(void *ptr)
{
    std::stringstream address;
    address << std::hex << ptr;
    return address.str();
}

std::vector<std::string> Utilites::splitString(std::string str)
{
    std::vector<std::string> result;

    if (str.length() == 0) {
        return result;
    }
    if (str.length() != 0 && str.find(" ") == std::string::npos) {
        result.push_back(str);
        return result;
    }

    size_t position = 0;

    while ((position = str.find(" ")) != std::string::npos) {
        std::string token = str.substr(0, position);

        if (token != "") {
            result.push_back(token);
        }

        str.erase(0, position + 1);
    }

    return result;
}


class Utilites::sys_info{
public:
    sys_info(struct sysinfo info) : status(info) {};

    struct sysinfo getStatus() { return status; }
private:
    struct sysinfo status;
};
