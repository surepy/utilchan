#ifndef UTILCHANBOT_H
#define UTILCHANBOT_H

#include "sleepy_discord/sleepy_discord.h"
#include "systemd.h"
#include "sys/sysinfo.h"
#include <map>

// sleepy's discord bot using SleepyDiscordClient :))
class UtilChanBot : public SleepyDiscord::DiscordClient {
public:
    UtilChanBot();

    using SleepyDiscord::DiscordClient::DiscordClient;

    void onMessage(SleepyDiscord::Message message) override;

    bool isElavatedUser(SleepyDiscord::User &usr);

    typedef std::function<int(UtilChanBot *, SleepyDiscord::Message)> MessageCallbackFn;

    SystemdManager* getSystemdManager() { return systemdManager; }


    enum CALLBACK_RESPONSE {
        OK,
        UNKNOWN_ERROR,
        INVALID_PERMISSIONS,
    };

public:
    // features!
    int ReplyTest(SleepyDiscord::Message message);


    int StartSystemdUnit(SleepyDiscord::Message message);
    CALLBACK_RESPONSE SystemdUnitActiveState(SleepyDiscord::Message message);
    CALLBACK_RESPONSE SystemdListUnits(SleepyDiscord::Message message);

    CALLBACK_RESPONSE help(SleepyDiscord::Message message);

    CALLBACK_RESPONSE info(SleepyDiscord::Message message);
    CALLBACK_RESPONSE uptime(SleepyDiscord::Message message);

    CALLBACK_RESPONSE mc_server_list(SleepyDiscord::Message message);
    CALLBACK_RESPONSE mc_server_status(SleepyDiscord::Message message);
    CALLBACK_RESPONSE mc_stop_server(SleepyDiscord::Message message);
    CALLBACK_RESPONSE mc_start_server(SleepyDiscord::Message message);

    // internal functions


    template<typename ... Args>
    int callApi(int (*fn_ptr)(Args ... args), Args ... args);

private:
    std::map<std::string, MessageCallbackFn> registeredCommands;
    SystemdManager *systemdManager;
};

#endif // UTILCHANBOT_H
