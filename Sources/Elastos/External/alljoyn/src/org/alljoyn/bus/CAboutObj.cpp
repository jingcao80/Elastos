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

#include "org/alljoyn/bus/CAboutObj.h"
#include "org/alljoyn/bus/NativeAboutObject.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("CAboutObj");

CAR_INTERFACE_IMPL(CAboutObj, Object, IAboutObj)

CAR_OBJECT_IMPL(CAboutObj)

CAboutObj::CAboutObj()
    : mHandle(0)
{}

CAboutObj::~CAboutObj()
{
    Destory();
}

ECode CAboutObj::constructor(
    /* [in] */ IBusAttachment* bus)
{
    return constructor(bus, FALSE);
}

ECode CAboutObj::constructor(
    /* [in] */ IBusAttachment* bus,
    /* [in] */ Boolean isAboutObjectAnnounced)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(((CBusAttachment*)bus)->mHandle);
    NativeAboutObject* aboutObj = NULL;
    if (isAboutObjectAnnounced) {
        aboutObj = new NativeAboutObject(busPtr, ajn::BusObject::ANNOUNCED);
    }
    else {
        aboutObj = new NativeAboutObject(busPtr, ajn::BusObject::UNANNOUNCED);
    }

    // Make the JAboutObj Accessable to the BusAttachment so it can be used
    // by the BusAttachment to Release the global ref contained in the JAboutObject
    // when the BusAttachment shuts down.
    aboutObj->mBusPtr->mAboutObj = aboutObj;
    // Incrament the ref so the BusAttachment will not be deleted before the About
    // Object.
    aboutObj->mBusPtr->IncRef();

    mHandle = reinterpret_cast<Int64>(aboutObj);
    return NOERROR;
}

void CAboutObj::Destory()
{
    if (mHandle != 0) {
        NativeAboutObject* aboutObj = reinterpret_cast<NativeAboutObject*>(mHandle);
        NativeBusAttachment* busPtr = aboutObj->mBusPtr;

        //Remove the BusAttachments pointer to the JAboutObject
        {
            AutoLock lock(busPtr->mBaCommonLock);
            busPtr->mAboutObj = NULL;
        }

        delete aboutObj;
        mHandle = 0;

        // Decrament the ref pointer so the BusAttachment can be released.
        busPtr->DecRef();
    }
}

ECode CAboutObj::Announce(
    /* [in] */ Int16 sessionPort,
    /* [in] */ IAboutDataListener* aboutData)
{
    NativeAboutObject* aboutObj = reinterpret_cast<NativeAboutObject*>(mHandle);
    if (aboutObj == NULL) {
        Logger::E(TAG, "Announce(): null AboutObj Exception");
        return E_STATUS_FAIL;
    }

    {
        // if we don't already have a GlobalRef obtain a GlobalRef
        AutoLock lock(aboutObj->mBusPtr->mBaCommonLock);
        if (aboutObj->mAboutObjGlobalRef == NULL) {
            aboutObj->mAboutObjGlobalRef = this;
        }
    }

    return aboutObj->Announce(this, sessionPort, aboutData);
}

ECode CAboutObj::Unannounce()
{
    NativeAboutObject* aboutObj = reinterpret_cast<NativeAboutObject*>(mHandle);
    if (aboutObj == NULL) {
        Logger::E(TAG, "Announce(): null AboutObj Exception");
        return E_STATUS_FAIL;
    }

    {
        // Release the GlobalRef it will be re-obtained if announce is called again
        AutoLock lock(aboutObj->mBusPtr->mBaCommonLock);
        if (aboutObj->mAboutObjGlobalRef != NULL) {
            aboutObj->mAboutObjGlobalRef = NULL;
        }
    }

    return aboutObj->Unannounce();
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
