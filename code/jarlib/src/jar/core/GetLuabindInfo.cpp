#include "jar/core/GetLuabindInfo.hpp"
#include <luabind/object.hpp> //for luabind::object, obviously - also include luabind/detail/call.hpp defining luabind::function_object
#include <luabind/function.hpp> //for is_luabind_function

int GetLuabindInfo (lua_State* L)
{
    int numArgs = lua_gettop(L);

    //if there are no arguments, return nil
    if(numArgs == 0)
    {
        lua_pushnil(L);
        return 1;
    }

    //if there is more than 1 argument, silently discard additional ones
    if(numArgs > 1)
    {
        lua_pop(L, numArgs - 1);
    }

    //is it a luabind function?
    if(luabind::detail::is_luabind_function(L, -1))
    {
        //retrieve luabind function object containing the information we want
        assert(lua_getupvalue(L, -1, 1));
        const luabind::detail::function_object* funcObj = *(const luabind::detail::function_object**)lua_touserdata(L, -1);

        lua_pop(L, 2); //upvalue and its origin

        //create table containing information
        luabind::object info = luabind::newtable(L);

        //this is a function
        info["type"] = "function";

        //we know its name
        if(!funcObj->name.empty())
        {
            info["name"] = funcObj->name;
        }

        //iterate through the overloads, save signatures
        luabind::object overloads = luabind::newtable(L);
        int counter = 0;
        for(const luabind::detail::function_object* overload = funcObj; overload != 0; overload = overload->next)
        {
            int top = lua_gettop(L); //sometimes, format_signature pushes multiple strings (e.g. for void Function())
            overload->format_signature(L, (funcObj->name.empty() ? "<unknown>" : funcObj->name.c_str()));
            lua_concat(L, lua_gettop(L) - top); //concatenate if there's more than 1 string
            overloads[++counter] = lua_tostring(L, -1);
            lua_pop(L, 1);
        }
        info["overloads"] = overloads;

        //return gathered information
        info.push(L);
        return 1;
    }

    //is it a luabind class?
    if(luabind::detail::is_class_rep(L, -1)) //this is not exported in the official luabind build but it is in my fork
    {
        const luabind::detail::class_rep* crep = (const luabind::detail::class_rep*)lua_touserdata(L, -1);

        //create table containing information
        luabind::object info = luabind::newtable(L);

        //this is a class
        info["type"] = "class";

        //we know its name
        info["name"] = crep->name();

        luabind::object methods = luabind::newtable(L);
        luabind::object attributes = luabind::newtable(L);

        //return gathered information
        info.push(L);
        return 1;
    }
    //not a luabind object
    lua_pop(L, 1);

    lua_pushnil(L);
    return 1;
}
