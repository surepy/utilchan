#ifndef SYSTEMD_H
#define SYSTEMD_H
#define DBUS_PROPERTY(type, str) type str() { return this->getProperty(#str); }
#define DBUS_SIGNAL(name) void register##name(sdbus::signal_handler func) \
{ \
    proxy->registerSignalHandler(interface, #name, func); \
    proxy->finishRegistration(); \
}

#include <iostream>
#include <sdbus-c++/sdbus-c++.h>


// i wrap the wrapper
class dbusProxy {
public:
    dbusProxy(std::string destination, std::string objectpath, std::string interface);
    sdbus::Variant getProperty(std::string prop);
protected:
    std::unique_ptr<sdbus::IProxy> proxy;
    // todo fix lmao
    std::string interface;
};


class SystemdUnit : public dbusProxy {
public:
    SystemdUnit(sdbus::ObjectPath obj_path);

    // properties
    DBUS_PROPERTY(std::string, ActiveState);
};

// todo
class SystemdServiceUnit : public dbusProxy {
public:
    SystemdServiceUnit();
};


// systemd wrapper class communicating with dbus
//
// refrences:
// https://www.freedesktop.org/wiki/Software/systemd/dbus/
// https://github.com/Kistler-Group/sdbus-cpp/blob/master/docs/using-sdbus-c++.md
// https://stackoverflow.com/questions/43499880/how-to-extract-service-state-via-systemd-dbus-api

class SystemdManager : public dbusProxy
{
public:
    SystemdManager();

    // wrapped functions
    SystemdUnit getUnit(std::string service);

    // org.freedesktop.systemd1.Manager

    // property getters
    DBUS_PROPERTY(std::string, Version);
    DBUS_PROPERTY(std::string, Features);
    DBUS_PROPERTY(std::string, Tainted);

    // signal callbacks
    //void registerJobNew(sdbus::signal_handler func);
    DBUS_SIGNAL(JobNew);
    DBUS_SIGNAL(StartupFinished);
    void unregisterAll();

    // raw dbus functions that need processing.
    void PowerOff();
    sdbus::ObjectPath StartUnit(std::string name, std::string mode);

    // ListUnits(out a(ssssssouso) units)
    typedef sdbus::Struct<std::string, std::string, std::string, std::string, std::string,
                          std::string, sdbus::ObjectPath, uint32_t, std::string, sdbus::ObjectPath> Units;
    std::vector<Units> ListUnits();
private:
    sdbus::ObjectPath GetUnit(std::string service);
};


#endif // SYSTEMD_H
