#include "ServiceManager.h"
#include <sys/types.h>
#include <unistd.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <stdio.h>

int main(int argv, char**argc)
{
    android::sp<android::ProcessState> proc(android::ProcessState::self());
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(ELASTOS_SERVICEMGR_NAME), new ServiceManager());
    if (res != 0) {
        ALOGE("<%s, %d> add service 'media.servicemanager' failed, res=%d\n",
            __FILE__, __LINE__, res);
        return -1;
    }

    android::IPCThreadState::self()->joinThreadPool();
    return 0;
}
