
#include <stdio.h>
#include "WifiNative.h"
#include <unistd.h>

void Usage()
{
    printf("WifiTool <Option> [args ...] \n");
    printf("Options: \n");
    printf("\t-app\tRun an application\n");
    printf("\t-test\t\tRun a test\n");
    printf("\t-?\t\t\tFor Help\n");
}



int main(int argc, char* argv[])
{
    // if (argc < 2) {
    //     Usage();
    //     return 0;
    // }

    while (TRUE) {
        char cmd[255];
        scanf("%s", cmd);
        printf("cmd = %s\n", cmd);
        if (!strcmp("start", cmd)) {
            if (WifiNative::LoadDriver()) {
                printf("== WifiNative::LoadDriver succeeded! ==\n");
            }

            WifiNative::SetInterfaceDown();

            if (WifiNative::StartSupplicant()) {
                printf("== WifiNative::StartSupplicant succeeded! ==\n");
            }

            int connectTries = 0;
            while (true) {
                if (WifiNative::ConnectToSupplicant()) {
                    printf("== WifiNative::ConnectToSupplicant succeeded! connectTries = %d==\n", connectTries);
                    break;
                }
                if (connectTries++ < 5) {
                    usleep(1000000);
                }
                else {
                    printf("== WifiNative::ConnectToSupplicant failed! ==\n");
                    break;
                }
            }
        }
        else if (!strcmp("stop", cmd)) {
            if (WifiNative::StopSupplicant()) {
                printf("== WifiNative::StopSupplicant succeeded! ==\n");
            }

            WifiNative::SetInterfaceDown();
            
            if (WifiNative::UnloadDriver()) {
                printf("== WifiNative::UnloadDriver succeeded! ==\n");
            }
        }
        else if (!strcmp("scan", cmd)) {
            if (WifiNative::ScanCommand(TRUE)) {
                printf("== WifiNative::ScanCommand succeeded! ==\n");
            }
        }
        else if (!strcmp("scan_result", cmd)) {
            String str = WifiNative::ScanResultsCommand();
            printf("== WifiNative::ScanResultsCommand :%s! ==\n", (const char*)str);
        }
        else if (!strcmp("reconfigure", cmd)) {
            if (WifiNative::ReloadConfigCommand()) {
                printf("== WifiNative::ReloadConfigCommand succeeded! ==\n");
            }
        }
        else if (!strcmp("rundhcp", cmd)) {
            if (WifiNative::RunDhcp()) {
                printf("== WifiNative::RunDhcp succeeded! ==\n");
            }
        }
        else if (!strcmp("stopdhcp", cmd)) {
            if (WifiNative::StopDhcp()) {
                printf("== WifiNative::stopdhcp succeeded! ==\n");
            }
        }
        else if (!strcmp("exit", cmd)) {
            break;
        }
        else {
            Usage();
        }
    }

    return 0;


    // if (!Wifi?Native::ScanCommand(TRUE)) {
        // printf("== WifiNative::ScanCommand failed! ==\n");
    // }

    // String status = WifiNative::StatusCommand();
    // printf("== [main] status: %s ==\n", (const char*)status);

    // return 0;
}
