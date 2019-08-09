#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H
#include "config.h"
#include "stringHandler.h"

class IRCConnection
{
	public:
            IRCConnection();
            IRCConnection(stringHandler * ss);
            bool connect(sf::IPAddress * hostAddr,int port,std::string nickname,const char * name,std::string channel);
            bool listen();


	private:
            bool socketSend(sf::SocketTCP * sock,std::string buffer);
            bool sockConnect(sf::SocketTCP * sock,int port,std::string ip);
            bool channelSendMsg(std::string channel, std::string message);
            void triggerInit(std::vector< std::vector<std::string> > &vec,std::string in);
            std::string getTriggersFrom(std::string host,std::string uri);
            bool checkTriggerString(std::string str,std::string str2,std::string cs,std::string ps);

            sf::SocketTCP mainSocket;
            char recv[1024];
            size_t r;
            std::string chan, nick, srecv;
            std::vector<std::string> recvArr, recvArr2;
            std::vector< std::vector<std::string> > trigger;
            stringHandler * sH;
            Config conf;
            char timeBuffer[10];

};
#endif