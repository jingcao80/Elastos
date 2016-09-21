#ifndef NODE_CARSHAREDCLIENT_H
#define NODE_CARSHAREDCLIENT_H

#include "elastos_root.h"

#include <utils/Log.h>

EXTERN NodeBridge gNodeBridge;

namespace Elastos {

    class CarSharedClient
    {
    public:
        // can be called from any thread, to be executed in webkit thread
        static void EnqueueFunctionPtr(void (*proc)(void*), void* payload) {
            ALOGD("CarSharedClient::EnqueueFunctionPtr======begin======bridge tag: %d", gNodeBridge.mTag);

            //JSC::Bindings::TestCallbackBuf::EnqueueFunctionPtr(proc, payload);

            //gNodeBridge.vt->Enqueue(&gNodeBridge, proc, payload);
            gNodeBridge.vt->Enqueue(&gNodeBridge, NULL, NULL, proc, payload);

            ALOGD("CarSharedClient::EnqueueFunctionPtr======end======");
        };

    };
}

#endif
