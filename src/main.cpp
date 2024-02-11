#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
// Must be here due to collision with boost
#include <windows.h>
#include <cpprest/base_uri.h>

#include "oauth.h"
#include "Client.h"
#include "VolumeController.h"
#include "updater.h"

#ifndef VERSION
#define VERSION "debug"
#endif

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("spotify_volume_controller", VERSION);
	program.add_argument("-c", "--config").help("Config file to use. If omitted file in working directory is used");
	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	std::wcout << "Starting..." << '\n';
	
	Config config = program.is_used("--config") ? Config(program.get("--config")) : Config();
	if (!config.is_valid()) {
		std::cerr << "Failed to read config file." << std::endl;
		std::cin.get();
		return 1;
	}
	web::json::value token = get_token(config);
	if (token.is_null()) {
		std::wcout << "Failed to connect to spotify, exiting..." << std::endl;
		std::cin.get();
		return 1;
	}
	Client client(token, config);

	std::wcout << "Connected to spotify successfully!" << std::endl;
	if (config.hide_window()) {
		FreeConsole();
	}
	VolumeController controller(config, client);
	if (config.should_print_keys()) {
		controller.print_keys();
	}
	else {
		controller.start();
	}
	return 0;
}