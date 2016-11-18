//========================================================================
#include "iniparser_light.h"
#include "stringtools_light.h"

#include <sstream>
#include <locale.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;
using namespace thequicklight;
using namespace thequicklight::str;


//========================================================================
thequicklight::IniParser::IniParser(const string& filename)
    : m_isIniViewModified(false)
    , m_isLoaded(false)
{
    ErrorCode res = initParser(filename);
    m_isLoaded = (res == ErrorCode::Success) ? true : false;
}

//========================================================================
thequicklight::IniParser::~IniParser()
{
    save();

    //MAC_MEMRELEASE(iniView_);
    //MAC_MEMRELEASE(rawStrings_);
}

//========================================================================
IniParser::ErrorCode IniParser::initParser(const string& filename)
{
    m_iniView.clear();
    m_rawStrings.clear();
    m_iniFilename = filename;
    return loadIniFile();
}

//========================================================================
IniParser::ErrorCode IniParser::loadIniFile()
{
    ifstream fs;
    fs.open(m_iniFilename);
    if (!fs.is_open())
        return ErrorCode::FileCannotBeOpened;

    // Get line from the stream
    string stringBuf;
    int strNum = 0;
    string currentSection;
    int pos = 0;

    while (getline(fs, stringBuf))
    {
        // Check if the line is comment or empty string
        string strWithoutComm = removeSubComment( stringBuf );
        if ((strWithoutComm == "") || (strWithoutComm == "[]"))
        {
            // Put it to raw strings
            m_rawStrings[strNum] = stringBuf;
        }
        // Check if it is a section
        else if ((strWithoutComm[0] == '[') && (strWithoutComm[strWithoutComm.size() - 1] == ']'))
        {
            string sectName = ::trim(strWithoutComm.substr(1, strWithoutComm.size() - 2));
            m_iniView[sectName].pos = pos;
            pos++;

            int commentPos = stringBuf.find_first_of("#");
            if (commentPos != string::npos)
            {
                m_iniView[sectName].comment = ::trim(stringBuf.substr(commentPos + 1));
            }

            currentSection = sectName;
        }
        // Check if it is a parameter-value line (param = value # comment)
        else
        {
            ParamContent paramContent;
            string paramName;
            if (splitRawParamLine(stringBuf, paramContent, paramName) != ErrorCode::Success)
                return ErrorCode::InvalidString;

            m_iniView[currentSection].paramLines[paramName] = paramContent;
            m_iniView[currentSection].paramLinesList.push_back(*(new const ParamLine(paramName, paramContent))); //+++
        }

        strNum++;
    }

    return ErrorCode::Success;
}

//========================================================================
string IniParser::removeSubComment(const string& source)
{
    string resultStr = source;
    size_t commentBeginning = resultStr.find("#");
    if (commentBeginning != string::npos)
        resultStr.erase(commentBeginning, string::npos);

    return ::trim(resultStr);
}

//========================================================================
IniParser::ErrorCode IniParser::splitRawParamLine(const string& paramLine,
                                                  ParamContent& paramContent,
                                                  string& paramName) const
{
    int equalSymbol = paramLine.find("=");

    if( equalSymbol == string::npos )
        return ErrorCode::InvalidString;

    paramName = ::trim(paramLine.substr(0, equalSymbol));

    int commentBeginning = paramLine.find( "#" );
    if( commentBeginning == string::npos )
	{
        paramContent.value = ::trim(paramLine.substr(equalSymbol + 1));
        paramContent.descr = "";
	}
	else
	{
        paramContent.value = ::trim(paramLine.substr(equalSymbol + 1, commentBeginning - equalSymbol - 1));
        paramContent.descr = ::trim(paramLine.substr(commentBeginning + 1));
	}

    return ErrorCode::Success;
}

