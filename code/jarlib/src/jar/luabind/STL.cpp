/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "jar/luabind/STL.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <string>
#include <vector>

namespace jar
{

/** luabind doesn't recognize the return value of std::vector<std::string>::at as a string, thus I need this helper function.
    That's all right since I need to change it to return nil on invalid indices anyway. **/
//note: I get an error if the return type is an std string* complaining about unregistered types. bullshit. anyway, using const char* instead works.
static const char * StringVectorAt(std::vector<std::string>& vec, unsigned int index)
{
    if(index >= vec.size()) return NULL;
    return vec[index].c_str();
}

void BindSTL(lua_State* L)
{
    luabind::module(L, "std")
    [
        luabind::class_<std::vector<std::string> >("stringvector")
            .def("at", &StringVectorAt)
            .def("__len", &std::vector<std::string>::size)
            .def("size", &std::vector<std::string>::size)
    ];
}

}
