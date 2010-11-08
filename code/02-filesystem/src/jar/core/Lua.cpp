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

#include "jar/core/Lua.hpp"
#include <lua.hpp>
#include <luabind/luabind.hpp> //includes both functions and classes. (function.hpp + class.hpp)

namespace jar
{

Lua::Lua() :
    mState(NULL),
    mIsMainThread(false)
{
    //ctor
}

Lua::~Lua()
{
    //dtor
    //if(mState) is included in Deinit()
    Deinit();
}

const bool Lua::Init()
{
    //try to open the lua state
    mState = lua_open();
    if(!mState)
    {
        //didn't work?
        mLastError = "Could not open lua State!";
        return false;
    }

    //luabind::open may only be called for the main thread. Otherwise it throws an exception.
    mIsMainThread = lua_pushthread(mState) == 1;
    lua_pop(mState, 1);
    if(mIsMainThread)
    {
        luabind::open(mState);
    }
    return true;
}

const bool Lua::OpenLibaries()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    luaL_openlibs(mState);
    return true;
}

const bool Lua::ProtectedCall(Lua::lua_CFunction fn)
{
    int level = lua_gettop(mState);
    lua_pushcfunction(mState, fn);
    int err = lua_pcall(mState, 0, 0, 0);
    if(err != 0)
    {
        if(err == LUA_ERRRUN)
        {
            mLastError = "Runtime error: ";
        }
        else if(err == LUA_ERRMEM)
        {
            mLastError = "Memory allocation error: ";
        }
        else
        {
            assert(err == LUA_ERRERR);
            mLastError = "Error handling error: ";
        }
        mLastError += lua_tostring(mState, -1);
        lua_pop(mState, 1);
        assert(level == lua_gettop(mState)); //stack should be as big as before.
        return false;
    }
    assert(level == lua_gettop(mState));
    return true;
}

const bool Lua::OpenBaseLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_base);
}

const bool Lua::OpenPackageLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_package);
}

const bool Lua::OpenStringLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_string);
    return true;
}

const bool Lua::OpenTableLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_table);
}

const bool Lua::OpenMathLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_math);
}

const bool Lua::OpenIOLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_io);
}

const bool Lua::OpenOSLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_os);
}

const bool Lua::OpenDebugLibrary()
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    return ProtectedCall(luaopen_debug);
}

void Lua::Deinit()
{
    if(mState)
    {
        lua_close(mState);
        mIsMainThread = false;
        mState = NULL;
    }
}

const bool Lua::ExecuteString(const std::string& str, const std::string& chunkname)
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    //I don' do luaL_dostring since I don't want the stack to be filled with return values I need to clean manually. So I just say I expect 0 returns, instead of LUA_MULTRET. Also, I cannot set a chunkname that way.

    //syntax: lua_pcall(lua_State, numArguments, numReturns, errorHandlingFunctionIndex (0 = none))
    if(luaL_loadbuffer(mState, str.c_str(), str.size(), chunkname.c_str())||lua_pcall(mState, 0, 0, 0))
    {
        //return != 0 means error.
        //the error is pushed onto the stack.
        mLastError = lua_tostring(mState, -1);
        return false;
    }
    return true;
}

const bool Lua::ExecuteFile(const std::string& filename)
{
    if(!mState)
    {
        mLastError = "Not initialized!";
        return false;
    }
    //I don' do luaL_dofile since I don't want the stack to be filled with return values I need to clean manually. So I just say I expect 0 returns, instead of LUA_MULTRET.

    //syntax: lua_pcall(lua_State, numArguments, numReturns, errorHandlingFunctionIndex (0 = none))
    if(luaL_loadfile(mState, filename.c_str()) || lua_pcall(mState, 0, 0, 0))
    {
        //returns non-zero on error and pushes the error on the stack.
        mLastError = lua_tostring(mState, -1);
        return false;
    }
    return true;
}

} // namespace jar
