#include "VolumeController.h"
#include "key_hooks.h"
VolumeController::VolumeController(const Config &config, Client &client) : 
	m_config(config),
	m_client(client),
	// TODO Change to not using errors to set defualt keys??? Why did i ever do this?
	VOLUME_UP(config.get_volume_up() != -1 ? config.get_volume_up() : VK_VOLUME_UP),
	VOLUME_DOWN(config.get_volume_down() != -1 ? config.get_volume_down() : VK_VOLUME_DOWN)
{
	volume = client.get_current_playing_volume();
}

VolumeController::~VolumeController() {}

void VolumeController::set_active_device() {
	json::value desktop = m_client.get_desktop_player();
	if (desktop == json::value::Null) return;
	volume = desktop[L"volume_percent"].as_integer();
	active_device = desktop;
}

int VolumeController::get_volume() {
	return volume;
}

Client VolumeController::get_client() {
	return m_client;
}

void VolumeController::set_volume(int new_volume) {
	if (volume == -1) {
		set_active_device();
		if (active_device != json::value::Null) {
			new_volume = active_device[L"volume_percent"].as_integer();
		}
	}
	if (0 < new_volume && new_volume < 100) {
		if (!m_client.set_volume(new_volume) && active_device != json::value::Null) {
			m_client.set_device_volume(new_volume, active_device[L"id"].as_string());
		}
		volume = new_volume;
	}
}

void VolumeController::start() {
	start_volume_hook(this);
}

void VolumeController::print_keys() {
	start_print_vkey();
}
