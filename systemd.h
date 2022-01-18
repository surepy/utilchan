#ifndef SYSTEMD_H
#define SYSTEMD_H
#define DBUS_PROPERTY(type, str) type str() { return this->getProperty(#str); }

#include <iostream>
#include <sdbus-c++/sdbus-c++.h>


// i wrap the wrapper
class dbusProxy {
public:
    dbusProxy(std::string destination, std::string objectpath);
    sdbus::Variant getProperty(std::string prop);
protected:
    std::unique_ptr<sdbus::IProxy> proxy;
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
    void registerJobNew(sdbus::signal_handler func);

    // property getters
    DBUS_PROPERTY(std::string, Version);
    DBUS_PROPERTY(std::string, Features);
    DBUS_PROPERTY(std::string, Tainted);


    // raw dbus functions that need processing.
private:
    sdbus::ObjectPath GetUnit(std::string service);
};


#endif // SYSTEMD_H
