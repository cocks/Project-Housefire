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

void Gyzweed::update() {
	if(_walk_interval != NULL) {
		if(!_walk_interval->step_play()) {	// returns false when the interval is finished
			_walk_interval = NULL;
			_app.get_anim_controls().stop_all();
			_app.get_anim_controls().pose_all(4);
		}
	}
}

void Gyzweed::walk_to(LPoint3f pos) {
	// stop walking if gyz already is
	if (_walk_interval != NULL) {
		_walk_interval->set_end_pos(_app.get_gyzweed()->get_node_path().get_pos());
		_walk_interval->finish();
	}
	
	// make gyz look at where you clicked
	_app.get_gyzweed()->get_node_path().heads_up(pos);
	// 'ralph' model is front-facing so we have to turn him around.
	_app.get_gyzweed()->get_node_path().set_h(((int)_app.get_gyzweed()->get_node_path().get_h()+180)%360); // the %360 is to prevent overturning
	_app.get_anim_controls().loop_all(true);
	
	// do the walking
	int walkSpeed = 11;
	LVector3f walkVect = _app.get_gyzweed()->get_node_path().get_pos() - pos;
	float walkDistance = walkVect.length();
	float walkTime = walkDistance / walkSpeed;
	
	_walk_interval = new CLerpNodePathInterval("_walk_interval",
													walkTime, CLerpInterval::BT_no_blend, true, false, _app.get_gyzweed()->get_node_path(), NodePath());
	_walk_interval->set_end_pos(pos);
	_walk_interval->setup_play(0, 10, 1, false);
	_walk_interval->start();
}
