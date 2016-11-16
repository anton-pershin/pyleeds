//========================================================================
#ifndef iniparser_lightH
#define iniparser_lightH
//========================================================================
#include "stringtools_light.h"
#include "libtools_light.h"

#include <string>
#include <fstream>
#include <list>
#include <set>
#include <map>
#include <algorithm>

#define SET_ERROR(ptr, val) if (ptr) *ptr = val
#define RETURN_DUMMY_IF_ERROR(ptr, err, dummy)\
    {\
        thequicklight::IniParser::ErrorCode err__ = err;\
        if (err__ != thequicklight::IniParser::ErrorCode::Success)\
        {\
            SET_ERROR(ptr, err__);\
            return dummy;\
        }\
    }

//========================================================================
namespace thequicklight {
    /*!
    \brief Parser of aINI-files

    It allows to laod and save files made up in aINI format.
    This light version supports only one complex type of aINI format, namely array
    */
    class MAC_DLLEXPORT IniParser {

        /// A pair "section-parameter"
        struct ParamAddress {
            ParamAddress(const std::string& section, const std::string& paramName)
                : paramName(paramName)
                , section(section)
            {}

            std::string section;
            std::string paramName;
        };

        /// Data corresponding to a parameter name
        struct ParamContent {
            std::string value;
            std::string descr;
        };

        /// Data corresponding to one line of "param=value//descr"
        /// \todo dublicate the information in map. Must rewrite on Boost.MultiIndex
        struct ParamLine {
            ParamLine(const std::string& paramName, const ParamContent& paramContent)
                : name(paramName)
                , content(paramContent)
            {}

            std::string name;
            ParamContent content;
        };

        typedef std::list< ParamLine > ParamLinesList;

        /// Mapping between parameter name and overall content of the line
        //			ParamName   ParamContent
        typedef std::map< std::string, ParamContent >  ParamLinesMap;

        /// Section data
        struct Section {
            ParamLinesMap paramLines;
            std::string comment;
            ParamLinesList paramLinesList;
            int pos;
        };

        /// Mapping between section name and overall content of the section.
        /// Due to the inner hierarchy of Section, this type sets a view of the whole aINI-file
        //			 Section	Data
        typedef std::map<std::string, Section> IniView;

    public:
        /// Parser errors
        enum class ErrorCode {
            Success,
            FileNotFound,
            FileCannotBeOpened,
            InvalidString,
            SectionNotFound,
            ParameterNotFound,
            ParameterIndexNotFound,
            IncorrectParameterType,
            ValueParsingError,
            BadArrayConversion,
            BadSaving,
            UnknownError
        };

        /// Available parameter types
        enum class ParamType {
            String,
            Array, ///< Example: (abc,def,ghi)
        };

        /// All data correspnding to one line
        /// \todo: duplicate ParamLine, should be removed
        struct ParamData {
            std::string name;
            std::string value;
            std::string description;
        };

    public:
        /*!
        Takes file name and attempts to open and parse it
        */
        IniParser(const std::string& filename);

        ~IniParser();

        /*!
        Checks if a file has been correctly loaded
        */
        bool isLoaded();

        /*!
        Returns current set file name
        \return Имя файла
        */
        std::string getFilename() const;

        /*!
        Sets new file name and parses it
        */
        ErrorCode setFilename(const std::string& iniFilename);

        /*!
        Saves all changes made via setOptionValue method to the opened file
        */
        ErrorCode save();

        /*!
        Returns all lines ("script") corresponding to a given section
        \param[in] removeSubcomments flag, whether to delete comments from lines
        */
        ErrorCode getScriptFromFile(const std::string& section,
                                    std::list<std::string>& script,
                                    bool removeSubcomments = true ) const;

        std::vector<std::string> getSections() const;

        /*!
        Returns a list of parameters contained in a given section
        */
        std::vector<std::string> getParamNames(const std::string& section, ErrorCode* error = NULL) const;

        bool sectionExists(const std::string& section) const;

        /*!
        Returns a comment to parameter. For example, "comment" for a line "param = value // comment"
        */
        std::string getParamDescription(const std::string& section,
                                        const std::string& paramName,
                                        ErrorCode* error) const;

