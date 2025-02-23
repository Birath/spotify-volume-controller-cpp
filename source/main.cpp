#include <filesystem>
#include <iostream>

#include <argparse/argparse.hpp>
#include <fmt/core.h>

#include "Client.h"
#include "VolumeController.h"
#include "oauth.h"
#include "windows_helpers.h"

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
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  std::cout << "Starting..." << '\n';

  spotify_volume_controller::Config config = program.is_used("--config")
      ? spotify_volume_controller::Config(program.get("--config"))
      : spotify_volume_controller::Config();
  if (!config.is_valid()) {
    std::cerr << "Failed to read config file." << std::endl;
    std::cin.get();
    return 1;
  }
  std::optional<spotify_volume_controller::token_t> token = spotify_volume_controller::oauth::get_token(config);
  if (!token.has_value()) {
    std::cout << "Failed to connect to spotify, exiting..." << std::endl;
    std::cin.get();
    return 1;
  }
  spotify_volume_controller::Client client(token.value(), config);

  std::cout << "Connected to spotify successfully!" << std::endl;
  if (config.hide_window()) {
    FreeConsole();
  }
  spotify_volume_controller::VolumeController controller(config, client);
  if (config.should_print_keys()) {
    controller.print_keys();
  } else {
    controller.start();
  }
  return 0;
}