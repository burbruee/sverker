#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>
#include <string>
#include <time.h>
#include <cctype>
#include "IRCConnection.h"

IRCConnection::IRCConnection()
{
}

IRCConnection::IRCConnection(stringHandler * ss)
{
    sH = ss;
}

bool IRCConnection::connect(sf::IPAddress * hostAddr,int port,std::string nickname,const char * name,std::string channel)
{

    std::cout << "Retrieving triggers... ";
    triggerInit(trigger,getTriggersFrom(conf.getApiHost(),conf.getApiUri()));
    std::cout << "DONE!\n[Loaded " << trigger.size() << " triggers.]\n\n";

    chan = channel;
    nick = nickname;

    std::cout << "Checking host... ";

    if (!hostAddr -> IsValid())
    {
        std::cout << "Could not validate host.";
        return false;
    }

    std::cout << "SUCCESS!\n" << "Connecting to " << hostAddr -> ToString() << "... ";

    if (!sockConnect(&mainSocket,port,hostAddr -> ToString()))
    {
        std::cout << "Could not connect to host.";
        return false;
    }

    std::cout << "SUCCESS!\nSending initial data to server... ";

    if (conf.getMode() == 0)
    {   
        if (!socketSend(&mainSocket,std::string("USER  ignored irc.quakenet.org :sverker").insert(5,name)))
            return false;

        if (!socketSend(&mainSocket,std::string("NICK ").insert(5,nick)))
            return false;
    }
    else if(conf.getMode() == 1)
    {   
        if (!socketSend(&mainSocket,std::string("PASS ").insert(5,conf.getOAuth())))
            return false;

        if (!socketSend(&mainSocket,std::string("NICK ").insert(5,nick)))
            return false;

        if (!socketSend(&mainSocket,std::string("USER  8 * :sverkerbot").insert(5,name)))
            return false;

        if (!socketSend(&mainSocket,std::string("JOIN ").insert(5,chan)))
            return false;
    }


    std::cout << "DONE!\n\n <<  LISTENING  >>\n\n";

    return true;

}

