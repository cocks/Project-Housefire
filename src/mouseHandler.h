//
// Project Housefire
// Copyright (C) 2010 LAMMJohnson
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef HOUSEFIRE_MOUSEHANDLER_HPP
#define HOUSEFIRE_MOUSEHANDLER_HPP

#include "application.hpp"
#include <mouseWatcher.h>
#include <collisionRay.h>
#include <collisionTraverser.h>
#include <collisionHandlerQueue.h>
#include <collisionNode.h>

class MouseHandler
{	
public:
	
	static void handle_mouse(const Event* e, void* data);
	
	static void init(Application const& app);
	
private:
	
	MouseHandler();
	
private:
	
	static PT(MouseWatcher) _mouse_watcher;
	static PT(CollisionRay) _picker_ray;
	static CollisionTraverser _traverser;
	static PT(CollisionHandlerQueue) _handler;
	static PT(CollisionNode) _picker_node;
	static NodePath _picker_np;
};

#endif // HOUSEFIRE_MOUSEHANDLER_HPP
