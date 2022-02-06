#include "utilchanbot.h"
#include "utilites.h"
#include <sys/utsname.h>
#include <iostream>

int ligma(UtilChanBot* bot, SleepyDiscord::Message message) {
    bot->sendMessage(message.channelID, "ligma");
    return 1;
}

// don't do this
int execbadidea(UtilChanBot* bot, SleepyDiscord::Message message) {
    if (!bot->isElavatedUser(message.author)) {
        bot->sendMessage(message.channelID, ":octagonal_sign: you don't have the permissions to do this!");
        return 1;
    }

    bot->sendMessage(message.channelID, "executing: ``" + message.content + "``");

    bot->sendMessage(message.channelID, "```\n" + Utilites::exec(message.content.c_str()) + "\n```");
    return 0;
}

int systemd_info(UtilChanBot* bot, SleepyDiscord::Message message) {
    if (!bot->isElavatedUser(message.author)) {
        bot->sendMessage(message.channelID, ":octagonal_sign: you don't have the permissions to do this!");
        return 1;
    }

    bot->sendMessage(
                message.channelID,
                "Version: ``" + bot->getSystemdManager()->Version() + "``\n" +
                "Features: ``" + bot->getSystemdManager()->Features() + "``\n" +
                "Tainted: ``" + bot->getSystemdManager()->Tainted() + "``\n"
    );
    return 0;
}

// this doesn't work
int turn_off_pc(UtilChanBot* bot, SleepyDiscord::Message message) {
    if (!bot->isElavatedUser(message.author)) {
        bot->sendMessage(message.channelID, ":octagonal_sign: you don't have the permissions to do this!");
        return 1;
    }

    try {
        bot->sendMessage(
                    message.channelID,
                    ":zzz: gooodnight."
        );

        bot->getSystemdManager()->PowerOff();
    }
    catch (const sdbus::Error &e) {
        bot->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return 0;
}

int test(UtilChanBot* bot, SleepyDiscord::Message message) {
    if (!bot->isElavatedUser(message.author)) {
        bot->sendMessage(message.channelID, ":octagonal_sign: you don't have the permissions to do this!");
        return 1;
    }

    /*
    struct sysinfo info;
    bot->callApi(sysinfo, &info);

    bot->sendMessage(
                message.channelID,
                std::to_string(info.uptime)
    );
    */

    try {
        std::vector<SystemdManager::Units> units = bot->getSystemdManager()->ListUnits();

        std::string result = "";

        for (size_t i = 0; i < units.size(); ++i) {

            std::string result_2 = result + std::to_string(i) + " " + units.at(i).get<0>() + "\n";

            if (result_2.size() >= 2000) {
                break;
            }

            result = result_2;
        }

        bot->sendMessage(
                    message.channelID,
                    result
        );
    }
    catch (const sdbus::Error &e) {
        bot->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return 0;
}

#define REGISTER_COMMAND_NAME(name) registeredCommands.insert(std::pair<std::string, MessageCallbackFn>(#name, &UtilChanBot::name));

UtilChanBot::UtilChanBot() : SleepyDiscord::DiscordClient(API_KEY, SleepyDiscord::USER_CONTROLED_THREADS)
{
    //MessageCallbackFn fun = [](UtilChanBot* bot, SleepyDiscord::Message message) -> int { return 1; } ;
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("registered_cmd", ligma));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("exec", execbadidea));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("systemd_info", systemd_info));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("test", test));

    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("shutdown", turn_off_pc));

    // commands i would actually use (implemented)
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("sayyo", &UtilChanBot::ReplyTest));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("start_unit", &UtilChanBot::StartSystemdUnit));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("unit_active_state", &UtilChanBot::SystemdUnitActiveState));

    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("uptime", &UtilChanBot::uptime));

    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("list_units", &UtilChanBot::SystemdListUnits));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("info", &UtilChanBot::info));
    registeredCommands.insert(std::pair<std::string, MessageCallbackFn>("help", &UtilChanBot::help));

    REGISTER_COMMAND_NAME(mc_server_status);
    REGISTER_COMMAND_NAME(mc_server_list);
    REGISTER_COMMAND_NAME(mc_start_server);
    REGISTER_COMMAND_NAME(mc_stop_server);


    systemdManager = new SystemdManager();
}

bool UtilChanBot::isElavatedUser(SleepyDiscord::User &usr) {
    if (usr.ID == "449284815976529940") {
        return true;
    }

    return false;
}

int UtilChanBot::ReplyTest(SleepyDiscord::Message message)
{
    this->sendMessage(message.channelID, "yo");

    return CALLBACK_RESPONSE::OK;
}

