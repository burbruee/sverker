#ifndef STRINGHANDLER_H
#define STRINGHANDLER_H

class stringHandler
{
	public:
            stringHandler();
            void splitString(const std::string& str,std::vector<std::string>& tokens,const std::string& delimiters);
            std::string replaceString(std::string str,std::string l,std::string rep);
            std::string mergeLast(std::vector<std::string>& arr,int at);
            std::string getNick(std::string hmask);
            std::string toLower(std::string str);
            std::string generateWhitespace(std::string chanstr,std::string nickstr,std::string msgstr);
            std::string applyUniFix(std::string str);
            std::string parseUni(std::string str);
        private:
};
#endif