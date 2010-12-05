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


#include "mouseHandler.h"

PT(MouseWatcher) MouseHandler::_mouse_watcher;
PT(CollisionRay) MouseHandler::_picker_ray;
CollisionTraverser MouseHandler::_traverser;
PT(CollisionHandlerQueue) MouseHandler::_handler;
PT(CollisionNode) MouseHandler::_picker_node;
NodePath MouseHandler::_picker_np;

MouseHandler::MouseHandler() {
}

void MouseHandler::init(Application const& app) {
	_traverser = CollisionTraverser("ctraverser");
	_handler = new CollisionHandlerQueue();
	
	_picker_node = new CollisionNode("mouseRay");
	_picker_np = app.get_window()->get_camera_group().attach_new_node (_picker_node);
	_picker_node->set_from_collide_mask(GeomNode::get_default_collide_mask());
	_picker_ray = new CollisionRay();
	_picker_node->add_solid(_picker_ray);
	_handler = new CollisionHandlerQueue();
	_traverser.add_collider(_picker_np, _handler);
	_traverser.show_collisions(app.get_window()->get_render());
	
	_mouse_watcher = DCAST (MouseWatcher, app.get_window()->get_mouse().node());
}

/*
 * Handles click events generated in-game.  Currently moves Gyzweed to the point on the level 
 * that was clicked.
 */
void MouseHandler::handle_mouse(const Event* e, void* data) {
	//TODO: If you click on Gyzweed he will move to the point on his model where the click hit.
	
	Application* app = static_cast< Application* >(data);
	
	if (!_mouse_watcher->has_mouse()){
		// The mouse is probably outside the screen.
		return;
	}
	// This gives up the screen coordinates of the mouse.
	LPoint2f mpos = _mouse_watcher->get_mouse();
	
	// This makes the ray's origin the camera and makes the ray point 
	// to the screen coordinates of the mouse.
	_picker_ray->set_from_lens(app->get_window()->get_camera(0), mpos.get_x(), mpos.get_y());
	
	_traverser.traverse(app->get_ground());
	
	LPoint3f newPos;
	int hits = _handler->get_num_entries();
	if (hits > 0) {
		newPos = _handler->get_entry(0)->get_surface_point(app->get_window()->get_render());
	} else {
		return;
	}
	
	app->get_gyzweed()->walk_to(newPos);
}
