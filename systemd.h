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


// i wrap the wrapper and become the wrapper
class dbusProxy {
public:
    dbusProxy(std::string destination, std::string objectpath, std::string interface);
    sdbus::Variant getProperty(std::string prop);
protected:
    std::unique_ptr<sdbus::IProxy> proxy;
    std::string interface;
};

// org.freedesktop.systemd1.Unit
class SystemdUnit : public dbusProxy {
public:
    static constexpr std::string_view classInterface = "org.freedesktop.systemd1.Unit";

    SystemdUnit(sdbus::ObjectPath obj_path);

    sdbus::ObjectPath Start(std::string mode);
    sdbus::ObjectPath Stop(std::string mode);
    sdbus::ObjectPath Reload(std::string mode);

    // properties
    DBUS_PROPERTY(std::string, ActiveState);
};

// REFACTOR NEEDED! we
class SystemdServiceUnit : public SystemdUnit {
public:
    static constexpr std::string_view classInterface = "org.freedesktop.systemd1.Service";

    SystemdServiceUnit(sdbus::ObjectPath obj_path) : SystemdUnit(obj_path) {
        // "hack" is an understatement.
        interface = "org.freedesktop.systemd1.Service";
    };

    DBUS_PROPERTY(std::string, StatusText);
    // doesnt work idk
    DBUS_PROPERTY(int, ExecMainStartTimestamp);
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
    // org.freedesktop.systemd1.Manager
    SystemdManager();

    // wrapped and raw dbus functions, unimplemented are commented, will likely not get impplemented.
    SystemdUnit getUnit(std::string service);
    SystemdServiceUnit GetUnitService(std::string service);

    // GetUnitByPID
    // GetUnitByInvocationID
    // GetUnitByControlGroup
    // LoadUnit
    sdbus::ObjectPath StartUnit(std::string name, std::string mode);
    // StartUnitWithFlags
    // StartUnitReplace
    // StopUnit
    // ReloadUnit
    sdbus::ObjectPath RestartUnit(std::string name, std::string mode);
    // TryRestartUnit
    // ReloadOrRestartUnit
    // ReloadOrTryRestartUnit
    // EnqueueUnitJob
    // KillUnit
    // CleanUnit
    // FreezeUnit
    // ThawUnit
    // ResetFailedUnit
    // SetUnitProperties
    // BindMountUnit
    // MountImageUnit
    // RefUnit
    // UnrefUnit
    // StartTransientUnit
    typedef sdbus::Struct<std::string, uint64_t, std::string> Process;
    // GetUnitProcesses
    // AttachProcessesToUnit
    // AbandonScope(in  s name);
    // GetJob
    // GetJobAfter
    // GetJobBefore
    // CancelJob
    // ClearJobs();
    // ResetFailed();
    // SetShowStatus(in  s mode);
    typedef sdbus::Struct<std::string, std::string, std::string, std::string, std::string,
                          std::string, sdbus::ObjectPath, uint32_t, std::string, sdbus::ObjectPath> Units;
    std::vector<Units> ListUnits();
    // ListUnitsFiltered
    std::vector<Units> ListUnitsByPatterns(std::vector<std::string> states, std::vector<std::string> patterns);
    // std::vector<Units> ListUnitsByNames(std::string names);
    // ListJobs(out a(usssoo) jobs);
    // Subscribe();
    // Unsubscribe();
    // Dump(out s output);
    // DumpByFileDescriptor(out h fd);
    // Reload();
    // Reexecute();
    // Exit();
    // Reboot();
    void PowerOff();
    // Halt();
    // KExec();
    // SwitchRoot
    // SetEnvironment(in  as assignments);
    // UnsetEnvironment(in  as names);
    // UnsetAndSetEnvironment
    // EnqueueMarkedJobs(out ao jobs);
    // ListUnitFiles(out a(ss) unit_files);
    // ListUnitFilesByPatterns
    // GetUnitFileState
    // EnableUnitFiles
    // DisableUnitFiles
    // EnableUnitFilesWithFlags
    // DisableUnitFilesWithFlags
    // ReenableUnitFiles
    // LinkUnitFiles
    // PresetUnitFiles
    // PresetUnitFilesWithMode
    // MaskUnitFiles
    // UnmaskUnitFiles
    // RevertUnitFiles
    // SetDefaultTarget
    // GetDefaultTarget(out s name);
    // PresetAllUnitFiles
    // AddDependencyUnitFiles
    // GetUnitFileLinks
    // SetExitCode
    // LookupDynamicUserByName
    // LookupDynamicUserByUID
    // GetDynamicUsers

    // property getters
    DBUS_PROPERTY(std::string, Version);
    DBUS_PROPERTY(std::string, Features);
    DBUS_PROPERTY(std::string, Tainted);

    // signal callbacks
    //void registerJobNew(sdbus::signal_handler func);
    DBUS_SIGNAL(JobNew);
    DBUS_SIGNAL(StartupFinished);
    void unregisterAll();


private:
    sdbus::ObjectPath GetUnit(std::string service);
};


#endif // SYSTEMD_H
