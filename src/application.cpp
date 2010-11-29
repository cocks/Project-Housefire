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


#include "application.hpp"
#include "gameObject.hpp"
#include "gameObjectController.hpp"
#include "gameObjectManager.hpp"
#include <texturePool.h>
#include <directionalLight.h>
#include <cIntervalManager.h>
#include <auto_bind.h>
#include <animControlCollection.h>
#include <mouseWatcher.h>
#include <collisionRay.h>
#include <collisionTraverser.h>
#include <collisionHandlerQueue.h>
#include <collisionNode.h>
#include <audioManager.h>
#include <asyncTaskManager.h>
#include <clockObject.h>
#include <load_prc_file.h>

// TODO: get rid of these globals
PT(MouseWatcher) mouseWatcher;
PT(CollisionRay) pickerRay;
CollisionTraverser myTraverser = CollisionTraverser("ctraverser");
PT(CollisionHandlerQueue) myHandler = new CollisionHandlerQueue();
PT(CollisionNode) pickerNode;
NodePath pickerNP;
PT(DirectionalLight) d_light;

Application::Application()
: _config_page(0),
_framework(0),
_window(0),
_gyzweed(0) {
}

Application* Application::instance() {
	static Application _instance;
	return &_instance;
}

#ifdef HOUSEFIRE_PLATFORM_WINDOWS
bool Application::initiate(LPTSTR command_line) {
	// TODO: convert command_line to char string and tokenize the arguments before
	// passing it into the other overload of initiate
	return initiate(0, 0);
}
#endif

bool Application::initiate(int argc, char** argv) {
	if (!initiate_engine(argc, argv)) {
		return false;
	}
	
	if (!load_assets()) {
		return false;
	}
	
	return true;
}

void Application::terminate() {
	unload_assets();
	terminate_engine();
}

bool Application::initiate_engine(int argc, char** argv) {
	// TODO: should probably clone a default configuration file from the resource
	// directory, copy it to a local user directory (eg. "~/.housefire/housefire.prc")
	// and load the configuration from there.
	
	_config_page = load_prc_file("housefire.prc");
	if (!_config_page) {
		return false;
	}
	
	_framework = new PandaFramework;
	_framework->open_framework(argc, argv);
	_framework->set_window_title("Project Housefire");
	
	_window = _framework->open_window();
	if (!_window) {
		return false;
	}
	_window->enable_keyboard();
	
	_audio_manager = AudioManager::create_AudioManager();
	_audio_manager->set_volume(1.0f);
	
	_object_manager = new GameObjectManager;
	
	_update_task = new GenericAsyncTask("HousefireUpdateTask", &Application::update_task_callback, this);
	_framework->get_task_mgr().add(_update_task);
	
	_framework->define_key("mouse1", "click", Application::handle_mouse, this);
	
	// used for click-to-move
	pickerNode = new CollisionNode("mouseRay");
	pickerNP = _window->get_camera_group().attach_new_node (pickerNode);
	pickerNode->set_from_collide_mask(GeomNode::get_default_collide_mask());
	pickerRay = new CollisionRay();
	pickerNode->add_solid(pickerRay);
	myHandler = new CollisionHandlerQueue();
	myTraverser.add_collider(pickerNP, myHandler);
	myTraverser.show_collisions(_window->get_render());
	
	return true;
}

void Application::terminate_engine() {
	if (_update_task) {
		_framework->get_task_mgr().remove(_update_task);
		_update_task.clear();
	}
	
	_object_manager.clear();
	_audio_manager.clear();
	
	_framework->close_framework();
	
	delete _framework;
	_framework = 0;
	_window = 0;
	
	if (_config_page) {
		unload_prc_file(_config_page);
		_config_page = 0;
	}
}

bool Application::load_assets() {
	// TODO: This will load in the intro screen, menu, and other common assets.
	// Loading in modules/levels will need to be changed to be more data driven,
	// de-serializing the game objects and scene graph from disk. Much of this
	// code might be better placed and managed in separate classes for the
	// different stages of the game or screens in the menu, ie. MainMenu,
	// SinglePlayerMenu, GameWorld, etc.
	
	srand ( time(NULL) );
	if (rand() % 2 != 0) {
		_background_music = _audio_manager->get_sound("assets/music/ambience.ogg");
	} else {
		_background_music = _audio_manager->get_sound("assets/music/durr.ogg");
	}
	
	if (!_background_music) {
		return false;
	}
	
	_background_music->set_loop(true);
	_background_music->play();
	
	// make a flat textured poly for the ground
	_ground = _window->load_model(_framework->get_models(), "background");
	_ground.reparent_to(_window->get_render());
	PT(Texture) groundTex = TexturePool::load_texture("assets/textures/burnt_sand_light.png");
	_ground.set_texture(groundTex, 1);
	_ground.flatten_light();        // so the texture attribute doesn't get inherited by _gyzweed
	_ground.set_pos(0,0,0);
	_ground.set_hpr(0,0,0);
	
	_gyzweed = new Gyzweed(*this);
	
	auto_bind(_gyzweed->get_node_path().node(), _anim_controls,
			  PartGroup::HMF_ok_part_extra | PartGroup::HMF_ok_anim_extra | PartGroup::HMF_ok_wrong_root_name);
	
	_window->get_camera_group().reparent_to(_gyzweed->get_node_path());
	_window->get_camera_group().set_pos(0, 100, 80);
	_window->get_camera_group().look_at(_gyzweed->get_node_path());
	
	d_light = new DirectionalLight("my d_light");
	NodePath dlnp = _window->get_render().attach_new_node(d_light);
	d_light->set_direction(LVector3f(-5, -5, -5));
	_window->get_render().set_light(dlnp);
	
	_window->set_lighting(1);
	
	// for collisions
	_ground.set_tag("groundTag", "1");
	
	mouseWatcher = DCAST (MouseWatcher, _window->get_mouse().node());
	
	return true;
}

void Application::handle_mouse(const Event* e, void* data) {
	//TODO: If you click on Gyzweed he will move to the point on his model where the click hit.
	
	Application* app = static_cast< Application* >(data);
	
	if (!mouseWatcher->has_mouse()){
		// The mouse is probably outside the screen.
		return;
	}
	// This gives up the screen coordinates of the mouse.
	LPoint2f mpos = mouseWatcher->get_mouse();
	
	// This makes the ray's origin the camera and makes the ray point 
	// to the screen coordinates of the mouse.
	pickerRay->set_from_lens(app->get_window()->get_camera(0), mpos.get_x(), mpos.get_y());
	
	myTraverser.traverse(app->get_window()->get_render());
	
	LPoint3f newPos;
	if (myHandler->get_num_entries() > 0) {
		newPos = myHandler->get_entry(0)->get_surface_point(app->get_window()->get_render());
	} else {
		return;
	}
	
	app->get_gyzweed()->walk_to(newPos);
}

void Application::unload_assets() {
	_background_music.clear();
}

void Application::run() {
	ClockObject* clock = ClockObject::get_global_clock();
	clock->reset();
	_framework->main_loop();
}

void Application::update() {
	ClockObject* clock = ClockObject::get_global_clock();
	double elapsed = clock->get_dt();
	_object_manager->update(elapsed);
	_audio_manager->update();
	
	// TODO: Update every GameObject
	_gyzweed->update();
}

AsyncTask::DoneStatus Application::update_task_callback(GenericAsyncTask* task, void* data) {
	Application* app = static_cast< Application* >(data);
	app->update();
	return AsyncTask::DS_cont;
}
