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

#include "jar/Core.hpp"
#include "jar/core/Lua.hpp"
#include "jar/core/Logger.hpp"
#include "jar/graphics/GraphicsImpl.hpp"
#include "jar/graphics/RenderWindow.hpp"
#include "jar/graphics/Font.hpp"
#include "jar/graphics/Text.hpp"
#include "jar/luabind/SFMLGraphics.hpp"

namespace jar {

GraphicsImpl::GraphicsImpl()
{
    //ctor
}

GraphicsImpl::~GraphicsImpl()
{
    //dtor
}

const bool GraphicsImpl::Init()
{
    Logger::GetDefaultLogger().Info("- Initializing Graphics component...", 1);
    lua_State* L = Core::GetSingleton().GetLua().GetState();
    BindSFMLGraphics(L);
    RenderWindow::Luabind(L);
    Font::BindToLua(L);
    Text::BindToLua(L);
    Logger::GetDefaultLogger().Info("Exposed Graphics System to Lua", 2);
    Logger::GetDefaultLogger().Info("- Graphics component successfully initialized.", 1);
    return true;
}

const bool GraphicsImpl::Deinit()
{
    return true;
}

} // namespace jar
