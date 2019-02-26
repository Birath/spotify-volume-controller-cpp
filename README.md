# Spotify Volume Controller

## Overview
Spotify Volume Controller lets you hook two keys (for example your volume controls) directly to Spotify using the Spotify REST API. 

## Usage
To use Spotify Volume Controller you need to create an application using the [Spotify Developer Dashboard](https://developer.spotify.com/dashboard/applications). 
After creating an application, go into its settings and add a *Redirect URI*. ```http://localhost:5000/callback``` is default callback URI, but you can use whatever you want as long as you set it in the config file.

Then copy the *client secret* and the *client id* values to the ```config-example.json``` file as strings. After that rename the file to ```config.json``` and start the program. Follow the instructions and if everything is correct it should be able to authorize it self.

## Changing volume keys
If the default values aren't working or you just want to use different keys just change the ```volume_up``` and ```volume_down``` values in the config file.

The values should be the decimal value of the virutal key codes for the keys you want to use. See [here](https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes) for a complete list of virtual key codes. You can also set ```print_keys``` to ```true``` to print the decimal values of the virtual key code for each key you press.

## Building a standalone exe using VS/MSBuild.

* Download and install [vcpkg](https://github.com/Microsoft/vcpkg).
* To make all packages installed with vcpkg availiable user wide for VS/MSBuild run following command after installation:
```
vcpkg integrate install
```
* Install the [cpprestsdk](https://github.com/Microsoft/cpprestsdk)  static triplet (see [here](https://github.com/Microsoft/vcpkg/blob/master/docs/users/triplets.md) more info about triplets)
``` 
vcpkg install cpprestsdk:x86-windows-static 
vcpkg install cpprestsdk:x64-windows-static
```
  * After the installation, open the project's ```.vcxproj``` file and add the following lines to the ```Globals``` PropertyGroup
    * If you only want to build a static version for certain build configurations add ```And '$(Configuration)' == 'DesiredConfiguration'``` to the ```Condition``` property. 
  ```xml
  <PropertyGroup Label="Globals">
    <!-- .... -->
    <VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
    <VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
</PropertyGroup>
```
  * Then open the project in Visual Studio and open the project properties and set the configuration and platform what you want to build against. 
  * Go to *C/C++ -> Preprocessor* and add **_NO_ASYNCRTIMP=1** to *Preprocessor Definitions*.
  * Then go to *Linker -> Input* and add **cpprest_2_10.lib;bcrypt.lib;crypt32.lib;winhttp.lib;httpapi.lib;zlib.lib;** to *Additional Dependencies*.
 * Lastly go to *C/C++ -> Code Generation* and set *Runtime Library* to **Multi-threaded (/MT)**.
 * Now just build the project as usual and the ```.exe``` that is generated should include all needed DLL's.
