#include "systemd.h"

// dbus proxy simple wrapper
dbusProxy::dbusProxy(std::string destination, std::string objectpath)
{
    this->proxy = sdbus::createProxy(destination, objectpath);
    std::cout << destination << " - " << objectpath << " proxy at addr " << std::hex << &this->proxy << std::endl;
}

sdbus::Variant dbusProxy::getProperty(std::string prop)
{
    return proxy->getProperty("Version").onInterface("org.freedesktop.systemd1.Manager");
}

// systemd manager
SystemdManager::SystemdManager() : dbusProxy("org.freedesktop.systemd1", "/org/freedesktop/systemd1")
{
    std::cout << "init systemd manager proxy" << std::endl;
}

void SystemdManager::registerJobNew(sdbus::signal_handler func)
{
    proxy->registerSignalHandler("org.freedesktop.systemd1.Manager", "JobNew", func);
    proxy->finishRegistration();
}


SystemdUnit SystemdManager::getUnit(std::string service)
{
    return SystemdUnit(this->GetUnit(service));
}

sdbus::ObjectPath SystemdManager::GetUnit(std::string service)
{
    auto GetUnitFn = proxy->createMethodCall("org.freedesktop.systemd1.Manager", "GetUnit");
    GetUnitFn << service;

    sdbus::ObjectPath retVal;
    proxy->callMethod(GetUnitFn) >> retVal;

    return retVal;
}


SystemdUnit::SystemdUnit(sdbus::ObjectPath obj_path) : dbusProxy("org.freedesktop.systemd1", obj_path)
{
    //
}
