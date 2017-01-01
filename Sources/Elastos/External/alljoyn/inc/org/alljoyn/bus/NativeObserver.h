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

#ifndef __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__
#define __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusListener.h>
#include <alljoyn/TransportMask.h>
#include <alljoyn/CoreObserver.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeObserver : public ajn::CoreObserver
{
public:
    NativeObserver(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ IObserver* listener,
        /* [in] */ const InterfaceSet& mandatory);

    ~NativeObserver();

    /* interface towards ObserverManager */
    virtual void ObjectDiscovered(
        /* [in] */ const ajn::ObjectId& oid,
        /* [in] */ const InterfaceSet& interfaces,
        /* [in] */ ajn::SessionId sessionid);

    virtual void ObjectLost(
        /* [in] */ const ajn::ObjectId& oid);

    /**
     * Enable all disabled listeners for this observer.
     *
     * Called from the ObserverManager work queue to make sure the
     * initial callbacks of triggerOnExisting listeners are called
     * from the local endpoint dispatcher threads.
     */
    virtual void EnablePendingListeners();

    void TriggerEnablePendingListeners();

    void Detach();

private:
    NativeObserver(const NativeObserver& other);
    NativeObserver& operator =(const NativeObserver& other);

    AutoPtr<IWeakReference> mObserver;
    AutoPtr<IBusAttachment> mBus;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__
