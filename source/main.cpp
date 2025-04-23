#include <exception>
#include <iostream>
#include <optional>

#include <argparse/argparse.hpp>
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#  include <wincon.h>
#  include <windows.h>
#endif
#include "Client.h"
#include "Config.h"
#include "VolumeController.h"
#include "data_types.h"
#include "oauth.h"

#ifndef VERSION
#  define VERSION "debug"
#endif

int main(int argc, char* argv[])
{
  argparse::ArgumentParser program("spotify_volume_controller", VERSION);
  program.add_argument("-c", "--config").help("Config file to use. If omitted file in working directory is used");
  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << '\n';
    std::cerr << program;
    return 1;
  }

  std::cout << "Starting..." << '\n';

  spotify_volume_controller::Config const config = program.is_used("--config")
      ? spotify_volume_controller::Config(program.get("--config"))
      : spotify_volume_controller::Config();
  if (!config.is_valid()) {
    std::cerr << "Failed to read config file." << '\n';
    std::cin.get();
    return 1;
  }

  if (config.should_print_keys()) {
    spotify_volume_controller::VolumeController::print_keys();
    return 0;
  }

  std::optional<spotify_volume_controller::token_t> token = spotify_volume_controller::oauth::get_token(config);
  if (!token.has_value()) {
    std::cout << "Failed to connect to spotify, exiting..." << '\n';
    std::cin.get();
    return 1;
  }
  spotify_volume_controller::Client client(token.value(), config);

  std::cout << "Connected to spotify successfully!" << '\n';
  if (config.hide_window()) {
#ifdef _WIN32
    FreeConsole();
#endif
  }
  spotify_volume_controller::VolumeController controller(config, client);
  controller.start();
  return 0;
}