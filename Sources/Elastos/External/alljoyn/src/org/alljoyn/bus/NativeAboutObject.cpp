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

#include "org/alljoyn/bus/NativeAboutObject.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeAboutObject::NativeAboutObject(
    /* [in] */ NativeBusAttachment* bus,
    /* [in] */ AnnounceFlag isAboutIntfAnnounced)
    : AboutObj(*reinterpret_cast<ajn::BusAttachment*>(bus), isAboutIntfAnnounced)
    , mBusPtr(bus)
{}

NativeAboutObject::~NativeAboutObject()
{}

QStatus NativeAboutObject::Announce(
    /* [in] */ IAboutObj* thiz,
    /* [in] */ Int16 sessionPort,
    /* [in] */ IAboutDataListener* aboutDataListener)
{
    mAboutDataListenerRef = aboutDataListener;
    return ajn::AboutObj::Announce(static_cast<ajn::SessionPort>(sessionPort), *this);
}

QStatus NativeAboutObject::GetAboutData(
    /* [in] */ ajn::MsgArg* msgArg,
    /* [in] */ const char* language)
{
    QStatus status = ER_FAIL;
    if (mAboutDataListenerRef != NULL) {
        AutoPtr<IMap> announceArg;
        ECode ec = mAboutDataListenerRef->GetAboutData(String(language), (IMap**)&announceArg);
        // check for ErrorReplyBusException exception
        status = CheckForThrownException(ec);
        if (ER_OK == status) {
            // Marshal the returned value
            if (Marshal("a{sv}", announceArg.Get(), msgArg) == NULL) {
                Logger::E("NativeAboutObject", "GetMsgArgAnnounce() marshaling error");
                return ER_FAIL;
            }
        }
        else {
            Logger::E("NativeAboutObject", "GetMsgArg exception with status %s", QCC_StatusText(status));
            return status;
        }
    }
    return ER_OK;
}

QStatus NativeAboutObject::GetAnnouncedAboutData(
    /* [in] */ ajn::MsgArg* msgArg)
{
    QStatus status = ER_FAIL;
    if (mAboutDataListenerRef != NULL) {
        AutoPtr<IMap> announceArg;
        ECode ec = mAboutDataListenerRef->GetAnnouncedAboutData((IMap**)&announceArg);
        // check for ErrorReplyBusException exception
        status = CheckForThrownException(ec);
        if (ER_OK == status) {
            if (Marshal("a{sv}", announceArg.Get(), msgArg) == NULL) {
                Logger::E("NativeAboutObject", "GetMsgArgAnnounce() marshaling error");
                return ER_FAIL;
            }
        }
        else {
            Logger::E("NativeAboutObject", "GetAnnouncedAboutData exception with status %s", QCC_StatusText(status));
            return status;
        }
    }
    return status;
}

QStatus NativeAboutObject::CheckForThrownException(
    /* [in] */ ECode ec)
{
    return (QStatus)ec;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
