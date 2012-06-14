#include "jar/graphics/SimpleLevel.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/object.hpp>

namespace jar
{

namespace
{
    luabind::object SimpleLevelGetEntityInfo(const SimpleLevel& level, lua_State* L)
    {
        luabind::object table = luabind::newtable(L);
        unsigned int index = 0;
        SimpleLevel::EntityList::const_iterator end = level.GetEntities().end();
        for(SimpleLevel::EntityList::const_iterator it = level.GetEntities().begin(); it != end; ++it)
        {
            luabind::object entity = luabind::newtable(L);
            if(!it->classname.empty()) entity["classname"] = it->classname;
            SimpleLevel::Entity::PropertyMap::const_iterator end = it->properties.end();
            for(SimpleLevel::Entity::PropertyMap::const_iterator curProperty = it->properties.begin(); curProperty != end; ++curProperty)
            {
                entity[curProperty->first] = curProperty->second;
            }
            table[++index] = entity;
        }
        return table;
    }
}

void SimpleLevel::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<SimpleLevel>("SimpleLevel")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &SimpleLevel::LoadFromFile, luabind::pure_out_value(_3))
            .def("GetEntities", &SimpleLevelGetEntityInfo)
            .def("UploadToGPU", &SimpleLevel::UploadToGPU, luabind::pure_out_value(_2))
            .def("DeleteFromGPU", &SimpleLevel::DeleteFromGPU, luabind::pure_out_value(_2))
            .def("Render", &SimpleLevel::Render, luabind::pure_out_value(_2))
    ];
}

}
