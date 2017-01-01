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

#ifndef __ORG_ALLJOYN_BUS_NATIVESIGNALHANDLER_H__
#define __ORG_ALLJOYN_BUS_NATIVESIGNALHANDLER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusAttachment.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/MessageReceiver.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeSignalHandler : public ajn::MessageReceiver
{
public:
    NativeSignalHandler(
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* method);

    virtual ~NativeSignalHandler();

    Boolean IsSameObject(
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* method);

    virtual QStatus Register(
        /* [in] */ ajn::BusAttachment& bus,
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* signalName,
        /* [in] */ const char* ancillary);

    virtual void Unregister(
        /* [in] */ ajn::BusAttachment& bus) = 0;

    void SignalHandler(
        /* [in] */ const ajn::InterfaceDescription::Member* member,
        /* [in] */ const char* sourcePath,
        /* [in] */ ajn::Message& msg);

private:
    NativeSignalHandler(const NativeSignalHandler& other);
    NativeSignalHandler& operator =(const NativeSignalHandler& other);

protected:
    AutoPtr<IWeakReference> mSignalHandler;
    AutoPtr<IMethodInfo> mMethod;
    const ajn::InterfaceDescription::Member* mMember;
    qcc::String mAncillaryData; /* can be both source or matchRule; */
};

class NativeSignalHandlerWithSrc
    : public NativeSignalHandler
{
public:
    NativeSignalHandlerWithSrc(
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* method)
        : NativeSignalHandler(obj, method)
    { }

    virtual QStatus Register(
        /* [in] */ ajn::BusAttachment& bus,
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* signalName,
        /* [in] */ const char* ancillary);

private:
    NativeSignalHandlerWithSrc(const NativeSignalHandlerWithSrc& other);
    NativeSignalHandlerWithSrc& operator =(const NativeSignalHandlerWithSrc& other);

    void Unregister(
        /* [in] */ ajn::BusAttachment& bus);
};

class NativeSignalHandlerWithRule
    : public NativeSignalHandler
{
public:
    NativeSignalHandlerWithRule(
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* method)
        : NativeSignalHandler(obj, method)
    {}

    virtual QStatus Register(
        /* [in] */ ajn::BusAttachment& bus,
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* signalName,
        /* [in] */ const char* ancillary);

private:
    NativeSignalHandlerWithRule(const NativeSignalHandlerWithRule& other);
    NativeSignalHandlerWithRule& operator =(const NativeSignalHandlerWithRule& other);

    void Unregister(
        /* [in] */ ajn::BusAttachment& bus);
};


} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVESIGNALHANDLER_H__
