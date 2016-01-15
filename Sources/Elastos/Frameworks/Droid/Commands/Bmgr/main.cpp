
#include "stdio.h"
#include "BackupManagerActor.h"
#include <elastos/System.h>
#include <elastos/utility/logging/Logger.h>
#include <unistd.h>

using Elastos::Core::System;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::CRandomAccessFile;
using Elastos::Droid::Commands::Bmgr::BackupManagerActor;

void Usage()
{
    printf("Bmgr <Option> [args ...]\n");
    printf("Options: \n");
    printf("\t-?\t\t\tFor Help\n");
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        Usage();
        return 0;
    }
    AutoPtr<BackupManagerActor> actor = new BackupManagerActor();
    actor->Init();
    char* cmd;
    cmd = argv[1];
    if(!strcmp("enabled", cmd)) {
        actor->DoEnabled();
    }
    else if (!strcmp("enable", cmd)) {
        int isEnable = atoi(argv[2]);
        actor->DoEnable(isEnable);
        printf("doenable\n");
    }
    else if (!strcmp("backup", cmd)) {
        char* pkg = argv[2];
        String pkgName(pkg);
        actor->DoBackup(pkgName);
    }
    else if (!strcmp("transport", cmd)) {
        char* transport = argv[2];
        String transportName(transport);
        actor->DoTransport(transportName);
    }
    else if (!strcmp("listTransport", cmd)) {
        actor->DoListTransport();
    }
    else if (!strcmp("run", cmd)) {
        actor->DoRun();
    }
    else if (!strcmp("restore", cmd)) {
        char* restore = argv[2];
        String restoreName(restore);
        actor->DoRestorePackage(restoreName);
    }
    else if (!strcmp("?", cmd)) {
        Usage();
    }
    else {
        printf("Unknown command\n");
        Usage();
    }
    return 0;
}

