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

#include "org/alljoyn/bus/NativeSignalHandler.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Object;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeSignalHandler");

//=============================================================
// NativeSignalHandler
//=============================================================
NativeSignalHandler::NativeSignalHandler(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
    : mMethod(method)
    , mMember(NULL)
{
    IWeakReferenceSource::Probe(obj)->GetWeakReference((IWeakReference**)&mSignalHandler);
}

NativeSignalHandler::~NativeSignalHandler()
{
}

Boolean NativeSignalHandler::IsSameObject(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
{
    AutoPtr<IInterface> handler;
    mSignalHandler->Resolve(EIID_IInterface, (IInterface**)&handler);
    if (!handler) {
        return FALSE;
    }

    return Object::Equals(obj, handler);
}

QStatus NativeSignalHandler::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    if (bus.IsConnected() == false) {
        return ER_BUS_NOT_CONNECTED;
    }
    const ajn::InterfaceDescription* intf = bus.GetInterface(ifaceName);
    if (!intf) {
        Logger::I(TAG, "Register(): ER_BUS_NO_SUCH_INTERFACE ifaceName:[%s], signalName:[%s], ancillary:[%s]",
            ifaceName, signalName, ancillary);
        return ER_BUS_NO_SUCH_INTERFACE;
    }

    mMember = intf->GetMember(signalName);
    if (!mMember) {
        Logger::E(TAG, "Register(): failed to GetMember ifaceName:[%s], signalName:[%s], ancillary:[%s]",
            ifaceName, signalName, ancillary);
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    mAncillaryData = ancillary;
    return ER_OK;
}

void NativeSignalHandler::SignalHandler(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ const char* sourcePath,
    /* [in] */ ajn::Message& msg)
{
    AutoPtr<IInterface> handler;
    mSignalHandler->Resolve(EIID_IInterface, (IInterface**)&handler);
    if (!handler) {
        return;
    }

    NativeMessageContext context(msg);

    const ajn::MsgArg* ajnArgs;
    size_t numArgs;
    msg->GetArgs(numArgs, ajnArgs);
    AutoPtr<IArgumentList> args;
    ECode ec = MsgArg::UnmarshalIn(mMethod, (Int64)ajnArgs, (IArgumentList**)&args);
    if (FAILED(ec)) {
        String methodName;
        mMethod->GetName(&methodName);
        Logger::E(TAG, "Failed to Unmarshal for method %s", methodName.string());
        return;
    }

    ec = mMethod->Invoke(handler, args);
    if (FAILED(ec)) {
        String methodName;
        mMethod->GetName(&methodName);
        Logger::E(TAG, "%s failed to Invoke method %s", TO_CSTR(handler), methodName.string());
    }
}

//=============================================================
// NativeSignalHandlerWithSrc
//=============================================================
QStatus NativeSignalHandlerWithSrc::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    QStatus status = NativeSignalHandler::Register(bus, ifaceName, signalName, ancillary);
    if (status != ER_OK) {
        return status;
    }

    return bus.RegisterSignalHandler(this,
        static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
        mMember,
        mAncillaryData.c_str());
}

void NativeSignalHandlerWithSrc::Unregister(
    /* [in] */ ajn::BusAttachment& bus)
{
    if (bus.IsConnected() == false) {
        return;
    }

    if (mMember) {
        bus.UnregisterSignalHandler(this,
            static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
            mMember,
            mAncillaryData.c_str());
    }
}


//=============================================================
// NativeSignalHandlerWithRule
//=============================================================
QStatus NativeSignalHandlerWithRule::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    QStatus status = NativeSignalHandler::Register(bus, ifaceName, signalName, ancillary);
    if (status != ER_OK) {
        return status;
    }

    return bus.RegisterSignalHandlerWithRule(this,
        static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
        mMember,
        mAncillaryData.c_str());
}

void NativeSignalHandlerWithRule::Unregister(
    /* [in] */ ajn::BusAttachment& bus)
{
    if (bus.IsConnected() == false) {
        return;
    }

    if (mMember) {
        bus.UnregisterSignalHandlerWithRule(this,
            static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
            mMember,
            mAncillaryData.c_str());
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org