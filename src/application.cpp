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
#include <cardMaker.h>
#include <texturePool.h>
#include <audioManager.h>
#include <asyncTaskManager.h>
#include <clockObject.h>
#include <load_prc_file.h>


Application::Application()
: _config_page(0),
_framework(0),
_window(0) {
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
	_window->setup_trackball();

	_audio_manager = AudioManager::create_AudioManager();
	_audio_manager->set_volume(1.0f);

	_object_manager = new GameObjectManager;

	_update_task = new GenericAsyncTask("HousefireUpdateTask", &Application::update_task_callback, this);
	_framework->get_task_mgr().add(_update_task);

	return true;
}

void Application::terminate_engine() {
	if (_update_task) {
		_framework->get_task_mgr().remove(_update_task);
		_update_task.clear();
	}

	_object_manager.clear();
	_audio_manager.clear();

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
	
	_background_music = _audio_manager->get_sound("assets/music/ambience.ogg");
	if (!_background_music) {
		return false;
	}

	_background_music->set_loop(true);
	_background_music->play();
	
	CardMaker cardmaker("cardmaker");
	PT(PandaNode) groundNode = cardmaker.generate();
	NodePath ground(groundNode);
	ground.reparent_to(_window->get_render());
	PT(Texture) groundTex = TexturePool::load_texture("assets/textures/stone.jpg");
	ground.set_texture(groundTex, 1);
	ground.flatten_light();	// so the texture attribute doesn't get inherited by _qyzweed
	ground.set_pos(-10, -5, -10);
	ground.set_hpr(0, -70, 0);
	ground.set_scale(20, 20, 20);
	
	_gyzweed = _window->load_model(_framework->get_models(), "assets/models/ralph");
	_gyzweed.reparent_to(ground);
	_gyzweed.set_scale(0.05, 0.05, 0.05);
	_gyzweed.set_pos(0.5, 0.02, 0.5);
	_gyzweed.set_hpr(180, -90, 0);

	return true;
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
}

AsyncTask::DoneStatus Application::update_task_callback(GenericAsyncTask* task, void* data) {
	Application* app = static_cast< Application* >(data);
	app->update();
	return AsyncTask::DS_cont;
}
