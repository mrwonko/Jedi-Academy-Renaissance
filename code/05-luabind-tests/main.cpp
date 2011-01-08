#include <jar/Core.hpp>
#include <jar/Core/CoutLogger.hpp>
#include <jar/Core/Lua.hpp>

#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>

#include <string>
#include <vector>

struct LBTest
{
    //works
    std::vector<std::string> Vector;

    void Add(const std::string& what)
    {
        Vector.push_back(what);
    }

    void Clear()
    {
        Vector.clear();
    }

    //works
    std::vector<std::string>& GetVector()
    {
        return Vector;
    }

    //doesn't work (well, partially...)
    std::vector<std::string> MakeVector()
    {
        return Vector;
    }
};

//this leads to runtime crashes
std::vector<std::string> Vec1()
{
    std::vector<std::string> vec;
    vec.push_back("Hello");
    vec.push_back("World");
    return vec;
}

std::vector<std::string> g_vec;

//this works
const std::vector<std::string>& Vec2()
{
    g_vec.clear();
    g_vec.push_back("Hello");
    g_vec.push_back("World");
    return g_vec;
}

int main(int argc, char** argv)
{
    //create Logger
    jar::CoutLogger logger;
    logger.SetLoggingLevel(5);
    logger.Info("Initialized Logger", 0);

    //initialize core & other components
    jar::Core core;

    if(!core.Init(argc, argv, "./"))
    {
        logger.Error("Could not initialize!");
        return 0;
    }

    luabind::module(core.GetLua().GetState())
    [
        luabind::class_<LBTest>("Test")
            .def(luabind::constructor<>())
            .def_readonly("Vector", &LBTest::Vector, luabind::return_stl_iterator)
            .def("Add", &LBTest::Add)
            .def("Clear", &LBTest::Clear)
            .def("GetVector", &LBTest::GetVector, luabind::return_stl_iterator)
            .def("MakeVector", &LBTest::MakeVector, luabind::return_stl_iterator),

        luabind::def("Vec1", &Vec1, luabind::return_stl_iterator),
        luabind::def("Vec2", &Vec2, luabind::return_stl_iterator)
    ];

    std::string code = "--lua test stuff\n\
print(\"===Lua Test code goes here===\") \n\
 \n\
local t = Test() \n\
t:Add(\"Rofl\") \n\
t:Add(\"Lol\") \n\
t:Add(\"Lmao\") \n\
 \n\
print(\"==Test 1==\") \n\
for s in t.Vector do \n\
  print(s) \n\
end \n\
 \n\
print(\"==Test 2==\") \n\
for s in t:GetVector() do \n\
  print(s) \n\
end \n\
 \n\
print(\"==Test 3==\") \n\
for s in t:MakeVector() do \n\
  print(s) \n\
end \n\
 \n\
--print(\"==Test 4==\") \n\
--for s in Vec1() do \n\
--  print(s) \n\
--end \n\
 \n\
print(\"==Test 5==\") \n\
for s in Vec2() do \n\
  print(s) \n\
end \n\
";

    if(!core.GetLua().ExecuteString(code, "Testcode"))
    {
        logger.Error(core.GetLua().GetLastError());
    }

    return 0;
}
