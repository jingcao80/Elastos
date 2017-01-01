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

#include "org/alljoyn/bus/Observer.h"
#include "org/alljoyn/bus/NativeObserver.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include <elastos/utility/logging/Logger.h>

#include <alljoyn/CoreObserver.h>
#include <alljoyn/BusInternal.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeObserver");

NativeObserver::NativeObserver(
        /* [in] */ IBusAttachment* inbus,
    /* [in] */ IObserver* listener,
    /* [in] */ const InterfaceSet& mandatory)
    : ajn::CoreObserver(mandatory)
{
    mBus = inbus;
    CBusAttachment* busObj = (CBusAttachment*)inbus;
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "Observer_create(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().RegisterObserver(this);
    IWeakReferenceSource::Probe(listener)->GetWeakReference((IWeakReference**)&mObserver);
}

NativeObserver::~NativeObserver()
{}

void NativeObserver::ObjectDiscovered(
    /* [in] */ const ajn::ObjectId& oid,
    /* [in] */ const InterfaceSet& interfaces,
    /* [in] */ ajn::SessionId sessionid)
{
    AutoPtr<IObserver> jo;
    mObserver->Resolve(EIID_IObserver, (IInterface**)&jo);
    if (jo == NULL) {
        return;
    }

    String busname(oid.uniqueBusName.c_str());
    String path(oid.objectPath.c_str());
    Int32 jsessionid = sessionid;
    AutoPtr<ArrayOf<String> > jinterfaces = ArrayOf<String>::Alloc(interfaces.size());

    Int32 i = 0;
    for (InterfaceSet::iterator it = interfaces.begin(); it != interfaces.end(); ++it) {
        String intfname(it->c_str());
        jinterfaces->Set(i++, intfname);
    }


    Observer* observer = (Observer*)jo.Get();
    observer->ObjectDiscovered(busname, path, jinterfaces, jsessionid);
}

void NativeObserver::ObjectLost(
    /* [in] */ const ajn::ObjectId& oid)
{
    AutoPtr<IObserver> o;
    mObserver->Resolve(EIID_IObserver, (IInterface**)&o);
    if (o == NULL) {
        return;
    }

    String busname(oid.uniqueBusName.c_str());
    String path(oid.objectPath.c_str());

    Observer* observer = (Observer*)o.Get();
    observer->ObjectLost(busname, path);
}

void NativeObserver::EnablePendingListeners()
{
    AutoPtr<IObserver> o;
    mObserver->Resolve(EIID_IObserver, (IInterface**)&o);
    if (o == NULL) {
        return;
    }

    Observer* observer = (Observer*)o.Get();
    observer->EnablePendingListeners();
}

void NativeObserver::TriggerEnablePendingListeners()
{
    CBusAttachment* busObj = (CBusAttachment*)mBus.Get();
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "TriggerEnablePendingListeners(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().EnablePendingListeners(this);
}

void NativeObserver::Detach()
{
    CBusAttachment* busObj = (CBusAttachment*)mBus.Get();
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "Detach(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().UnregisterObserver(this);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org