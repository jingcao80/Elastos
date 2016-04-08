
#include "WifiFun.h"
#include <cutils/properties.h>


static Object gLock;


void* ElWifi_Start_Internal(void* arg)
{
    AutoLock lock(&gLock);

    if(!WifiNative::LoadDriver())
        return NULL;
    // LOGD("ElWifi:== LoadDriver succeeded ==\n");

    WifiNative::SetInterfaceDown();

    if(!WifiNative::StartSupplicant()) {
        WifiNative::UnloadDriver();
        return NULL;
    }
    // LOGD("ElWifi:== StartSupplicant succeeded==\n");

    int connectTries = 0;
    while (true) {
        if (WifiNative::ConnectToSupplicant()) {
            // LOGD("ElWifi:== ConnectToSupplicant succeeded!==\n");
            break;
        }
        if (connectTries++ < 5) {
            usleep(1000000);
        }
        else {
            // LOGD("ElWifi:== ConnectToSupplicant failed! ==\n");
            WifiNative::StopSupplicant();
            WifiNative::UnloadDriver();
            return NULL;
        }
    }

    if(!WifiNative::ReloadConfigCommand())
        return NULL;
    // LOGD("ElWifi:== ReloadConfigCommand succeeded==\n");

printf("==== File: %s, Line: %d ====, FUNC : %s.\n", __FILE__, __LINE__, __FUNCTION__);
    if (WifiNative::RunDhcp())
        property_set("net.state", "1");

    return NULL;
}

bool ElWifi_Start()
{
    // //TODO Asynchronize
    // pthread_t thread;
    // return pthread_create(&thread, NULL, ElWifi_Start_Internal, NULL);

    // Synchronize
    ElWifi_Start_Internal(NULL);
    return true;
}

void* ElWifi_Stop_Internal(void*)
{
    AutoLock lock(&gLock);

    WifiNative::StopDhcp();

    if(!WifiNative::StopSupplicant())
        return NULL;
    // LOGD("ElWifi:== StopSupplicant succeeded==\n");

    WifiNative::SetInterfaceDown();

    if(!WifiNative::UnloadDriver())
        return NULL;
    // LOGD("ElWifi:== UnloadDriver succeeded==\n");

    property_set("net.state", "0");

    return NULL;
}

bool ElWifi_Stop()
{
    // //TODO Asynchronize
    // pthread_t thread;
    // return pthread_create(&thread, NULL, ElWifi_Stop_Internal, NULL);

    // Synchronize
    ElWifi_Stop_Internal(NULL);
    return true;
}

ScanResult* ParseScanResult(String line)
{
    if (line.IsNullOrEmpty()) {
        return NULL;
    }

    ScanResult* scanResult = NULL;

    /*
     * Cache implementation (LinkedHashMap) is not synchronized, thus,
     * must synchronized here!
     */
    String result[5];
    int resultCount = 0;

    int lineLen = line.GetLength();
    for (int tabBeg = 0, tabEnd = 0; tabEnd <= lineLen; ++tabEnd) {
        if (tabEnd == lineLen || line.GetChar(tabEnd) == '\t') {
            if (tabEnd > tabBeg) {
                result[resultCount] = line.Substring(tabBeg, tabEnd);
                ++resultCount;
            }
            tabBeg = tabEnd + 1;
        }
    }

    if (3 <= resultCount && resultCount <= 5) {
        //String bssid = result[0];
        // bssid | frequency | level | flags | ssid
        // int frequency;
        // int level;
        // try {
        //     frequency = Integer.parseInt(result[1]);
        //     level = Integer.parseInt(result[2]);
            /* some implementations avoid negative values by adding 256
             * so we need to adjust for that here.
             */
        //     if (level > 0) level -= 256;
        // } catch (NumberFormatException e) {
        //     frequency = 0;
        //     level = 0;
        // }

        /*
         * The formatting of the results returned by
         * wpa_supplicant is intended to make the fields
         * line up nicely when printed,
         * not to make them easy to parse. So we have to
         * apply some heuristics to figure out which field
         * is the SSID and which field is the flags.
         */
        String ssid;
        String flags;
        if (resultCount == 4) {
            if (result[3].GetChar(0) == '[') {
                flags = result[3];
                ssid = "";
            }
            else {
                flags = "";
                ssid = result[3];
            }
        }
        else if (resultCount == 5) {
            flags = result[3];
            ssid = result[4];
        }
        else {
            // Here, we must have 3 fields: no flags and ssid
            // set
            flags = "";
            ssid = "";
        }

        // Do not add scan results that have no SSID set
        if (0 < ssid.Trim().GetLength()) {
            int security = -1;
            if (flags.Contains("WPA-PSK") || flags.Contains("WPA2-PSK"))
                security = 0;
            else if (flags.Contains("WEP"))
                security = 1;
            else if (flags.Contains("WPA2-EAP"))
                return NULL;//TODO:
            scanResult = new ScanResult();
            strcpy(scanResult->ssid, ssid);
            scanResult->security = security;
        }
    }

    return scanResult;
}

