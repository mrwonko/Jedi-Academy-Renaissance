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

#include "jar/luabind/Core.hpp"
#include "jar/luabind/FileSystem.hpp"
#include "jar/luabind/STL.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/Core.hpp"

#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace jar
{

void BindCore(lua_State* L)
{
    //Didz suggested these functions may be used for evil and should be omitted in C++, instead of just overwritten in Lua.
    luabind::globals(L)["dofile"] = 0;
    luabind::globals(L)["loadfile"] = 0;
    luabind::globals(L)["package"]["loaders"] = 0;

    luabind::module(L, "jar")
    [
        luabind::class_<Logger>("Logger")
            .scope
            [
                luabind::def("GetDefaultLogger", &Logger::GetDefaultLogger)
            ]
            .def("Log", &Logger::Log)
            .def("Warning", &Logger::Warning)
            .def("Error", &Logger::Error)
            .def("Info", &Logger::Info)
            .def("IncreaseLoggingLevel", &Logger::IncreaseLoggingLevel)
            .def("DecreaseLoggingLevel", &Logger::DecreaseLoggingLevel)
            .def("GetLoggingLevel", &Logger::GetLoggingLevel)
            .def("SetLoggingLevel", &Logger::SetLoggingLevel),

        luabind::def("GetFilesInDirectory", &Helpers::GetFilesInDirectory),
        luabind::def("GetDirectoriesInDirectory", &Helpers::GetDirectoriesInDirectory),

        luabind::class_<Core>("Core")
            .def("Update", &Core::Update)
            .scope
            [
                luabind::def("GetSingleton", &Core::GetSingleton)
            ]
    ];

    BindFileSystem(L);
    BindSTL(L);
}

}
