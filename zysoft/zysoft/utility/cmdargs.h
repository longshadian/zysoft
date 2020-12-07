/* /////////////////////////////////////////////////////////////////////////
 * File:        stlsoft/system/cmdargs.hpp
 *
 * Purpose:     Command-line sequences class.
 *
 * Created:     25th June 2005
 * Updated:     4th November 2015
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 2005-2015, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <vector>

namespace zysoft
{

class basic_cmdargs
{
public:
    typedef std::string             string_type;
    typedef std::string         string_view_type;

    enum
    {
        singleDash = 1
        , doubleDash = 2
        , tripleDash = 3
    };

    /// Represents a command-line option
    ///
    /// \warning The value of the <code>original</code> member will be
    ///   undefined if a copy of the instance is kept beyond the lifetime of
    ///   the cmdargs instance from which it was obtained.
    struct option
    {
        string_type         name;
        string_type         value;
        int                 type;
        int                 index;
        string_view_type    original;

        option()
            : name()
            , value()
            , type(-1)
            , index(-1)
            , original()
        {}
        option(string_type const& n, string_type const& v, int t, int i, string_view_type const& o)
            : name(n)
            , value(v)
            , type(t)
            , index(i)
            , original(o)
        {}

        inline std::string to_string() const
        {
            std::ostringstream stm;
            static const char   s_dashes[] = { '-' ,   '-' ,   '\0' };
            char const* dashes = &s_dashes[(this->type == basic_cmdargs::singleDash)];
            if (this->value.empty()) {
                stm << dashes << this->name;
            } else {
                stm << dashes << this->name << '=' << this->value;
            }
            return stm.str();
        }
    };

    /// Represents a command-line value
    ///
    /// \warning The value of the <code>name</code> member will be undefined
    ///   if a copy of the instance is kept beyond the lifetime of the
    ///   cmdargs instance from which it was obtained.
    struct value
    {
        string_view_type    name;
        int                 index;

        value()
            : name()
            , index(-1)
        {}
        value(string_view_type const& v, int i)
            : name(v)
            , index(i)
        {}

        inline std::string to_string() const
        {
            std::ostringstream stm;
            stm << this->name;
            return stm.str();
        }
    };

    typedef option                                  options_value_type;
    typedef value                                   values_value_type;

    typedef std::vector<options_value_type>         options_type;
    typedef std::vector<values_value_type>          values_type;

    typedef typename options_type::const_iterator   const_options_iterator;
    typedef typename values_type::const_iterator    const_values_iterator;

    typedef std::size_t                             size_type;
    typedef bool                                    bool_type;

public:
    basic_cmdargs(int argc, char /*const*/** argv);
    ~basic_cmdargs();

public:
    options_type const& options() const { return m_options; }
    values_type const& values() const { return m_values; }

    const_options_iterator  options_begin() const { return m_options.begin(); }
    const_options_iterator  options_end() const { return m_options.end(); }

    const_values_iterator   values_begin() const { return m_values.begin(); }
    const_values_iterator   values_end() const { return m_values.end(); }

    size_type   options_size() const { return m_options.size();} 
    size_type   values_size() const { return m_values.size(); }
    size_type   size() const { return options_size() + values_size(); }
    bool_type   empty() const { return 0 == size(); }

    /// Determines whether the options collection contains an option of
    ///  the given name
    ///
    /// \param optionName The name of the option to search for
    /// \param type The type of the option (i.e the number of hyphens). It
    ///    defaults to -1, which indicates that the caller does not care.
    template <typename S>
    bool_type   has_option(S const& optionName, int type = -1) const
    {
        return m_options.end() != has_option_(c_str_data(optionName), type);
    }

    /// Determines whether the options collection contains an option of
    ///  the given name, and copies the found option's details into a
    ///  caller-supplied instance
    ///
    /// \param optionName The name of the option to search for
    /// \param type The type of the option (i.e the number of hyphens). The
    ///    caller can specify -1 to indicate that it does not care.
    /// \param opt The instance into which the
    template <typename S>
    bool_type   has_option(S const& optionName, int type, option& opt) const
    {
        const_options_iterator it = has_option_(c_str_data(optionName), type);
        if (m_options.end() != it) {
            opt = *it;
            return true;
        }
        return false;
    }

    template <typename S>
    bool_type   has_value(S const& valueName) const
    {
        return m_values.end() != has_value_(c_str_data(valueName));
    }

    template <typename S>
    bool_type   has_value(S const& valueName, value& val) const
    {
        const_values_iterator it = has_value_(c_str_data(valueName));
        if (m_values.end() != it) {
            val = *it;
            return true;
        }
        return false;
    }
