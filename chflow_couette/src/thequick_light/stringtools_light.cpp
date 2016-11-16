//========================================================================
#include "stringtools_light.h"
#include "libtools_light.h"

#include <functional>
#include <ctype.h>
#include <wchar.h>
#include <iostream>

using namespace std;
using namespace thequicklight::str;
//========================================================================
std::string thequicklight::str::replaceAll(const std::string& str, const std::string& from, const std::string& to)
{
    string strBuf(str);
	size_t start_pos = 0;
    while((start_pos = strBuf.find(from, start_pos)) != string::npos)
	{
        strBuf.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
    return strBuf;
}

//========================================================================
bool thequicklight::str::removeComments(const std::string& sourceStr, const std::string& commentSeparator, std::string& destStr)
{
	bool res = true;

	string source_str(sourceStr);

	int v_pos = source_str.find(commentSeparator);

	if (v_pos > 0)
		source_str.erase(v_pos, source_str.size() - v_pos + 1);
	else
		res = false;
	
    destStr = trim(source_str);

	return res;
}

//========================================================================
std::string thequicklight::str::trim(std::string str, std::string whitespaces)
{ 
    return ltrim(rtrim(str, whitespaces), whitespaces);
} 

//========================================================================
std::string thequicklight::str::ltrim(std::string str, std::string whitespaces)
{ 
    str.erase(0, str.find_first_not_of(whitespaces));
    return str;
}

//========================================================================
std::string thequicklight::str::rtrim(std::string str, std::string whitespaces)
{ 
    str.erase(str.find_last_not_of(whitespaces) + 1, string::npos);
    return str;
} 

//========================================================================
bool thequicklight::str::paramValueSplit(const std::string& pair, const std::string& delimeter, std::string &param, std::string &value)
{
	bool res = true;

	param.clear();
	value.clear();
	size_t delim_pos = pair.find(delimeter);
	if (delim_pos != string::npos)
	{
        param = pair.substr(0, delim_pos);
		value = pair.substr(delim_pos + delimeter.size());
	}
	else
	{
		res = false;
		param = pair;
	}

    param = trim(param);
    value = trim(value);

	return res;
}

//========================================================================
bool thequicklight::str::paramValueSplit(const std::string& pair, char delimeter, std::string &param, std::string &value)
{
    return paramValueSplit(pair, string(1, delimeter), param, value);
}

//========================================================================
bool thequicklight::str::splitString(const string& str, char delimeter, vector<string>& output)
{
    bool res = true;

    string temp = str;
    string strToAdd;

    output.clear();
    string::iterator sitBeg = temp.begin(),
                     sitEnd = find(temp.begin(),temp.end(),delimeter);
    do
    {
        strToAdd.clear();
        copy(sitBeg, sitEnd, std::inserter(strToAdd, strToAdd.begin()));
        output.push_back(strToAdd);
        if (sitEnd == temp.end())
            break;
        else
        {
            sitBeg = ++sitEnd;
            sitEnd = find(sitBeg, temp.end(), delimeter);
        }
    }
    while (true);

    return res;
}

//========================================================================
template<>
std::string thequicklight::str::toString(bool value)
{
	string str;	
	std::stringstream ss;
    ss << std::boolalpha << value;
	return ss.str();
}

//========================================================================
template<>
bool thequicklight::str::fromString<bool>(const std::string& str, bool* ok)
{
    if (ok) *ok = true;

    string strBuf(str);
    std::transform(strBuf.begin(), strBuf.end(), strBuf.begin(), ::tolower);

    if (strBuf.empty() || (strBuf == "null"))
    {
        return false;
    }

    if (strBuf == "t")
        return true;
    else if (strBuf == "f")
        return false;

    std::stringstream ss(strBuf);
    bool b;
	ss >> std::boolalpha >> b;
    if (ok)
        *ok = (ss.fail() == false);

	return b;
}

//========================================================================
