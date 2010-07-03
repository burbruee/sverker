/*

            CRULLOP 2.0
        FRASSEFRAS FÖR FAN
    BURBRUEE ÄR BÄST JAG ÄLSKAR HONOM

                                            */
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>
#include <time.h>

class stringHandler
    {

        public:

        void splitString(const std::string& str,std::vector<std::string>& tokens,const std::string& delimiters = " ")
            {
                std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
                std::string::size_type pos = str.find_first_of(delimiters, lastPos);

                while (std::string::npos != pos || std::string::npos != lastPos)
                {
                    tokens.push_back(str.substr(lastPos, pos - lastPos));
                    lastPos = str.find_first_not_of(delimiters, pos);
                    pos = str.find_first_of(delimiters, lastPos);
                }
            }

        std::string mergeLast(std::vector<std::string>& arr,int at)
            {
                std::string end = "";
                for (int i = at;i < arr.size();i++)
                    {
                        end += arr[i];
                        end += " ";
                    }

                end.erase(0,1);
                end.erase(end.length() - 1,1);
                return end;
            }

        std::string getNick(std::string hmask)
            {
                hmask = hmask.substr(0,hmask.find_first_of("!"));
                hmask.erase(0,1);
                return hmask;
            }

        void doUni(std::string * str)
            {
                std::string change[] = {"å","ä","ö"};
                std::string into[] = {"a","a","o"};

                for (int i = 0;i < str -> length();i++)
                    {
                        for (int t = 0;t < 3;t++)
                            {
                                if (str -> substr(i,1) == change[t])
                                    {
                                        str -> replace(i,1,into[t]);
                                    }
                            }
                    }
            }
    };

class IRCConnection
    {
        public:

            IRCConnection(stringHandler * ss)
                {
                    sH = ss;
                }

            bool connect(sf::IPAddress * hostAddr,int port,std::string nickname,const char * name,std::string channel)
                {
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

                    if (!socketSend(&mainSocket,std::string("NICK ").insert(5,nick)))
                        return false;

                    if (!socketSend(&mainSocket,std::string("USER  ignored irc.quakenet.org :sverker").insert(5,name)))
                        return false;

                    std::cout << "DONE!\n\n <<  LISTENING  >>\n\n";
                    return true;
                }

            bool listen()
                {
                    if (mainSocket.Receive(recv,sizeof(recv),r) != sf::Socket::Done)
                        {
                            std::cout << "Host has disconnected...";
                            return false;
                        }
                    else
                        {
                            std::string srecv(recv);
                            //sH -> doUni(&srecv);
                            time_t myTime = time(NULL);
                            strftime(timeBuffer, 10, "%H:%M:%S", localtime(&myTime));

                            sH -> splitString(srecv,recvArr,"\r\n");
                            for (int i = 0;i < recvArr.size();i++)
                                {
                                    //std::cout << " >> " << recvArr[i] << "\n";
                                    sH -> splitString(recvArr[i],recvArr2);

                                    if (recvArr2.size() > 1)
                                        {
                                            if (recvArr2[1] == "AUTH")
                                                    std::cout << " >> " << sH -> mergeLast(recvArr2,2) << "\n";

                                            if (recvArr2[0] == "PING")
                                                    socketSend(&mainSocket,std::string("PONG ").insert(5,recvArr2[1]));

                                            if (recvArr2[1] == "MODE")
                                                {
                                                    if (recvArr2[3] == "+i")
                                                        {
                                                            std::cout << "\n Joining " << chan << "...\n";
                                                            socketSend(&mainSocket,std::string("JOIN ").insert(5,chan));
                                                        }
                                                }

                                            if (recvArr2[1] == "JOIN")
                                                {
                                                    if (sH -> getNick(recvArr2[0]) == nick)
                                                        {
                                                            std::cout << " You're now talking in " << recvArr2[2] << ".\n";
                                                        }
                                                }

                                            if (recvArr2[1] == "PRIVMSG")
                                                {

                                                    std::cout << " [" << timeBuffer << "] <" << sH -> getNick(recvArr2[0]) << "> " << sH -> mergeLast(recvArr2,3) << "\n";

                                                    if (sH -> mergeLast(recvArr2,3) == "hej")
                                                        {
                                                            channelSendMsg(recvArr2[2],std::string("Tja, !").insert(5,sH -> getNick(recvArr2[0])));
                                                        }

                                                    if (sH -> mergeLast(recvArr2,3) == "korv")
                                                        {
                                                            channelSendMsg(recvArr2[2],"Korv? Mums!");
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

        private:

            bool socketSend(sf::SocketTCP * sock,std::string buffer)
                {
                    buffer += "\r\n";
                    if (sock -> Send(buffer.data(),buffer.length()) != sf::Socket::Done)
                        return false;

                    return true;
                }

            bool sockConnect(sf::SocketTCP * sock,int port,std::string ip)
                {
                    if (sock -> Connect(port,ip) != sf::Socket::Done)
                        return false;

                    return true;
                }

            bool channelSendMsg(std::string channel, std::string message)
                {
                    if (!socketSend(&mainSocket,std::string("PRIVMSG ").append(channel).append(" :").append(message)))
                        return false;

                    std::cout << " [" << timeBuffer << "] <" << nick << "> " << message << "\n";
                    return true;
                }

            sf::SocketTCP mainSocket;
            char recv[1024];
            size_t r;
            std::string chan, nick, srecv;
            std::vector<std::string> recvArr, recvArr2;
            stringHandler * sH;
            char timeBuffer[10];
    };

int main()
    {

        std::cout <<    "****************************************************************************\n" <<
                        "*** THE MOST AWESOME AND SPAMTASTIC INTERNET RELAY CHAT BOT IN THE WORLD ***\n" <<
                        "****************************************************************************\n" <<
                        "*::'######::'##::::'##:'########:'########::'##:::'##:'########:'########::*\n" <<
                        "*:'##... ##: ##:::: ##: ##.....:: ##.... ##: ##::'##:: ##.....:: ##.... ##:*\n" <<
                        "*: ##:::..:: ##:::: ##: ##::::::: ##:::: ##: ##:'##::: ##::::::: ##:::: ##:*\n" <<
                        "*:. ######:: ##:::: ##: ######::: ########:: #####:::: ######::: ########::*\n" <<
                        "*::..... ##:. ##:: ##:: ##...:::: ##.. ##::: ##. ##::: ##...:::: ##.. ##:::*\n" <<
                        "*:'##::: ##::. ## ##::: ##::::::: ##::. ##:: ##:. ##:: ##::::::: ##::. ##::*\n" <<
                        "*:. ######::::. ###:::: ########: ##:::. ##: ##::. ##: ########: ##:::. ##:*\n" <<
                        "*::......::::::...:::::........::..:::::..::..::::..::........::..:::::..::*\n" <<
                        "****************************************************************************\n" <<
                        "************* PROGRAMMING: CARL, BURBRUEE | COPYRIGHT (C) 2010 *************\n" <<
                        "****************************************************************************\n\n";

        stringHandler strH;
        IRCConnection IRC(&strH);

        sf::IPAddress iP("irc.quakenet.org");
        if (!IRC.connect(&iP,6667,"SVERKER","SVERKER","#sverker"))
            return 0;

        while(true)
            {
                if (!IRC.listen())
                    return 0;
            }

        return 0;
    }
