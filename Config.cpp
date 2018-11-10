#include "Config.h"

Config::Config() {
}


Config::~Config() {
}

std::wstring Config::CLIENT_ID = L"de2d94bddd8d490fbc9dc9ac23c80d00";
std::wstring Config::CLIENT_SECRET = L"";
std::wstring Config::SCOPES = L"user-read-playback-state user-modify-playback-state";

std::wstring Config::REDIRECT_URI = L"http://localhost:5000/callback";