//========================================================================
IniParser::ErrorCode IniParser::getParamContent(const string& section,
                                                const string& paramName,
                                                ParamContent& paramContent) const
{
    auto sectIt = m_iniView.find(section);
    if (sectIt == m_iniView.end())
        return ErrorCode::SectionNotFound;

    // Search parameter in the section
    string nextParamName;
    auto it = sectIt->second.paramLinesList.begin();
    for (; it != sectIt->second.paramLinesList.end(); it++)
    {
        nextParamName = it->name;
        if ((nextParamName == paramName) ||
             (nextParamName[0] == '*' && ::trim(nextParamName.substr(1)) == paramName) ||
             (nextParamName[0] == '-' && ::trim(nextParamName.substr(1)) == paramName)
           )
        {
            paramContent = it->content;
            break;
        }
    }

    if( it == sectIt->second.paramLinesList.end() )
        return ErrorCode::ParameterNotFound;
	

	/*																												//#ash.26.04.2013.00009
    string nextParamName;
    auto it;
	for( it = sectIt->second.mParamLines.begin(); it != sectIt->second.mParamLines.end(); it++ )
	{
		nextParamName = it->first;
		if( (nextParamName == pParamName) ||
			(nextParamName[0] == L'*' && this->trim(nextParamName.substr(1)) == pParamName) ||
			(nextParamName[0] == L'-' && this->trim(nextParamName.substr(1)) == pParamName) 
		  )///@todo: fix it
		{
			oParamContent = it->second;
			break;
		}
	}
	
	if( it == sectIt->second.mParamLines.end() ) 
		return -1;
	*/

    return ErrorCode::Success;
}

//========================================================================
IniParser::ErrorCode IniParser::setValue(const string& section,
                                         const string& paramName,
                                         const string& newValue)
{
    auto sectIt = m_iniView.find(section);
    if (sectIt == m_iniView.end())
        return ErrorCode::SectionNotFound;

    auto paramLinesListIt = sectIt->second.paramLinesList.begin();
    for (; paramLinesListIt != sectIt->second.paramLinesList.end(); ++paramLinesListIt)
    {
        if (paramLinesListIt->name == paramName) break;
    }

    if (paramLinesListIt == sectIt->second.paramLinesList.end())
        return ErrorCode::ParameterNotFound;

    paramLinesListIt->content.value = newValue;

    /// \todo: duplicate in map
    auto paramIt = sectIt->second.paramLines.find(paramName);
    if (paramIt == sectIt->second.paramLines.end())
    {
        return ErrorCode::ParameterNotFound;
    }

    paramIt->second.value = newValue;
    m_isIniViewModified = true;
    return ErrorCode::Success;
}

//========================================================================
bool IniParser::sectionExists(const string& section) const
{
    if (m_iniView.find(section) != m_iniView.end())
		return true;

	return false;
}

//========================================================================
IniParser::ErrorCode IniParser::save()
{
    // list is used due to importance of params and section order
    if (m_isIniViewModified)
	{
        ofstream fileStream(getFilename().c_str(), ios_base::out | ios_base::trunc);
        vector< string > sectionsList = getSections();
        auto sectIt = sectionsList.begin();
        ParamLinesList::iterator paramLineListIt;
        auto rawStrIt = m_rawStrings.begin();
        int strNum = 0;
        bool sectStarts = false;

        while (((sectIt != sectionsList.end()) || (rawStrIt != m_rawStrings.end())) && fileStream.good()) /// \todo: bad checks (as well as below)
        {
            if ((rawStrIt != m_rawStrings.end()) && (rawStrIt->first == strNum)) {
                fileStream << rawStrIt->second << endl;
                rawStrIt++;
            }
            else if (!sectStarts) {
                fileStream	<< "[" << *sectIt << "]" << "#" << m_iniView[*sectIt].comment << endl;
                paramLineListIt = m_iniView[*sectIt].paramLinesList.begin();															//#ash.26.04.2013.00009
                sectStarts = true;
            }
            else {
                fileStream << paramLineListIt->name
                    << "="
                    << paramLineListIt->content.value
                    << "//"
                    << paramLineListIt->content.descr
                    << endl;

                if (++paramLineListIt == m_iniView[*sectIt].paramLinesList.end()) {
                    sectIt++;
                    sectStarts = false;
                }
            }

            strNum++;
        }

        if (!fileStream.good())
            return ErrorCode::BadSaving;

        m_isIniViewModified = false;
    }

    return ErrorCode::Success;
}

//========================================================================
string IniParser::getFilename() const
{
    return m_iniFilename;
}

