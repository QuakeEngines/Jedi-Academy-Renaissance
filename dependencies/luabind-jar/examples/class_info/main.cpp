extern "C"
{
#include "lua.h"
#include "lualib.h"
}

#include <iostream>
#include <luabind/luabind.hpp>
#include <luabind/class_info.hpp>

#include <luabind/object.hpp> //for luabind::object, obviously - also include luabind/detail/call.hpp defining luabind::function_object
#include <luabind/function.hpp> //for is_luabind_function

int pcall_handler(lua_State* /*L*/)
{
	return 1;
}

void dostring(lua_State* state, char const* str)
{
    lua_pushcclosure(state, &pcall_handler, 0);

    if (luaL_loadbuffer(state, str, std::strlen(str), str))
    {
        std::string err("Error loading buffer: ");
        err += lua_tostring(state, -1);
        lua_pop(state, 2);
		throw err;
    }

    if (lua_pcall(state, 0, 0, -2))
    {
        std::string err("Error executing: ");
        err += lua_tostring(state, -1);
        lua_pop(state, 2);
		throw err;
    }

    lua_pop(state, 1);
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
    if(luabind::detail::is_class_rep(L, -1))
    {
        const luabind::detail::class_rep* crep = (const luabind::detail::class_rep*)lua_touserdata(L, -1);

        //create table containing information
        luabind::object info = luabind::newtable(L);

        //this is a class
        info["type"] = "class";

        //we know its name
        info["name"] = crep->name();

        //return gathered information
        info.push(L);
        return 1;
    }
    //not a luabind object
    lua_pop(L, 1);

    lua_pushnil(L);
    return 1;
}

void HelloWorld()
{
    std::cout<<"Hello World!"<<std::endl;
}

void HelloWorld(const std::string& text)
{
    std::cout<<"Hello World! "<<text<<std::endl;
}

void HelloWorld(const int num)
{
    std::cout<<"Hello World! "<<num<<std::endl;
}

struct MyClass
{
    void Hello()
    {
        std::cout<<"My Class says Hello!"<<std::endl;
    }

    void Hello(const std::string& text)
    {
        std::cout<<"My Class says Hello! "<<text<<std::endl;
    }
};

struct MyOtherClass
{
    int num;
};

int main()
{
    lua_State* L = lua_open();
    luaopen_base(L);
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
	 // lua 5.1 or newer
	 luaL_openlibs(L);
#else
	 // lua 5.0.2 or older
    lua_baselibopen(L);
#endif
    luabind::open(L);

    luabind::module(L, "namespace")
    [
        luabind::def("HelloWorld", (void(*)())&HelloWorld),
        luabind::def("HelloWorld", (void(*)(const std::string&))&HelloWorld),
        luabind::def("HelloWorld", (void(*)(const int))&HelloWorld),
        luabind::class_<MyClass>("MyClass")
            .def(luabind::constructor<>())
            .def("Hello", (void(MyClass::*)()) &MyClass::Hello)
            .def("Hello", (void(MyClass::*)(const std::string&)) &MyClass::Hello)
            .scope
            [
                luabind::class_<MyOtherClass>("MyOtherClass")
                    .property("num", &MyOtherClass::num)
            ]
    ];

    lua_register(L, "GetLuabindInfo", &GetLuabindInfo);

    try
    {

        dostring(L,
                 "assert(GetLuabindInfo() == nil)\n"
                 "assert(GetLuabindInfo(1) == nil)\n"
                 "assert(GetLuabindInfo(\"test\") == nil)\n"

                 "testUs = {namespace.HelloWorld, namespace.MyClass, namespace.MyClass.MyOtherClass}\n"
                 "for index, testMe in ipairs(testUs) do\n"
                 " info = GetLuabindInfo(testMe)\n"
                 " if not info then\n"
                 "  print(\"entry \" .. index .. \" is no luabind function/class\")\n"
                 " else\n"
                 "  print(\"entry \" .. index .. \" is a \" .. info.type .. \" called \" .. (info.name or \"\"))\n"
                 "  if info.type == \"function\" then\n"
                 "   print(\"available overloads:\")\n"
                 "   for _, signature in ipairs(info.overloads) do\n"
                 "    print(signature)\n"
                 "   end\n"
                 "  else\n"
                 "   assert(info.type == \"class\")\n"
                 "  end\n"
                 " end\n"
                 " print()\n"
                 "end\n"
                 "namespace.HelloWorld{}\n" //this prints the correct signature in the error
                 );
    }
    catch(std::string err)
    {
        std::cout<<"Caught error: " << err << std::endl;
    }

    lua_close(L);
    return 0;
}
