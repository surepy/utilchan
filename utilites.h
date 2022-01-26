#ifndef UTILITES_H
#define UTILITES_H

#include <iostream>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include "sys/sysinfo.h"

class Utilites
{
public:
    class sys_info;

    Utilites();
    static std::string sysInfoStr();
    static std::string exec(const char* cmd);
    static std::chrono::milliseconds epochTime();
    static std::string addressToString(void* ptr);
    static std::vector<std::string> splitString(std::string);


    // format std::string, printf style.
    // pasted from https://stackoverflow.com/a/26221725
    // LICENSE: CCO 1.0, by iFreilicht.
    // infinite <3
    template<typename ... Args>
    static std::string string_format( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        auto buf = std::make_unique<char[]>( size );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }
};

#endif // UTILITES_H