private:
    const_values_iterator has_value_(char const* valueName) const;
    const_options_iterator has_option_(char const* optionName, int type) const;

private:
    options_type    m_options;
    values_type     m_values;

#if 1
    static inline const char* c_str_data(const std::string& s)
    {
        return s.c_str();
    }

    static inline const char* c_str_data(const char* s)
    {
        return s;
    }

    static inline const wchar_t* c_str_data(const std::wstring& s)
    {
        return s.c_str();
    }

    static inline const wchar_t* c_str_data(const wchar_t* s)
    {
        return s;
    }

    static inline std::size_t c_str_len(const std::string& s)
    {
        return s.length();
    }

    static inline std::size_t c_str_len(const char* s)
    {
        return std::strlen(s);
    }

    static inline std::size_t c_str_len(const std::wstring& s)
    {
        return s.length();
    }

    template<typename S1 , typename S2 , typename C >
    static inline std::size_t split(S1 const& s, C delim , S2& s0 , S2& s1)
    {
        return split_impl(c_str_data(s), c_str_len(s), delim, s0, s1);
    }

    template <typename S , typename C>
    static inline std::size_t split_impl(const C* s, std::size_t n, C delim, S& s0, S& s1)
    {
        const C* const  b = s;
        const C* const  e = s + n;
        const C* it0 = b;
        const C* it1 = std::find(it0, e, delim);
        std::size_t     cnt = (it1 != e) ? 1 : 0;
        s0 = S(it0, it1);

        if (e != it1) {
            ++cnt;
            it0 = ++it1;
            it1 = e;
        }
        else {
            it0 = it1;
        }
        s1 = S(it0, it1);
        return cnt;
    }
#endif
};

inline basic_cmdargs::basic_cmdargs(int argc, char /*const*/** argv)
{
    for (int i = 1; i < argc; ++i) {
        char const* arg = argv[i];
        if ('-' == arg[0]) {
            ++arg;
            const int   type = ('-' != arg[0]) ? singleDash : (++arg, doubleDash);
            string_type s0;
            string_type s1;
            split(arg, '=', s0, s1);
            m_options.push_back(option(s0, s1, type, i, argv[i]));
        } else {
            m_values.push_back(value(arg, i));
        }
    }
}

inline basic_cmdargs::~basic_cmdargs()
{
}

inline  basic_cmdargs::values_type::const_iterator 
    basic_cmdargs::has_value_(char const* valueName) const
{
    assert(valueName);
    typename values_type::const_iterator b;
    for (b = m_values.begin(); b != m_values.end(); ++b) {
        if ((*b).name == valueName) {
            break;
        }
    }
    return b;
}

inline  basic_cmdargs::options_type::const_iterator 
    basic_cmdargs::has_option_(char const* optionName, int type) const
{
    assert(optionName);
    typename options_type::const_iterator b;
    for (b = m_options.begin(); b != m_options.end(); ++b) {
        if ((*b).name == optionName && 
            (-1 == type || type == (*b).type)) {
            break;
        }
    }
    return b;
}

#if 0
template <typename S, typename Args>
inline S& operator <<(S& stm, typename Args::option const& opt)
{
    static const char   s_dashes[] =
    {
            '-'
        ,   '-'
        ,   '\0'
    };
    char const* dashes = &s_dashes[(opt.type == Args::singleDash)];

    if (opt.value.empty()) {
        stm << dashes << opt.name;
    }
    else {
        stm << dashes << opt.name << '=' << opt.value;
    }

    return stm;
}

template <typename S, typename Args>
inline S& operator <<(S& stm, typename Args::value const& val)
{
    stm << val.name;
    return stm;
}

#else
inline std::ostream& operator <<(std::ostream& stm,  basic_cmdargs::option const& option)
{
    static const char   s_dashes[] =
    {
            '-'
        ,   '-'
        ,   '\0'
    };
    char const* dashes = &s_dashes[(option.type == basic_cmdargs::singleDash)];

    if (option.value.empty()) {
        stm << dashes << option.name;
    }
    else {
        stm << dashes << option.name << '=' << option.value;
    }

    return stm;
}

inline std::ostream& operator <<(std::ostream& stm, basic_cmdargs::value const& value)
{
    stm << value.name;
    return stm;
}
#endif

} // namespace zysoft


