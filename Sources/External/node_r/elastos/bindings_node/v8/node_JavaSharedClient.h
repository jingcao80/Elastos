#ifndef NODE_JAVASHAREDCLIENT_H
#define NODE_JAVASHAREDCLIENT_H

#include "elastos_root.h"

#include <utils/Log.h>

EXTERN NodeBridge gNodeBridge;

namespace Elastos {

    class JavaSharedClient
    {
    public:
        // can be called from any thread, to be executed in webkit thread
        static void EnqueueFunctionPtr(void (*proc)(void*), void* payload) {
            ALOGD("JavaSharedClient::EnqueueFunctionPtr======begin======bridge tag: %d", gNodeBridge.mTag);

            //JSC::Bindings::TestCallbackBuf::EnqueueFunctionPtr(proc, payload);

            //gNodeBridge.vt->Enqueue(&gNodeBridge, proc, payload);
            gNodeBridge.vt->Enqueue(&gNodeBridge, NULL, NULL, proc, payload);

            ALOGD("JavaSharedClient::EnqueueFunctionPtr======end======");
        };

    };
}

#endif
