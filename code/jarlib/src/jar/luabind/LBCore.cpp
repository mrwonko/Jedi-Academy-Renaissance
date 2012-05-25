#include "jar/luabind/Core.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/FileSystem.hpp"
#include "jar/core/Logger.hpp"
#include "jar/core/LuaLogger.hpp"
#include "jar/core/Helpers.hpp"
#include "jar/core/Updatable.hpp"
#include "jar/Core.hpp"
#include "jar/core/CLArguments.hpp"
#include "jar/core/GetLuabindInfo.hpp"
#include <physfs.h> //for PHYSFS_File
#include <unzip.h>
#include <string>

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp> //for pure_out_value
#include <luabind/iterator_policy.hpp>
#include <luabind/object.hpp>

namespace jar
{

namespace
{
    //  Helpers for unz/MiniZip

    int GetCurrentUnzFileContent(unzFile filehandle, std::string& output)
    {
        int status;
        if((status = unzOpenCurrentFile(filehandle)) != UNZ_OK)
        {
            return status;
        }
        char buf[1024];
        while((status = unzReadCurrentFile(filehandle, buf, sizeof(buf))) > 0)
        {
            //status returns number of bytes read
            output += std::string(buf, status);
        }
        unzCloseCurrentFile(filehandle);
        return status;
    }

    struct MyUnzFile
    {
        MyUnzFile(unzFile f) : file(f) {}
        unzFile file;
        operator unzFile() {return file;}
    };

    MyUnzFile MyUnzOpen(const std::string& filename)
    {
        return MyUnzFile(unzOpen((CLArguments::GetSingleton().GetWorkingDirectory() + Core::GetSingleton().GetRootPath() + filename).c_str()));
    }

