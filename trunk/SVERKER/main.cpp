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
                std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
                std::string::size_type pos = str.find_first_of(delimiters,lastPos);

                while (std::string::npos != pos || std::string::npos != lastPos)
                {
                    tokens.push_back(str.substr(lastPos, pos - lastPos));
                    lastPos = str.find_first_not_of(delimiters,pos);
                    pos = str.find_first_of(delimiters,lastPos);
                }
            }

        std::string replaceString(std::string str,std::string l,std::string rep)
            {
                std::string::size_type pos = str.find(l);

                while(std::string::npos != pos)
                    {
                        str.replace(pos,l.length(),rep);
                        pos = str.find(l);
                    }

                return str;
            }

        std::string mergeLast(std::vector<std::string>& arr,int at)
            {
                std::string end = "";
                for (unsigned int i = at;i < arr.size();i++)
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

                for (unsigned int i = 0;i < str -> length();i++)
                    {
                        for (unsigned int t = 0;t < 3;t++)
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

                    std::cout << "Retrieving triggers... ";
                    triggerInit(trigger,getTriggersFrom("www.xeyibland.se","/sverker/api.php"));
                    std::cout << "DONE!\n";

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
                            time_t myTime = time(NULL);
                            strftime(timeBuffer, 10, "%H:%M:%S", localtime(&myTime));

                            sH -> splitString(srecv,recvArr,"\r\n");
                            for (unsigned int i = 0;i < recvArr.size();i++)
                                {
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

                                                    for (unsigned int i = 0;i < trigger.size();i++)
                                                        {
                                                            if (sH -> mergeLast(recvArr2,3) == trigger[i][0])
                                                                {
                                                                    channelSendMsg(recvArr2[2],sH -> replaceString(trigger[i][1],"$n",sH -> getNick(recvArr2[0])));
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

            void triggerInit(std::vector< std::vector<std::string> > &vec,std::string in)
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

            std::string getTriggersFrom(std::string host,std::string uri)
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

            sf::SocketTCP mainSocket;
            char recv[1024];
            size_t r;
            std::string chan, nick, srecv;
            std::vector<std::string> recvArr, recvArr2;
            std::vector< std::vector<std::string> > trigger;
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

        sf::IPAddress iP("se.quakenet.org");
        if (!IRC.connect(&iP,6667,"SVERKER","Sverker","#sverker"))
            return 0;

        while(true)
            {
                if (!IRC.listen())
                    return 0;
            }

        return 0;
    }
