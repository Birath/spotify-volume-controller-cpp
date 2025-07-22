# Spotify Volume Controller

## Overview

Spotify Volume Controller lets you hook two keys (for example your volume controls) directly to Spotify using Spotify's REST API.

## Usage

To use Spotify Volume Controller you need to create an application using the [Spotify Developer Dashboard](https://developer.spotify.com/dashboard/applications).
After creating an application go into its settings and add a _Redirect URI_. `http://127.0.0.1:5000/callback` is default callback URI but you can use whatever you want as long as you set it in the config file and follow [Spotify's requirements](https://developer.spotify.com/documentation/web-api/concepts/redirect_uri). For example `localhost` is not allowed to be used.

Then copy the _client secret_ and the _client id_ values to the `config-example.json` file as strings. After that rename the file to `config.json` and start the program. Follow the instructions and if everything is correct it should be able to authorize it self.

## Changing volume keys

If the default values aren't working or you just want to use different keys just change the `volume_up` and `volume_down` values in the config file.

The values should be the decimal value of the virtual key codes for the keys you want to use. See [Microsoft's documentation](https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes) for a complete list of virtual key codes. You can also set `print_keys` to `true` to print the decimal values of the virtual key code for each key you press.

## 403 Error when quickly changing volume

This seems to be some kind of rate limiting from Spotify (though they have a separate error code for that, 429, so not sure why this one is `403`). If you are encountering it frequently, try to increase the volume increment using the `volume_increment` config option (default is 1).

## Building and installing

See the [BUILDING](BUILDING.md) document.
