//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

