#ifndef CONFIG_H
#define CONFIG_H

class Config
{
	public:
                Config();
                std::string getApiHost();
                std::string getApiUri();
                sf::IPAddress getIrcServer();
                int getIrcPort();
                std::string getIrcChannel();
                std::string getOAuth();
                bool getMode();

	private:
                void setApiHost(std::string _apihost);
                void setApiUri(std::string _apiuri);
                void setIrcServer(sf::IPAddress _ircserver);
                void setIrcPort(int _ircport);
                void setIrcChannel(std::string _ircchannel);

                std::string apihost;
                std::string apiuri;
                std::string oauth;
                bool mode;
                sf::IPAddress ircserver;
                int ircport;
                std::string ircchannel;
};
#endif