#include "jar/graphics/Font.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp> //for pure_out_value

namespace jar
{
static short GetFontHeight(const Font& f)
{
    return f.GetFontData().mHeight;// + f.GetFontData().mDescender+1;
}

static short GetFontDefaultSize(const Font& f)
{
    return f.GetFontData().mPointSize;
}

void Font::BindToLua(lua_State* L)
{
    luabind::module(L, "jar")
    [
        luabind::class_<Font>("Font")
            .def(luabind::constructor<>())
            .def("LoadFromFile", &Font::LoadFromFile, luabind::pure_out_value(_3))
            .def("GetWidth", &Font::GetWidth)
            .def("GetTabWidth", &Font::GetTabWidth)
            .def("SetTabWidth", &Font::SetTabWidth)
            .def("GetHeight", &GetFontHeight)
            .def("GetDefaultSize", &GetFontDefaultSize)
    ];
}
}
