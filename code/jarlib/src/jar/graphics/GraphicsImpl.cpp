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
#include "jar/graphics/GraphicsImpl.hpp"
#include "jar/graphics/RenderWindow.hpp"
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
    BindSFMLGraphics(Core::GetSingleton().GetLua().GetState());
    RenderWindow::Luabind(Core::GetSingleton().GetLua().GetState());
    return true;
}

const bool GraphicsImpl::Deinit()
{
    return true;
}

} // namespace jar
