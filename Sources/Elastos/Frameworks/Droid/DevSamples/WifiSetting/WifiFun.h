
#ifndef __WIFIFUN_H__
#define __WIFIFUN_H__

#include "WifiNative.h"
#include <utils/Log.h>
#include <vector>

class ScanResult
{
public:
    char ssid[255];
    int security; //-1:NULL 0:psk 1:web
};

void* ElWifi_Start_Internal(void* arg);

bool ElWifi_Start();

void* ElWifi_Stop_Internal(void*);

bool ElWifi_Stop();

ScanResult* ParseScanResult(String line);

std::vector<ScanResult*>* ParseScanResults(String scanResults);

std::vector<ScanResult*>* ElWifi_ScanResults();

int ElWifi_AddNetwork(const char* ssid, int security, const char* password);

bool ElWifi_SaveConfig();

bool ElWifi_ReloadConfig();

#endif  //__WIFIFUN_H__
