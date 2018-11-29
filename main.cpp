#include <iostream>
// Must be here due to collision with boost
#include "oauth.h"
#include "windows.h"
#include <cpprest/base_uri.h>
#include "Client.h"
#include "VolumeController.h"

Client *client;

int main() {
	std::wcout << "Starting..." << '\n';
	Config config;
	if (!config.is_valid()) {
		std::wcout << "Failed to read config file." << std::endl;
		std::cin.get();
		return 1;
	}
	web::json::value token = get_token(config);
	if (token.is_null()) {
		std::wcout << "Failed to connect to spotify, exiting..." << std::endl;
		std::cin.get();
		return 1;
	}
	Client temp(token, config);
	client = &temp;
	
	std::wcout << "Connected to spotify successfully!" << std::endl;
	VolumeController controller(config, *client);
	if (config.should_print_keys()) {
		controller.print_keys();
	}
	else {
		controller.start();
	}
    return 0;
}