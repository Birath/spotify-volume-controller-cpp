#include "VolumeController.h"
#include "key_hooks.h"
#include "string"

VolumeController::VolumeController(const Config &config, Client &client) : 
	m_config(config),
	m_client(client),
	VOLUME_UP(config.is_default_up() ? VK_VOLUME_UP: config.get_volume_up()),
	VOLUME_DOWN(config.is_default_down() ? VK_VOLUME_DOWN: config.get_volume_down())
{
	m_volume = client.get_current_playing_volume();
}

VolumeController::~VolumeController() {}

void VolumeController::set_active_device() {
	json::value desktop = m_client.get_desktop_player();
	if (desktop == json::value::Null) return;
	m_volume = desktop[L"volume_percent"].as_integer();
	active_device = desktop;
}

int VolumeController::get_volume() const {
	return m_volume;
}

void VolumeController::decrease_volume()
{
	set_volume(m_volume - m_config.volume_increment());
}

void VolumeController::increase_volume()
{
	set_volume(m_volume + m_config.volume_increment());
}

Client VolumeController::get_client() {
	return m_client;
}

void VolumeController::set_volume(int new_volume) {
	if (m_volume == -1) {
		set_active_device();
		if (active_device != json::value::Null) {
			new_volume = active_device[L"volume_percent"].as_integer();
		}
	}
	if (0 < new_volume && new_volume < 100) {
		bool success = m_client.set_volume(new_volume);
		if (!success && active_device != json::value::Null) {
			m_client.set_device_volume(new_volume, active_device[L"id"].as_string());
		}
		if (success) {
			m_volume = new_volume;
		}
	}
}

void VolumeController::start() {
	start_volume_hook(this);
}

void VolumeController::print_keys() {
	start_print_vkey();
}
