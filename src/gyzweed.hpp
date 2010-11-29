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


#ifndef HOUSEFIRE_GYZWEED_HPP
#define HOUSEFIRE_GYZWEED_HPP

#include "gameObject.hpp"
#include "gameObjectManager.hpp"
#include "gameObjectController.hpp"
#include "application.hpp"
#include <cLerpNodePathInterval.h>

class Gyzweed : public GameObject
{	
public:
	Gyzweed(Application const&);
	
	void walk_to(LPoint3f pos);
	
	void update();
	
	NodePath get_node_path() const;
	
private:
	PT(CLerpNodePathInterval) _walk_interval;

	NodePath _node_path;
	
	Application const& _app;
};

inline NodePath Gyzweed::get_node_path() const {
	return _node_path;
}

#endif // HOUSEFIRE_GYZWEED_HPP