std::vector<ScanResult*>* ParseScanResults(String scanResults)
{
    if (scanResults.IsNullOrEmpty()) {
        return NULL;
    }

    std::vector<ScanResult*>* scanList = new std::vector<ScanResult*>;
    int lineCount = 0;

    int scanResultsLen = scanResults.GetLength();
    // Parse the result string, keeping in mind that the last line does
    // not end with a newline.
    for (int lineBeg = 0, lineEnd = 0; lineEnd <= scanResultsLen; ++lineEnd) {
        if (lineEnd == scanResultsLen || scanResults.GetChar(lineEnd) == '\n') {
            ++lineCount;

            if (lineCount == 1) {
                lineBeg = lineEnd + 1;
                continue;
            }
            if (lineEnd > lineBeg) {
                String line = scanResults.Substring(lineBeg, lineEnd - lineBeg);
                // LOGD("ElWifi: ParseScanResult %d:%s", lineCount, (const char*)line);
                ScanResult* scanResult = ParseScanResult(line);
                // LOGD("ElWifi: ParseScanResult end");
                if (scanResult != NULL) {
                    scanList->push_back(scanResult);
                } else {
                    //TODO: hidden network handling
                }
            }
            lineBeg = lineEnd + 1;
        }
    }

    if (lineCount == 1) {
        delete scanList;
        scanList = NULL;
    }

    return scanList;
}

std::vector<ScanResult*>* ElWifi_ScanResults()
{
    if (WifiNative::ScanCommand(true)) {
        String str = WifiNative::ScanResultsCommand();
        // LOGD("ElWifi:scanResult:\n%s\n", (const char*)str);
        return ParseScanResults(str);
    }

    return NULL;
}

int ElWifi_AddNetwork(const char* ssid, int security, const char* password)
{
    int netId;
    netId = WifiNative::AddNetworkCommand();
    if (-1 == netId)
        return -1;
    // LOGD("ElWifi:netId = %d\n", netId);

    String strSsid("\"");
    strSsid.Append(ssid);
    strSsid.Append("\"");
    if (!WifiNative::SetNetworkVariableCommand(netId, "ssid", strSsid)) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

    String strPassword("\"");
    if (password != NULL) {
        strPassword.Append(password);
        strPassword.Append("\"");
    }
    if (0 == security) {
        if (!WifiNative::SetNetworkVariableCommand(netId, "psk", strPassword)) {
            WifiNative::RemoveNetworkCommand(netId);
            return -1;
        }
    }
    else {
        if (!WifiNative::SetNetworkVariableCommand(netId, "key_mgmt", "NONE")) {
            WifiNative::RemoveNetworkCommand(netId);
            return -1;
        }

        if (1 == security) {
            if (!WifiNative::SetNetworkVariableCommand(netId, "wep_key0", strPassword)) {
                WifiNative::RemoveNetworkCommand(netId);
                return -1;
            }

            if (!WifiNative::SetNetworkVariableCommand(netId, "wep_tx_keyidx", 0)) {
                WifiNative::RemoveNetworkCommand(netId);
                return -1;
            }
        }
    }

    if (!WifiNative::EnableNetworkCommand(netId, true)) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

printf("==== File: %s, Line: %d ====, FUNC : %s.\n", __FILE__, __LINE__, __FUNCTION__);
    if (!WifiNative::RunDhcp()) {
        WifiNative::RemoveNetworkCommand(netId);
        return -1;
    }

    property_set("net.state", "1");

    // LOGD("ElWifi:netId = %d, Enable!\n", netId);

    WifiNative::SaveConfigCommand();


    return netId;
}

bool ElWifi_SaveConfig()
{
    return WifiNative::SaveConfigCommand();
}

bool ElWifi_ReloadConfig()
{
    return WifiNative::ReloadConfigCommand();
}