// ========================================================================
IniParser::ErrorCode IniParser::setFilename(const string& filename)
{
    save();
    ErrorCode res = initParser(filename);
    m_isLoaded = (res == ErrorCode::Success) ? true : false;
    return res;
}

//========================================================================
IniParser::ErrorCode IniParser::getScriptFromFile(const string&  section,
                                                  list< string >&  script,
                                                  bool removeSubcomments) const
{
    auto sectIt = m_iniView.find(section);
    if (sectIt == m_iniView.end())
        return ErrorCode::SectionNotFound;

    script.clear();
    const ParamLinesList& paramLinesList = sectIt->second.paramLinesList;
    for (auto paramListIt = paramLinesList.begin(); paramListIt != paramLinesList.end(); ++paramListIt)
    {
        script.push_back((removeSubcomments)? paramListIt->name + "=" + paramListIt->content.value
                                                 :paramListIt->name + "=" + paramListIt->content.value + "//" + paramListIt->content.descr );
    }																																				//#ash.26.04.2013.00009

    return ErrorCode::Success;
}

//========================================================================
std::vector<std::string> IniParser::getParamNames(const std::string& section, IniParser::ErrorCode* error /*= NULL*/) const
{
    SET_ERROR(error, ErrorCode::Success);
    std::vector<std::string> paramNames;
    auto sectIt = m_iniView.find(section);
    RETURN_DUMMY_IF_ERROR(error, (sectIt == m_iniView.end()) ? ErrorCode::SectionNotFound : ErrorCode::Success, paramNames);
                                                                                                                                        //#ash.26.04.2013.00009
    const ParamLinesList& paramLinesList = sectIt->second.paramLinesList;
    paramNames.resize(paramLinesList.size());
    ::transform(paramLinesList.begin(), paramLinesList.end(), paramNames.begin(),
                [](const ParamLine& paramLine) -> string {return paramLine.name;});
    return paramNames;
}

//========================================================================
std::string IniParser::getParamDescription(const std::string& section,
                                           const std::string& paramName,
                                           IniParser::ErrorCode* error /*= NULL*/) const
{
    SET_ERROR(error, ErrorCode::Success);
    ParamContent paramContent;
    RETURN_DUMMY_IF_ERROR(error, getParamContent(section, paramName, paramContent), "");
    return paramContent.descr;
}

//========================================================================
std::string IniParser::getSectionDescription(const string& section, IniParser::ErrorCode* error /*= NULL*/) const
{
    SET_ERROR(error, ErrorCode::Success);
    auto sectIt = m_iniView.find(section);
    RETURN_DUMMY_IF_ERROR(error, (sectIt == m_iniView.end()) ? ErrorCode::SectionNotFound : ErrorCode::Success, "");

    return sectIt->second.comment;
}

//========================================================================
IniParser::ParamData IniParser::splitParamLine(const string&  section,
                                               int paramIndex,
                                               ErrorCode* error /*= NULL*/) const
{
    SET_ERROR(error, ErrorCode::Success);
    ParamData paramData;

    auto sectIt = m_iniView.find(section);
    RETURN_DUMMY_IF_ERROR(error, (sectIt == m_iniView.end()) ? ErrorCode::SectionNotFound : ErrorCode::Success, paramData);

    const ParamLinesList& paramLinesList = sectIt->second.paramLinesList;
	int i = 0;
    auto paramListIt = paramLinesList.begin();
    for (; paramListIt != paramLinesList.end(); ++paramListIt, ++i)	{
        if(i == paramIndex) {
            paramData.name = paramListIt->name;
            paramData.value = paramListIt->content.value;
            paramData.description = paramListIt->content.descr;
			break;
        }
    }

    RETURN_DUMMY_IF_ERROR(error, (paramListIt == paramLinesList.end()) ? ErrorCode::ParameterIndexNotFound : ErrorCode::Success, paramData);
    return paramData;
}

//========================================================================
vector<string> IniParser::getSections() const
{
    vector<string> sections;
    sections.resize(m_iniView.size());
    for (auto& sectIt : m_iniView) {
        sections[sectIt.second.pos] = sectIt.first;
    }

    return sections;
}

//========================================================================
bool IniParser::isLoaded()
{
    return m_isLoaded;
}

//========================================================================
