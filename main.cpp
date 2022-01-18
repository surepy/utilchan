#include <iostream>
#include "sleepy_discord/sleepy_discord.h"
#include <sdbus-c++/sdbus-c++.h>
#include "sys/sysinfo.h"
#include "systemd.h"

bool is_elavated_user(SleepyDiscord::User &usr) {
    if (usr.ID == "449284815976529940") {
        return true;
    }

    return false;
}

// sleepy's discord bot using sleepy-discord :))
class UtilChanClientClass : public SleepyDiscord::DiscordClient {
public:
    using SleepyDiscord::DiscordClient::DiscordClient;

    void onMessage(SleepyDiscord::Message message) override {
        // ignore bot messages and messages from ourselves
        if (message.author.bot || message.author.ID == getID()) {
            return;
        }

        std::cout << "message: " << message.content << std::endl ;
        std::cout << "is admin? : " << std::to_string(is_elavated_user(message.author)) << std::endl ;

        if (message.content == "-ping")
            sendMessage(message.channelID, "pong! - " + message.author.username);
        else if (message.content == "sysinfo") {
            struct sysinfo status;

            int ret_code = sysinfo(&status);

            if (ret_code != 0) {
                sendMessage(message.channelID, ":warning: error: sysinfo() returned code " + std::to_string(ret_code) + " !");
                return;
            }


            sendMessage(
                        message.channelID,
                        "up " + std::to_string(status.uptime) + ", " + std::to_string(status.freeram) + " bytes free out of " + std::to_string(status.totalram)
                        );

        }
    }
};

void onConcatenated(sdbus::Signal& signal)
{
    std::string concatenatedString;
    signal >> concatenatedString;

    std::cout << "Received signal with concatenated string " << concatenatedString << std::endl;
}

int main() {


    auto concatenatorProxy = sdbus::createProxy("org.freedesktop.systemd1", "/org/freedesktop/systemd1");

    SystemdManager asfd = SystemdManager();

    std::cout << "help " << asfd.Version() << std::endl;

    SystemdUnit unit = asfd.getUnit("dbus.service");

    std::cout << "dbus active: " << unit.ActiveState() << std::endl;

    auto statust = concatenatorProxy->createMethodCall("org.freedesktop.systemd1.Manager", "GetUnit");
    statust << "dbus.service";
    auto stre = concatenatorProxy->callMethod(statust);

    sdbus::ObjectPath obj_path;
    stre >> obj_path;
    std::cout << "dbus obj path: " << obj_path << std::endl;

    auto serviceProxy = sdbus::createProxy("org.freedesktop.systemd1", obj_path);
    sdbus::PropertyGetter te = serviceProxy->getProperty("ActiveState");
    std::string efd = te.onInterface("org.freedesktop.systemd1.Unit");

    std::cout << "dubs active: " << efd << std::endl;

    sdbus::PropertyGetter version = concatenatorProxy->getProperty("Version");
    std::string verstring = version.onInterface("org.freedesktop.systemd1.Manager");

    sdbus::PropertyGetter features = concatenatorProxy->getProperty("Features");
    std::string feastring = features.onInterface("org.freedesktop.systemd1.Manager");

    std::cout << "version: " << verstring << " features: " << feastring << std::endl;




    std::cout << "Hello World!" << std::endl;

    UtilChanClientClass client(API_KEY, SleepyDiscord::USER_CONTROLED_THREADS);

    client.run();
    return 0;
}