bool IRCConnection::listen()
{

    if (mainSocket.Receive(recv,sizeof(recv),r) != sf::Socket::Done)
    {
        std::cout << "Host has disconnected...";
        return false;
    }
    else
    {
        std::string srecv(recv);
        srecv = sH -> applyUniFix(srecv);
        time_t myTime = time(NULL);
        strftime(timeBuffer, 10, "%H:%M:%S", localtime(&myTime));

        sH -> splitString(srecv,recvArr,"\r\n");
        for (unsigned int i = 0;i < recvArr.size();i++)
        {
            sH -> splitString(recvArr[i],recvArr2, " ");

            if (recvArr2.size() > 1)
            {
                if (recvArr2[1] == "AUTH")
                    std::cout << " >> " << sH -> mergeLast(recvArr2,2) << "\n";

                    if (recvArr2[0] == "PING")
                        socketSend(&mainSocket,std::string("PONG ").insert(5,recvArr2[1]));
                        //socketSend(&mainSocket,std::string("PONG :tmi.twitch.tv"));

                        if (recvArr2[1] == "MODE")
                        {
                            if (recvArr2[3] == "+i")
                            {
                                if (conf.getMode() == 0)
                                {
                                    socketSend(&mainSocket,std::string("PRIVMSG Q@CServe.quakenet.org :AUTH SVERKERBOT mFKMFxnWvt"));
                                    socketSend(&mainSocket,std::string("MODE SVERKER +x"));
                                }                               
                                
                                socketSend(&mainSocket,std::string("JOIN ").insert(5,chan));
                            }
                        }

                        if (recvArr2[1] == "NICK")
                        {
                            if (sH -> getNick(recvArr2[0]) == nick)
                            {
                                nick = sH -> mergeLast(recvArr2,2);
                            }
                        }

                        if (recvArr2[1] == "JOIN")
                        {
                            if (sH -> getNick(recvArr2[0]) == nick)
                            {
                                std::cout << " You're now talking in " << recvArr2[2] << ":\n";
                            }
                            else
                            {
                                std::cout << " [" << timeBuffer << "] " << sH -> getNick(recvArr2[0]) << " joined " << recvArr2[2] << ".\n";
                            }
                        }

                        if (recvArr2[1] == "PART")
                        {
                            if (recvArr2.size() > 3)
                            {
                                std::cout << " [" << timeBuffer << "] " << sH -> getNick(recvArr2[0]) << " left " << recvArr2[2] << ". (" << sH -> parseUni(sH -> mergeLast(recvArr2,3)) << ")\n";
                            }
                            else
                            {
                                std::cout << " [" << timeBuffer << "] " << sH -> getNick(recvArr2[0]) << " left " << recvArr2[2] << ".\n";
                            }
                        }

                        if (recvArr2[1] == "PRIVMSG")
                        {
                            if (recvArr2[2] != nick)
                            {
                                std::cout << " [" << timeBuffer << "] [" << recvArr2[2] << "] <" << sH -> getNick(recvArr2[0]) << "> " << sH -> generateWhitespace(recvArr2[2],sH -> getNick(recvArr2[0]),sH -> parseUni(sH -> mergeLast(recvArr2,3))) << "\n";

                                for (unsigned int i = 0;i < trigger.size();i++)
                                {
                                    if (checkTriggerString(sH -> mergeLast(recvArr2,3),trigger[i][0],trigger[i][2],trigger[i][3]))
                                    {
                                        channelSendMsg(recvArr2[2],sH -> replaceString(trigger[i][1],"$n",sH -> getNick(recvArr2[0])));
                                    }
                                }
                            }
                            else
                            {
                                if (sH -> getNick(recvArr2[0]) == "Frasse" || sH -> getNick(recvArr2[0]) == "burb")
                                {
                                    if (recvArr2[3] == ":UPDATE")
                                    {
                                        channelSendMsg(sH -> getNick(recvArr2[0]),std::string("\x01").append("ACTION is clearing triggers...\x01"));
                                        trigger.clear();
                                        channelSendMsg(sH -> getNick(recvArr2[0]),std::string("\x01").append("ACTION is retrieving new triggers...\x01"));
                                        triggerInit(trigger,IRCConnection::getTriggersFrom(conf.getApiHost(),conf.getApiUri()));
                                        channelSendMsg(sH -> getNick(recvArr2[0]),std::string("\x01").append("ACTION is done!\x01"));
                                    }
                                    else
                                    {
                                        socketSend(&mainSocket,sH -> mergeLast(recvArr2,3));
                                    }
                                }
                            }
                        }
            }

            recvArr2.clear();
        }

        memset(recv,'\0',1024);
        recvArr.clear();
        srecv = "";
    }

    return true;
}       

bool IRCConnection::socketSend(sf::SocketTCP * sock,std::string buffer)
{

    buffer += "\r\n";
    if (sock -> Send(buffer.data(),buffer.length()) != sf::Socket::Done)
        return false;

    buffer = "";

    return true;
}

bool IRCConnection::sockConnect(sf::SocketTCP * sock,int port,std::string ip)
{

    if (sock -> Connect(port,ip) != sf::Socket::Done)
        return false;

    return true;

}

bool IRCConnection::channelSendMsg(std::string channel, std::string message)
{

    if (!socketSend(&mainSocket,std::string("PRIVMSG ").append(channel).append(" :").append(message)))
        return false;

    std::cout << " [" << timeBuffer << "] [" << channel << "] <" << nick << "> " << sH -> generateWhitespace(channel,nick,sH -> parseUni(message)) << "\n";

    return true;
}

void IRCConnection::triggerInit(std::vector< std::vector<std::string> > &vec,std::string in)
{

    std::vector<std::string> r;
    sH -> splitString(in,r,"|");

    for (unsigned int i = 0;i < r.size();i++)
    {
        std::vector<std::string> r2;
            sH -> splitString(r[i],r2,"§");
            vec.push_back(r2);
    }

}

std::string IRCConnection::getTriggersFrom(std::string host,std::string uri)
{

    sf::Http http;
    http.SetHost(host);

    sf::Http::Request req;
    req.SetMethod(sf::Http::Request::Get);
    req.SetURI(uri);


    sf::Http::Response resp = http.SendRequest(req);
    std::string body = resp.GetBody();


    return body;
}

bool IRCConnection::checkTriggerString(std::string str,std::string str2,std::string cs,std::string ps)
{

    if (cs == "no")
    {
        str = sH -> toLower(str);
        str2 = sH -> toLower(str2);
    }

    if (ps == "yes" && str == str2)
        return true;

    if (ps == "no" && str.find(str2) != std::string::npos)
        return true;

    return false;
}
