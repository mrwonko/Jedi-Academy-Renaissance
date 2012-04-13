#include "jar/core/GetLuabindInfo.hpp"
#include <luabind/object.hpp> //for luabind::object, obviously - also include luabind/detail/call.hpp defining luabind::function_object
#include <luabind/function.hpp> //for is_luabind_function

#include <cassert>

namespace jar
{

namespace
{
    //returns a lua table containing information on the luabind function on the stack top
    luabind::object GetLuabindFunctionInfo(lua_State* L)
    {
#ifdef _DEBUG
        int stacktop = lua_gettop(L);
#endif
        //retrieve luabind function object containing the information we want
        assert(lua_getupvalue(L, -1, 1));

        const luabind::detail::function_object** funcObjPtr = (const luabind::detail::function_object**)lua_touserdata(L, -1);
        assert(funcObjPtr);
        const luabind::detail::function_object* funcObj = *funcObjPtr;

        lua_pop(L, 1); //upvalue

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

#ifdef _DEBUG
        assert(stacktop == lua_gettop(L));
#endif

        return info;
    }
}

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
        GetLuabindFunctionInfo(L).push(L);
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

        //get static members (functions, classes, whatever there may be)
        luabind::object staticMemberFunctions = luabind::newtable(L);
        luabind::object classScope = luabind::newtable(L);
        {
            int staticMemberFunctionCounter = 0;
            //copy the whole default table into scope
            crep->get_default_table(L); // stack: crep default table
            int index = lua_gettop(L);
            lua_pushnil(L); //initialize lua_next - stack: crep default table, nil
            //iterate through table (in no particular order) - pops key, pushes next key/value pair returning 1 (or 0 and pushing nothing once done)
            while(lua_next(L, index) != 0)
            {
#ifdef _DEBUG
                int stacksize = lua_gettop(L);
#endif
                //stack: crep default table, key, value

                if(luabind::detail::is_luabind_function(L, -1))
                {
                    luabind::object funcInfo = GetLuabindFunctionInfo(L);
                    funcInfo["actualName"] = lua_tostring(L, -2);
                    staticMemberFunctions[++staticMemberFunctionCounter] = funcInfo;
                    lua_pop(L, 1);
                    //stack: crep default table, key
                }
                else
                {
                    classScope.push(L);
                    //stack: crep default table, key, value, class scope table
                    lua_pushvalue(L, -3);
                    lua_pushvalue(L, -3);
                    //stack: crep default table, key, value, target table, key, value
                    lua_settable(L, -3);
                    //stack: crep default table, key, value, target table
                    lua_pop(L, 2);
                    //stack: crep default table, key
                }

#ifdef _DEBUG
                assert(stacksize - 1 == lua_gettop(L));
#endif
            }
            //stack: crep default table
            lua_pop(L, 1); //crep default table
        }

        info["staticMemberFunctions"] = staticMemberFunctions;
        info["scope"] = classScope;

        //get static constants (from enums)
        luabind::object constants = luabind::newtable(L);
        {
            int counter = 0;
            for(std::map<const char*, int, luabind::detail::ltstr>::const_iterator it = crep->get_static_constants().begin(); it != crep->get_static_constants().end(); ++it) //this does not exist in the official luabind build but it does in my fork
            {
                luabind::object constantInfo = luabind::newtable(L);
                constantInfo["name"] = it->first;
                constantInfo["value"] = it->second;
                constants[++counter] = constantInfo;
            }
        }
        info["constants"] = constants;

        //get member functions & properties
        luabind::object methods = luabind::newtable(L);
        luabind::object properties = luabind::newtable(L);

        {
            int counterM = 0;
            int counterP = 0;
            // push table on stack
            crep->get_table(L);
            int index = lua_gettop(L);
            lua_pushnil(L); //initialize lua_next
            //iterate through table (in no particular order) - pops key, pushes next key/value pair returning 1 (or 0 and pushing nothing once done)
            while(lua_next(L, index) != 0)
            {
                //is it a property?
                if(lua_tocfunction(L, -1) == &luabind::detail::property_tag)
                {
                    //unfortunately we cannot find out much more than the name of a property...
                    luabind::object propInfo = luabind::newtable(L);
                    propInfo["name"] = lua_tostring(L, -2);
                    properties[++counterP] = propInfo;
                }
                else //no property, probably a function
                {
                    if(luabind::detail::is_luabind_function(L, -1))
                    {
                        luabind::object funcInfo = GetLuabindFunctionInfo(L);
                        methods[++counterM] = funcInfo;
#ifdef _DEBUG
                        std::string keyName(lua_tostring(L, -2));
                        assert(keyName == funcInfo["name"]);
#endif
                    }
                    else
                    {
                        //stuff in get_table that is no luabind function has most likely been added from Lua and is basically a static member.
                        classScope.push(L);
                        //stack: crep table, key, value, scope table
                        lua_pushvalue(L, -3);
                        //stack: crep table, key, value, scope table, key
                        lua_pushvalue(L, -3);
                        //stack: crep table, key, value, scope table, key, value
                        lua_settable(L, -3);
                        //stack: crep table, key, value, scope table
                        lua_pop(L, 1);
                        //stack: crep table, key, value
                    }
                }
                //pop value from stack (key is consumed by next)
                lua_pop(L, 1);
                //stack: crep table, key
            }
            lua_pop(L, 1); //pop crep table
        }
        info["properties"] = properties;
        info["methods"] = methods;

        //return gathered information
        info.push(L);
        return 1;
    }
    //not a luabind object
    lua_pop(L, 1);

    lua_pushnil(L);
    return 1;
}

}
