#pragma once
#include "windows.h"
#include "Config.h"
#include "Client.h"

class VolumeController
{
public:
	VolumeController(const Config &config, Client &client);
	~VolumeController();

	void set_active_device();

	int get_volume();
	void set_volume(int new_volume);

	Client get_client();

	void start();
	void print_keys();

	const int VOLUME_UP;
	const int VOLUME_DOWN;
private:
	
	int volume = 0;
	json::value active_device = json::value::Null;
	const Config m_config;
	Client m_client;
	

};

