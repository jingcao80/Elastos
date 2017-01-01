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

#ifndef __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusListener.h>
#include <alljoyn/TransportMask.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusListener : public ajn::BusListener
{
public:
    NativeBusListener(
        /* [in] */ IBusListener* listener);

    ~NativeBusListener();

    void Setup(
        /* [in] */ IBusAttachment* busAttachment);

    void ListenerRegistered(
        /* [in] */ ajn::BusAttachment* bus);

    void ListenerUnregistered();

    void FoundAdvertisedName(
        /* [in] */ const char* name,
        /* [in] */ ajn::TransportMask transport,
        /* [in] */ const char* namePrefix);

    void LostAdvertisedName(
        /* [in] */ const char* name,
        /* [in] */ ajn::TransportMask transport,
        /* [in] */ const char* namePrefix);

    void NameOwnerChanged(
        /* [in] */ const char* busName,
        /* [in] */ const char* previousOwner,
        /* [in] */ const char* newOwner);

    void BusStopping();

    void BusDisconnected();

private:
    NativeBusListener(const NativeBusListener& other);
    NativeBusListener& operator =(const NativeBusListener& other);

    AutoPtr<IWeakReference> mBusListener;
    AutoPtr<IWeakReference> mBusAttachment;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__