	// For functions accepting one string as the argument and returning a string vector, turns that into a lua table
	template<const std::vector<std::string> (*function)(const std::string&)> luabind::object StringVectorAsTable(const std::string& argument, lua_State* L)
	{
		luabind::object result = luabind::newtable(L);
		std::vector<std::string> vec = function(argument);
		std::vector<std::string>::iterator end = vec.end();
		unsigned int index = 0;
		for(std::vector<std::string>::iterator it = vec.begin(); it != end; ++it)
		{
			result[++index] = *it;
		}
		return result;
	}
	template<typename T> luabind::object TestTemplate(const std::string& argument, lua_State* L)
	{
		luabind::object result = luabind::newtable(L);
		return result;
	}
}

void BindCore(lua_State* L)
{
    //Didz suggested these functions may be used for evil and should be omitted in C++, instead of just overwritten in Lua.
    luabind::globals(L)["dofile"] = luabind::nil;
    luabind::globals(L)["loadfile"] = luabind::nil;
    luabind::globals(L)["package"]["loaders"] = luabind::nil;

    luabind::module(L)
    [
        // unz - MiniZip

        luabind::namespace_("unz")
        [
            luabind::class_<MyUnzFile>("ZipFile")
                .def("Close", &unzClose)
                .def("LocateFile", &unzLocateFile)
                .def("GetCurrentFileContent", &GetCurrentUnzFileContent),
            luabind::def("Open", &MyUnzOpen)
        ],

        // jar - Jedi Academy: Renaissance

        luabind::namespace_("jar")
        [
            luabind::class_<Logger, LuaLogger>("Logger")
                .def(luabind::constructor<>())
                .def("Log", &Logger::Log, &LuaLogger::WrapLog)
                .def("Warning", &Logger::Warning, &LuaLogger::WrapWarning)
                .def("Error", &Logger::Error, &LuaLogger::WrapError)
                .def("Info", &Logger::Info)
                .def("IncreaseLoggingLevel", &Logger::IncreaseLoggingLevel)
                .def("DecreaseLoggingLevel", &Logger::DecreaseLoggingLevel)
                .def("GetLoggingLevel", &Logger::GetLoggingLevel)
                .def("SetLoggingLevel", &Logger::SetLoggingLevel)
                .scope
                [
                    luabind::def("GetDefaultLogger", &Logger::GetDefaultLogger),
                    luabind::def("SetDefaultLogger", &Logger::SetDefaultLogger)
                ],

            luabind::def("Sleep", &Sleep),
            luabind::def("GetTime", &GetTime),

            luabind::def("GetFilesInDirectory", &StringVectorAsTable<Helpers::GetFilesInDirectory>),
            luabind::def("GetDirectoriesInDirectory", &StringVectorAsTable<Helpers::GetDirectoriesInDirectory>),

            luabind::class_<Core>("Core")
                .def("Update", &Core::Update)
                .scope
                [
                    luabind::def("GetSingleton", &Core::GetSingleton)
                ],

            // jar.fs - FileSystem

            luabind::namespace_("fs")
            [
                luabind::def("GetLastError", &fs::GetLastError),
                luabind::def("Mount", (const bool (*)(const std::string&, const bool)) &fs::Mount),
                luabind::def("Mount", (const bool (*)(const std::string&, const std::string&, const bool)) &fs::Mount),
                luabind::def("OpenRead", &fs::OpenRead),
                luabind::def("OpenWrite", &fs::OpenWrite),
                luabind::def("Unmount", &fs::Unmount),
                luabind::def("GetFilesInDirectory", &StringVectorAsTable<fs::GetFilesInDirectory>),
                luabind::def("GetDirectoriesInDirectory", &StringVectorAsTable<fs::GetDirectoriesInDirectory>),

                luabind::class_<PHYSFS_File>("File")
                    .def("Close", &fs::Close)
                    //chars are read as numbers - keep that in mind!
                    .def("ReadChar", &fs::ReadChar, luabind::pure_out_value(_2))
                    .def("ReadString", (const bool (*)(PHYSFS_File*, std::string&)) &fs::ReadString, luabind::pure_out_value(_2))
                    .def("ReadString", (const bool (*)(PHYSFS_File*, unsigned int, std::string&)) &fs::ReadString, luabind::pure_out_value(_3))
                    .def("ReadFloat", &fs::ReadFloat, luabind::pure_out_value(_2))
                    .def("ReadInt", &fs::ReadInt, luabind::pure_out_value(_2))
                    .def("ReadUnsignedInt", &fs::ReadUnsignedInt, luabind::pure_out_value(_2))
                    //2nd parameter is a pure out reference -> used as return value
                    //i.e. you do "local success, content = file:GetContent()"
                    .def("GetContent", &fs::GetCurrentFileContent, luabind::pure_out_value(_2))
                    .def("EOF", &fs::EndOfFile)
                    .def("Seek", &fs::Seek)
                    .def("Tell", &fs::Tell)
                    .def("WriteString", &fs::WriteString)
                    //TODO: WriteInt etc.
            ]
        ]
    ];

    luabind::globals(L)["unz"]["OK"] = UNZ_OK;
    luabind::globals(L)["unz"]["END_OF_LIST_OF_FILE"] = UNZ_END_OF_LIST_OF_FILE;
    luabind::globals(L)["unz"]["ERRNO"] = UNZ_ERRNO;
    luabind::globals(L)["unz"]["EOF"] = UNZ_EOF;
    luabind::globals(L)["unz"]["PARAMERROR"] = UNZ_PARAMERROR;
    luabind::globals(L)["unz"]["BADZIPFILE"] = UNZ_BADZIPFILE;
    luabind::globals(L)["unz"]["INTERNALERROR"] = UNZ_INTERNALERROR;
    luabind::globals(L)["unz"]["CRCERROR"] = UNZ_CRCERROR;

    //register jar.GetLuabindInfo()
    lua_getglobal(L, "jar"); //push jar table
    assert(lua_istable(L, -1));
    lua_pushcfunction(L, &GetLuabindInfo); //push function
    lua_setfield(L, -2, "GetLuabindInfo"); //add function to table (pops value)
    lua_pop(L, 1); //pop jar table
}

}