        /*!
        Returns a comment to section. For example, "comment" for a line "[section] // comment"
        */
        std::string getSectionDescription(const std::string& section, ErrorCode* error) const;

        ErrorCode setValue(const std::string& section,
                           const std::string& paramName,
                           const std::string& newValue);

        /*!
        Template method returning different representations of values.
        There are specializations for all arithmetic types, bool, string, std::vector<double> and std::vector<string>.
        Generally, any value can be represented as a string.
        */
        template <class T>
        T getValue(const std::string& section,
                   const std::string& paramName,
                   ErrorCode* error = NULL) const;

        /*!
        Returns parameter data corresponding to its index (i.e, line number in the section)
        */
        ParamData splitParamLine(const std::string& section,
                                 int paramIndex,
                                 ErrorCode* error = NULL) const;

    private:
        ErrorCode initParser(const std::string& filename);
        ErrorCode loadIniFile();
        std::string removeSubComment(const std::string& source);
        ErrorCode splitRawParamLine(const std::string& paramLine, ParamContent& paramContent, std::string& paramName) const;
        ErrorCode getParamContent(const std::string& section, const std::string& paramName, ParamContent& paramContent) const;
        template <class T>
        ErrorCode getArithmeticValue(const std::string& section,
                                     const std::string& paramName,
                                     T& val) const;

        template <class T>
        std::vector<T> parseArrayLikedValue(const std::string& value, ErrorCode* error = NULL) const;

    private:
        std::string m_iniFilename;
        IniView m_iniView;
        std::map<int, std::string> m_rawStrings; // raw strings which are not parsed (blank lines, comments above/below lines and section), but have to be saved in the file
        bool m_isIniViewModified;
        bool m_isLoaded;
    };
}

//========================================================================
template <class T>
thequicklight::IniParser::ErrorCode thequicklight::IniParser::getArithmeticValue(const std::string& section,
                                                                                 const std::string& paramName,
                                                                                 T& val) const
{
    ErrorCode err = ErrorCode::Success;
    ParamContent paramContent;
    err = getParamContent(section, paramName, paramContent);
    val = thequicklight::str::fromString<T>(paramContent.value);
    return err;
}

