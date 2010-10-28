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

#include "gyzweed.hpp"

Gyzweed::Gyzweed(Application const& application)
: _app(application) {
	_node_path = _app.get_window()->load_model(_app.get_framework()->get_models(), "ralph");
	_app.get_window()->load_model(_node_path, "ralph-run");
	
	_node_path.reparent_to(_app.get_window()->get_render());
	_node_path.set_scale(1.1);
	_node_path.set_pos(_app.get_ground().get_x()+20, _app.get_ground().get_y()+20, _app.get_ground().get_z());
	_node_path.set_hpr(180, 0, 0);
}