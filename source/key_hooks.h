#pragma once

#include "VolumeController.h"
namespace spotify_volume_controller::key_hooks
{

void start_volume_hook(std::unique_ptr<VolumeController> controller);

void start_print_vkey();

}  // namespace spotify_volume_controller::key_hooks