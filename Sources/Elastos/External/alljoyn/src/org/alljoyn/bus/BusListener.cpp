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

#include "org/alljoyn/bus/BusListener.h"
#include "org/alljoyn/bus/NativeBusListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(BusListener, Object, IBusListener);

BusListener::BusListener()
    : mHandle(0)
{
    Create();
}

BusListener::~BusListener()
{
    Destroy();
}

void BusListener::Create()
{
    NativeBusListener* nbl = new NativeBusListener(this);
    assert(nbl != NULL);

    mHandle = reinterpret_cast<Int64>(nbl);
}

void BusListener::Destroy()
{
    NativeBusListener* nbl = reinterpret_cast<NativeBusListener*>(mHandle);

    assert(nbl != NULL);
    delete nbl;

    mHandle = 0;
    return;
}

ECode BusListener::ListenerRegistered(
    /* [in] */ IBusAttachment* bus)
{
    return NOERROR;
}

ECode BusListener::ListenerUnregistered()
{
    return NOERROR;
}

ECode BusListener::FoundAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    return NOERROR;
}

ECode BusListener::LostAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    return NOERROR;
}

ECode BusListener::NameOwnerChanged(
    /* [in] */ const String& busName,
    /* [in] */ const String& previousOwner,
    /* [in] */ const String& newOwner)
{
    return NOERROR;
}

// @Deprecated
// public void propertyChanged(String propName, Variant propValue) {}

ECode BusListener::BusStopping()
{
    return NOERROR;
}

ECode BusListener::BusDisconnected()
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
