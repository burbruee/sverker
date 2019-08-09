#include <vector>
#include <string>
#include "stringHandler.h"

stringHandler::stringHandler()
{
}

void stringHandler::splitString(const std::string& str,std::vector<std::string>& tokens,const std::string& delimiters = " ")
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

std::string stringHandler::replaceString(std::string str,std::string l,std::string rep)
{
    std::string::size_type pos = str.find(l);

    while(std::string::npos != pos)
    {
        str.replace(pos,l.length(),rep);
        pos = str.find(l);
    }

    return str;
}

std::string stringHandler::mergeLast(std::vector<std::string>& arr,int at)
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

std::string stringHandler::getNick(std::string hmask)
{
    hmask = hmask.substr(0,hmask.find_first_of("!"));
    hmask.erase(0,1);
    return hmask;
}

std::string stringHandler::toLower(std::string str)
{
    char uni[] = {};
    char to[] = {};
    for (unsigned int i = 0;i < str.length();i++)
    {
        for(unsigned int t = 0;t < 3;t++)
        {
            if (str[i] == uni[t])
            {
                str[i] = to[t];
            }
        }

        str[i] = std::tolower(str[i]);
    }

    return str;
}

std::string stringHandler::generateWhitespace(std::string chanstr,std::string nickstr,std::string msgstr)
{
    unsigned int whitespaces = 18 + chanstr.length() + nickstr.length();
    unsigned int curSize = 0;
    std::string endStr = "";

    std::vector<std::string> words;
    splitString(msgstr,words," ");

    for (unsigned int i = 0;i < words.size();i++)
    {
            if (words[i].length() > 80 - whitespaces)
            {
                std::vector<std::string>::iterator it(words.begin());
                words.insert(it + i + 1,words[i].substr((79 - whitespaces),words[i].length() - (79 - whitespaces)));
                words[i] = words[i].substr(0,(79 - whitespaces));
            }

            if (words[i].length() + curSize < 80 - whitespaces)
            {
                endStr += words[i].append(" ");
                curSize += words[i].length();
            }
            else
            {
                #ifndef _WIN32
                    endStr.append("\n");
                #endif

                endStr.append(((80 - whitespaces) - curSize) + whitespaces,' ');
                curSize = 0;
                i--;
            }
    }

    return endStr;
}

std::string stringHandler::applyUniFix(std::string str)
{
/*
    str = replaceString(str,std::string("\xC3").append("\xA5"),"å");
    str = replaceString(str,std::string("\xC3").append("\xA4"),"ä");
    str = replaceString(str,std::string("\xC3").append("\xB6"),"ö");
    str = replaceString(str,std::string("\xC3").append("\x85"),"Å");
    str = replaceString(str,std::string("\xC3").append("\x84"),"Ä");
    str = replaceString(str,std::string("\xC3").append("\x96"),"Ö");
*/
    return str;
}

std::string stringHandler::parseUni(std::string str)
{
/*
    str = replaceString(str,"å","\x86");
    str = replaceString(str,"ä","\x84");
    str = replaceString(str,"ö","\x94");
    str = replaceString(str,"Å","\x8F");
    str = replaceString(str,"Ä","\x8E");
    str = replaceString(str,"Ö","\x99");
*/
    return str;
}