int UtilChanBot::StartSystemdUnit(SleepyDiscord::Message message)
{
    if (!this->isElavatedUser(message.author)) {
        return CALLBACK_RESPONSE::INVALID_PERMISSIONS;
    }

    std::vector<std::string> arguments = Utilites::splitString(message.content);

    if (arguments.size() <= 0) {
        this->sendMessage(
                    message.channelID,
                    "not enough arguments.\n``start_unit <job> <?mode=replace>``\n\t``<job>`` = a systemd job.\n\
\t``<mode>`` = unit mode string.\n\
- replace = will start the unit and its dependencies, possibly replacing already queued jobs that conflict with this.\n\
- fail = will start the unit and its dependencies, but will fail if this would change an already queued job.\n\
- isolate = will start the unit in question and terminate all units that aren't dependencies of it.\n\
- ignore-dependencies = will start a unit but ignore all its dependencies\n\
- ignore-requirements = will start a unit but only ignore the requirement dependencies\n\
  It is not recommended to make use of the latter two options."
                    );
        return CALLBACK_RESPONSE::OK;
    }

    try {
        std::string return_value = this->getSystemdManager()->StartUnit(arguments.at(0), arguments.size() > 2 ? arguments.at(1) : "replace");

        this->sendMessage(
                    message.channelID,
                    Utilites::string_format(
                        "starting unit %s. (object at %s)",
                        arguments.at(0).c_str(),
                        return_value.c_str()
                    )
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return CALLBACK_RESPONSE::OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::SystemdUnitActiveState(SleepyDiscord::Message message)
{
    std::vector<std::string> arguments = Utilites::splitString(message.content);

    if (arguments.size() <= 0) {
        this->sendMessage(
                    message.channelID,
                    "not enough arguments.\n``unit_active_state <job>"
                    );
        return OK;
    }

    try {
        SystemdUnit dbus = this->getSystemdManager()->getUnit(message.content);

        this->sendMessage(
                    message.channelID,
                    dbus.ActiveState()
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::SystemdListUnits(SleepyDiscord::Message message)
{
    std::vector<std::string> arguments = Utilites::splitString(message.content);

    int offset = arguments.size() > 0 ? std::stoi(arguments.at(0)) : 0;

    try {
        std::vector<SystemdManager::Units> units = this->getSystemdManager()->ListUnits();

        //
        std::string result = "";

        this->sendMessage(message.channelID, Utilites::string_format("starting from offset %d", offset));

        size_t i = offset;
        for (; i < units.size(); ++i) {
            std::string result_2 = result + "``" + std::to_string(i) + "`` " + units.at(i).get<0>() + "\n";

            if (result_2.size() >= 1900) {
                break;
            }

            result = result_2;
        }

        result += "read ``" + std::to_string(i) + "`` / ``" + std::to_string(units.size()) + "``";

        this->sendMessage(
                    message.channelID,
                    result
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::help(SleepyDiscord::Message message)
{
    std::string commands = "help | registered commands: \n";

    for (auto i = registeredCommands.begin(); i != registeredCommands.end(); i++) {
        commands += "``" + i->first + "`` @ ``" + Utilites::addressToString(&i->second) + "``\n";
    }

    this->sendMessage(message.channelID, commands);

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::info(SleepyDiscord::Message message)
{
    struct utsname info;
    this->callApi(uname, &info);

    this->sendMessage(
                message.channelID,
                Utilites::string_format(
                    "utilchan by sleepy :D\n\nrunning at ``%s`` powered by ``%s %s %s %s``\nSystemd version ``%s``\n\nSystemd Features ``%s`` (proxyed ``%s``)",
                    info.nodename,
                    info.sysname,
                    info.release,
                    info.version,
                    info.machine,
                    systemdManager->Version().c_str(),
                    systemdManager->Features().c_str(),
                    Utilites::addressToString(systemdManager).c_str()
                    )
    );

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::uptime(SleepyDiscord::Message message)
{
    struct sysinfo info;
    this->callApi(sysinfo, &info);

    std::string str = Utilites::string_format(
                "%d seconds", info.uptime
                );

    this->sendMessage(message.channelID, str);

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::mc_server_list(SleepyDiscord::Message message)
{
    std::vector<std::string> arguments = Utilites::splitString(message.content);

    int offset = arguments.size() > 0 ? std::stoi(arguments.at(0)) : 0;

    try {
        std::vector<SystemdManager::Units> units = systemdManager->ListUnitsByPatterns({}, { "^@minecraft.service" });

        std::string result = "";

        this->sendMessage(message.channelID, Utilites::string_format("starting from offset %d", offset));

        size_t i = offset;
        for (; i < units.size(); ++i) {
            std::string name = units.at(i).get<0>();

            name = name.substr(0, name.find("@"));

            std::string result_2 = result + "``" + std::to_string(i) + "`` " + name + "\n";

            if (result_2.size() >= 1900) {
                break;
            }

            result = result_2;
        }

        result += "read ``" + std::to_string(i) + "`` / ``" + std::to_string(units.size()) + "``";

        this->sendMessage(
                    message.channelID,
                    result
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::mc_server_status(SleepyDiscord::Message message)
{
    std::vector<std::string> arguments = Utilites::splitString(message.content);

    if (arguments.size() <= 0) {
        this->sendMessage(
                    message.channelID,
                    "not enough arguments.\n``mc_server_status <unit>"
                    );
        return OK;
    }

    try {
        SystemdUnit proxy = systemdManager->getUnit(arguments.at(0));
        SystemdServiceUnit servic_proxy = systemdManager->GetUnitService(arguments.at(0));

        this->sendMessage(
                    message.channelID,
                    Utilites::string_format(
                        "statustext: %s\nstart ts:",
                        servic_proxy.StatusText().c_str()
                        )
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }

    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::mc_start_server(SleepyDiscord::Message message)
{
    if (!this->isElavatedUser(message.author)) {
        return INVALID_PERMISSIONS;
    }

    std::vector<std::string> arguments = Utilites::splitString(message.content);

    if (arguments.size() <= 0) {
        this->sendMessage(
                    message.channelID,
                    "not enough arguments.\n``mc_start_server <instance-name>``"
                    );
        return CALLBACK_RESPONSE::OK;
    }

    std::string unit_name = arguments.at(0)+"@minecraft.unit";

    try {
        std::string return_value = systemdManager->StartUnit(arguments.at(0), "replace");

        this->sendMessage(
                    message.channelID,
                    Utilites::string_format(
                        "starting server %s. (object at %s)",
                        arguments.at(0).c_str(),
                        return_value.c_str()
                    )
        );
    }
    catch (const sdbus::Error &e) {
        this->sendMessage(
                    message.channelID,
                    ":warning: exception!: " + e.getMessage()
        );
    }
    return OK;
}

UtilChanBot::CALLBACK_RESPONSE UtilChanBot::mc_stop_server(SleepyDiscord::Message message)
{
    if (!this->isElavatedUser(message.author)) {
        return INVALID_PERMISSIONS;
    }


    this->sendMessage(message.channelID, "stop server soon tm :)");

    return OK;
}

void UtilChanBot::onMessage(SleepyDiscord::Message message) {
    // ignore bot messages and messages from ourselves
    if (message.author.bot || message.author.ID == getID()) {
        return;
    }

    try {
        std::cout << "message: " << message.content << std::endl ;
        std::cout << "is admin? : " << std::to_string(isElavatedUser(message.author)) << std::endl ;

        // registered commands
        std::string command_prefix = "u!";

        if (message.startsWith(command_prefix)) {
            std::string cmd = message.content.substr(command_prefix.length());

            size_t space_loc = message.content.find(" ");
            if (space_loc != std::string::npos) {
                cmd = message.content.substr(command_prefix.length(), space_loc - command_prefix.length());
            }

            std::cout << "attempting cmd: " << cmd << std::endl;

            if (registeredCommands.count(cmd) == 1) {
                // might be a bad idea, might just make MessageCallbackFn add a seperate processed string.
                message.content = message.content.substr(space_loc + 1);
                if (space_loc == std::string::npos) {
                    message.content = "";
                }

                try {
                    int response = registeredCommands.at(cmd)(this, message);

                    switch (static_cast<CALLBACK_RESPONSE>(response)) {
                    case OK:
                        break;
                    case UNKNOWN_ERROR:
                        this->sendMessage(message.channelID, ":exclamation: callback commmand returned unknown error!");
                        break;
                    case INVALID_PERMISSIONS:
                        this->sendMessage(message.channelID, ":octagonal_sign: you don't have the permissions to do this!");
                        break;
                    default:
                        this->sendMessage(message.channelID, Utilites::string_format(":warning::interrobang: wtf? what is this response code? (code: %d)", response));
                    }
                }
                catch (std::exception &err) {
                    this->sendMessage(
                                message.channelID,
                                Utilites::string_format(
                                    ":warning: std::exception at %s while attempting command at %s (what() - %s)\n I will attempt to continue running!",
                                    Utilites::addressToString(&err).c_str(),
                                    Utilites::addressToString(&registeredCommands.at(cmd)).c_str(),
                                    err.what()
                                    )
                                );
                }
                return;
            }
            else {
                sendMessage(message.channelID, ":x: command not found. (recieved ``" + message.content + "``)");
            }
            return;
        }

        // hardcoded "dumb" commands i guess
        if (message.content == "ping") {
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(message.ID.timestamp().time_since_epoch());
            auto time_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            auto result = time_epoch - timestamp;
            sendMessage(message.channelID, "pong! - " + message.author.username + " (" + std::to_string(result.count()) + "ms)");
        }
        else if (message.content == "sysinfo") {
            sendMessage(message.channelID, Utilites::sysInfoStr());
        }
    }
    catch (const SleepyDiscord::ErrorCode &err) {
        this->sendMessage(
                    message.channelID,
                    Utilites::string_format(
                        ":warning: exception! (SleepyDiscord::ErrorCode %d)\nI will attempt to continue running!",
                        err
                        )
                    );
    }

}

template<typename ... Args>
int UtilChanBot::callApi(int (*fn_ptr)(Args ... args), Args ... args)
{
    int return_val = fn_ptr(args...);

    if (return_val != 0) {
        std::invalid_argument(Utilites::string_format("! expected return code 0, recieved %d", return_val));
    }

    return return_val;
}
