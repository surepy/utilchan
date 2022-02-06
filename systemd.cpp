#include "systemd.h"
#include <array>

// dbus proxy simple wrapper
dbusProxy::dbusProxy(std::string destination, std::string objectpath, std::string interface) : interface(interface)
{
    this->proxy = sdbus::createProxy(destination, objectpath);
    std::cout << destination << " - " << objectpath << " proxy at addr " << std::hex << &this->proxy << std::endl;
}

sdbus::Variant dbusProxy::getProperty(std::string prop)
{
    return proxy->getProperty(prop).onInterface(interface);
}

// systemd manager
SystemdManager::SystemdManager() : dbusProxy("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager")
{
    std::cout << "init systemd manager proxy" << std::endl;
}

void SystemdManager::unregisterAll() {
    proxy->unregister();
}

void SystemdManager::PowerOff()
{
    // use logind
    proxy->callMethod(proxy->createMethodCall(interface, "PowerOff"));
}

SystemdUnit SystemdManager::getUnit(std::string service)
{
    return SystemdUnit(this->GetUnit(service));
}

SystemdServiceUnit SystemdManager::GetUnitService(std::string service)
{
    return SystemdServiceUnit(this->GetUnit(service));
}

sdbus::ObjectPath SystemdManager::GetUnit(std::string service)
{
    auto GetUnitFn = proxy->createMethodCall(interface, "GetUnit");
    GetUnitFn << service;

    sdbus::ObjectPath retVal;
    proxy->callMethod(GetUnitFn) >> retVal;

    return retVal;
}

std::vector<SystemdManager::Units> SystemdManager::ListUnits()
{
    // another way: proxy->callMethod("ListUnits").onInterface(interface).storeResultsTo(ligma);
    auto ListUnitFn = proxy->createMethodCall(interface, "ListUnits");

    std::vector<Units> units;
    auto msg = proxy->callMethod(ListUnitFn);

    // tip: good to know this.
    std::string type, contents;
    msg.peekType(type, contents);

    msg >> units;

    return units;
}

std::vector<SystemdManager::Units> SystemdManager::ListUnitsByPatterns(std::vector<std::string> states, std::vector<std::string> patterns)
{
    auto ListUnitFn = proxy->createMethodCall(interface, "ListUnitsByPatterns");
    ListUnitFn << states;
    ListUnitFn << patterns;

    std::vector<Units> units;
    auto msg = proxy->callMethod(ListUnitFn);

    msg >> units;

    return units;
}


sdbus::ObjectPath SystemdManager::StartUnit(std::string name, std::string mode)
{
    auto StartUnitFn = proxy->createMethodCall(interface, "StartUnit");
    StartUnitFn << name;
    StartUnitFn << mode;

    sdbus::ObjectPath retVal;
    proxy->callMethod(StartUnitFn) >> retVal;

    return retVal;
}

sdbus::ObjectPath SystemdManager::RestartUnit(std::string name, std::string mode)
{
    auto StartUnitFn = proxy->createMethodCall(interface, "StartUnit");
    StartUnitFn << name;
    StartUnitFn << mode;

    sdbus::ObjectPath retVal;
    proxy->callMethod(StartUnitFn) >> retVal;

    return retVal;
}

SystemdUnit::SystemdUnit(sdbus::ObjectPath obj_path) : dbusProxy("org.freedesktop.systemd1", obj_path, "org.freedesktop.systemd1.Unit")
{
}