//========================================================================
/*!
Explicit specializations of getValue() for arithmetic types
\todo rewrite on enable_if via traits
*/
namespace thequicklight {
    template <>
    char IniParser::getValue<char>(const std::string& section,
                                   const std::string& paramName,
                                   ErrorCode* error /*= NULL*/) const
    {
        char res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    unsigned char IniParser::getValue<unsigned char>(const std::string& section,
                                                     const std::string& paramName,
                                                     ErrorCode* error /*= NULL*/) const
    {
        unsigned char res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    short IniParser::getValue<short>(const std::string& section,
                                     const std::string& paramName,
                                     ErrorCode* error /*= NULL*/) const
    {
        short res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    unsigned short IniParser::getValue<unsigned short>(const std::string& section,
                                                       const std::string& paramName,
                                                       ErrorCode* error /*= NULL*/) const
    {
        unsigned short res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    int IniParser::getValue<int>(const std::string& section,
                                 const std::string& paramName,
                                 ErrorCode* error /*= NULL*/) const
    {
        int res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    unsigned int IniParser::getValue<unsigned int>(const std::string& section,
                                                   const std::string& paramName,
                                                   ErrorCode* error /*= NULL*/) const
    {
        unsigned int res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    long IniParser::getValue<long>(const std::string& section,
                                   const std::string& paramName,
                                   ErrorCode* error /*= NULL*/) const
    {
        long res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    unsigned long IniParser::getValue<unsigned long>(const std::string& section,
                                                     const std::string& paramName,
                                                     ErrorCode* error /*= NULL*/) const
    {
        unsigned long res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    float IniParser::getValue<float>(const std::string& section,
                                     const std::string& paramName,
                                     ErrorCode* error /*= NULL*/) const
    {
        float res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    double IniParser::getValue<double>(const std::string& section,
                                       const std::string& paramName,
                                       ErrorCode* error /*= NULL*/) const
    {
        double res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    template <>
    long double IniParser::getValue<long double>(const std::string& section,
                                                 const std::string& paramName,
                                                 ErrorCode* error /*= NULL*/) const
    {
        long double res;
        SET_ERROR(error, getArithmeticValue(section, paramName, res));
        return res;
    }

    //========================================================================
    /*!
    Explicit specialization of getValue() for strings
    */
    template <>
    std::string IniParser::getValue<std::string>(const std::string& section,
                                                 const std::string& paramName,
                                                 ErrorCode* error /*= NULL*/) const
    {
        SET_ERROR(error, ErrorCode::Success);
        ParamContent paramContent;
        RETURN_DUMMY_IF_ERROR(error, getParamContent(section, paramName, paramContent), "");
        return paramContent.value;
    }

    //========================================================================
    /*!
    Explicit specialization of getValue() for an array of real numbers
    */
    template <>
    std::vector<double> IniParser::getValue< std::vector<double> >(const std::string& section,
                                                                   const std::string& paramName,
                                                                   ErrorCode* error /*= NULL*/) const
    {
        SET_ERROR(error, ErrorCode::Success);
        std::vector<double> array;
        ErrorCode tempErr = ErrorCode::Success;
        std::string rawStr = getValue<std::string>(section, paramName, &tempErr);
        RETURN_DUMMY_IF_ERROR(error, tempErr, array);
        array = parseArrayLikedValue<double>(rawStr, &tempErr);
        SET_ERROR(error, tempErr);

        //array.resize(valuesList.size());
        //std::transform(valuesList.begin(), valuesList.end(), array.begin(),
        //            [&](std::string str) -> double {convertDecimalSeparator(str); return thequick::str::fromString<double>(str);});

        return array; // no copying since C++11 implies move-constructor while returning
    }

    //========================================================================
    /*!
    Explicit specialization of getValue() for an array of strings
    */
    template <>
    std::vector<std::string> IniParser::getValue< std::vector<std::string> >(const std::string& section,
                                                                             const std::string& paramName,
                                                                             ErrorCode* error /*= NULL*/) const
    {
        SET_ERROR(error, ErrorCode::Success);
        std::vector<std::string> array;
        ErrorCode tempErr = ErrorCode::Success;
        std::string rawStr = getValue<std::string>(section, paramName, &tempErr);
        RETURN_DUMMY_IF_ERROR(error, tempErr, array);
        array = parseArrayLikedValue<std::string>(rawStr, &tempErr);
        SET_ERROR(error, tempErr);
        return array; // no copying since C++11 implies move-constructor while returning
    }

    //========================================================================
    template <class T>
    T IniParser::getValue(const std::string& section,
                          const std::string& paramName,
                          IniParser::ErrorCode* error /*= NULL*/) const
    {
        SET_ERROR(error, ErrorCode::Success);
        ParamContent paramContent;
        RETURN_DUMMY_IF_ERROR(error, getParamContent(section, paramName, paramContent), T());
        T res = thequicklight::str::fromString<T>(paramContent.value);
        return res;
    }

    //========================================================================
    template <class T>
    std::vector<T> IniParser::parseArrayLikedValue(const std::string& value, ErrorCode* error) const
    {
        std::vector<T> array;
        // standard layout of array: (abc, def, ghi)
        std::string arrayStr = thequicklight::str::trim(value);
        RETURN_DUMMY_IF_ERROR(error, (arrayStr[0] == '(') ? ErrorCode::Success : ErrorCode::BadArrayConversion, array);
        int curElementBeg = 0;
        for (int i = 0; i < arrayStr.size(); ++i) {
            if (arrayStr[i] == '(') {
                curElementBeg = i + 1;
            }
            else if (arrayStr[i] == ',') {
                array.push_back(thequicklight::str::fromString<T>(arrayStr.substr(curElementBeg, i - curElementBeg - 1)));
            }
            else if (arrayStr[i] == ')') {
                array.push_back(thequicklight::str::fromString<T>(arrayStr.substr(curElementBeg, i - curElementBeg - 1)));
                break;
            }
        }

        return array;
    }
}

//========================================================================
#endif
//===========================================================================
