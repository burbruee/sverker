#include <SFML/Network.hpp>
#include <string>
#include "config.h"

Config::Config()
{
    setApiHost("");
    setApiUri("");

    //0 = Normal IRC, 1 = Twitch IRC
    mode = 1;
    /*
    setIrcServer("se.quakenet.org");
    setIrcPort(6667);
    setIrcChannel("#142");
    */
    
    setIrcServer("irc.chat.twitch.tv");
    setIrcPort(6667);
    setIrcChannel("");
    oauth = "";
}

std::string Config::getApiHost()
{
    return apihost;
}

std::string Config::getApiUri()
{
    return apiuri;
}

sf::IPAddress Config::getIrcServer()
{
    return ircserver;
}

int Config::getIrcPort()
{
    return ircport;
}

std::string Config::getIrcChannel()
{
    return ircchannel;
}

bool Config::getMode()
{
    return mode;
}

std::string Config::getOAuth()
{
    return oauth;
}

void Config::setApiHost(std::string _apihost)
{
    apihost = _apihost;
}

void Config::setApiUri(std::string _apiuri)
{
    apiuri = _apiuri;
}

void Config::setIrcServer(sf::IPAddress _ircserver)
{
    ircserver = _ircserver;
}

void Config::setIrcPort(int _ircport)
{
    ircport = _ircport;
}

void Config::setIrcChannel(std::string _ircchannel)
{
    ircchannel = _ircchannel;
}