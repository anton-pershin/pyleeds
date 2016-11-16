//===========================================================================
#ifndef stringtools_lightH
#define stringtools_lightH
//========================================================================
#include "libtools_light.h"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
//========================================================================
namespace thequicklight {
    namespace str {
        /*!
        Convert a type into string. All internal types are supported
        */
        template<class SomeType>
        std::string toString(SomeType value)
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        /*!
        Explicit specialization of function toString() for bool
        */
        template<>
        MAC_DLLEXPORT std::string toString(bool value);

        /*!
        Convert string into some type. All internal types are supported
        */
        template<class SomeType>
        SomeType fromString(const std::string& str, bool* ok = 0)
        {
            SomeType value;
            std::stringstream ss(str);
            ss >> value;
            if (ok)
                *ok = (ss.fail() == false);
            return value;
        }

        /*!
        Explicit specialization of function fromString() for bool
        */
        template<>
        MAC_DLLEXPORT bool fromString<bool>(const std::string& str, bool* ok);

        /*!
        Remove comments from a string
        \param[in] sourceStr Source string with a comment inside
        \param[in] commentSeparator Comment separator
        \param[out] destStr Destination string without comment and separator
        */
        MAC_DLLEXPORT bool removeComments(const std::string& sourceStr, const std::string& commentSeparator,
                                          std::string& destStr);

        /*!
        Remove whitespaces from the left and the right
        */
        MAC_DLLEXPORT std::string trim(std::string str, std::string whitespaces = " \t\n\r");

        /*!
        Remove whitespaces from the left
        */
        MAC_DLLEXPORT std::string ltrim(std::string str, std::string whitespaces = " \t\n\r");

        /*!
        Remove whitespaces from the right
        */
        MAC_DLLEXPORT std::string rtrim(std::string str, std::string whitespaces = " \t\n\r");

        /*!
        Replace all inclusions of substring
        \param[in] str Source string
        \param[in] from Substring to search for
        \param[in] to Substring to replace on
        */
        MAC_DLLEXPORT std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

        /*!
        Split a pair of the form "param=value" into separate strings "param" and "value"
        \param[in] delimeter Delimiter between param and value
        \todo Rename if a function splits into two substring by delimiter
        */
        MAC_DLLEXPORT bool paramValueSplit(const std::string& pair, const std::string& delimeter,
                                           std::string &param, std::string &value);
        /*!
        Overload for paramValueSplit for char delimeter
        */
        MAC_DLLEXPORT bool paramValueSplit(const std::string& pair, char delimeter,
                                           std::string &param, std::string &value);

        /*!
        Split string in a list of substrings by a delimiter
        */
        MAC_DLLEXPORT bool splitString(const std::string& str, char delimeter, std::vector< std::string >& output);
    }
}

//===========================================================================
#endif
//===========================================================================
