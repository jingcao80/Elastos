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

#include "org/alljoyn/bus/NativeBusListener.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeBusListener::NativeBusListener(
    /* [in] */ IBusListener* listener)
{
    /*
     * Be careful when using a weak global reference.  They can only be
     * passed to NewLocalRef, NewGlobalRef and DeleteWeakGlobalRef.
     */
    IWeakReferenceSource::Probe(listener)->GetWeakReference((IWeakReference**)&mBusListener);
    assert(mBusListener != NULL);
}

NativeBusListener::~NativeBusListener()
{}

void NativeBusListener::Setup(
    /* [in] */ IBusAttachment* busAttachment)
{
    mBusAttachment = NULL;
    IWeakReferenceSource::Probe(busAttachment)->GetWeakReference((IWeakReference**)&mBusAttachment);
}

void NativeBusListener::ListenerRegistered(
    /* [in] */ ajn::BusAttachment* bus)
{
    AutoPtr<IBusAttachment> ba;
    mBusAttachment->Resolve(EIID_IBusAttachment, (IInterface**)&ba);
    if (ba == NULL) {
        Logger::E("NativeBusListener", "ListenerRegistered(): Can't get new local reference to BusAttachment");
        return;
    }
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(((CBusAttachment*)ba.Get())->mHandle);
    if (busPtr == NULL) {
        Logger::E("NativeBusListener", "ListenerRegistered(): Exception or NULL bus pointer");
        return;
    }
    assert(bus == busPtr);

    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "ListenerRegistered(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->ListenerRegistered(ba);
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "ListenerRegistered(): Exception");
    }
}

void NativeBusListener::ListenerUnregistered()
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "ListenerUnregistered(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->ListenerUnregistered();
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "ListenerUnregistered(): Exception");
    }
}

void NativeBusListener::FoundAdvertisedName(
    /* [in] */ const char* name,
    /* [in] */ ajn::TransportMask transport,
    /* [in] */ const char* namePrefix)
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "FoundAdvertisedName(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->FoundAdvertisedName(String(name), (Int16)transport, String(namePrefix));
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "FoundAdvertisedName(): Exception");
    }
}

void NativeBusListener::LostAdvertisedName(
    /* [in] */ const char* name,
    /* [in] */ ajn::TransportMask transport,
    /* [in] */ const char* namePrefix)
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "LostAdvertisedName(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->LostAdvertisedName(String(name), (Int16)transport, String(namePrefix));
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "LostAdvertisedName(): Exception");
    }
}

void NativeBusListener::NameOwnerChanged(
    /* [in] */ const char* busName,
    /* [in] */ const char* previousOwner,
    /* [in] */ const char* newOwner)
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "NameOwnerChanged(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->NameOwnerChanged(String(busName), String(previousOwner), String(newOwner));
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "NameOwnerChanged(): Exception");
    }
}

void NativeBusListener::BusStopping()
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "BusStopping(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->BusStopping();
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "BusStopping(): Exception");
    }
}

void NativeBusListener::BusDisconnected()
{
    AutoPtr<IBusListener> bl;
    mBusListener->Resolve(EIID_IBusListener, (IInterface**)&bl);
    if (bl == NULL) {
        Logger::E("NativeBusListener", "BusDisconnected(): Can't get new local reference to BusListener");
        return;
    }

    ECode ec = bl->BusDisconnected();
    if (FAILED(ec)) {
        Logger::E("NativeBusListener", "BusDisconnected(): Exception");
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
