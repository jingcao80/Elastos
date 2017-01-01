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

#ifndef __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <alljoyn/ProxyBusObject.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::List;

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeProxyBusObject : public ajn::ProxyBusObject
{
public:
    NativeProxyBusObject(
        /* [in] */ IProxyBusObject* pbo,
        /* [in] */ NativeBusAttachment* nba,
        /* [in] */ const char* endpoint,
        /* [in] */ const char* path,
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ bool secure);

    ~NativeProxyBusObject();

    CARAPI RegisterPropertiesChangedListener(
        /* [in] */ const String& ifaceName,
        /* [in] */ ArrayOf<String>* properties,
        /* [in] */ IPropertiesChangedListener* propertiesChangedListener);

    CARAPI UnregisterPropertiesChangedListener(
        /* [in] */ const String& ifaceName,
        /* [in] */ IPropertiesChangedListener* propertiesChangedListener);

    using ajn::ProxyBusObject::AddInterface;

private:
    static ECode AddInterfaceStatus(
        /* [in] */ IProxyBusObject* pbo,
        /* [in] */ NativeBusAttachment* busPtr,
        /* [in] */ const String& interfaceName);

    static ECode AddInterface(
        /* [in] */ IProxyBusObject* pbo,
        /* [in] */ NativeBusAttachment* busPtr,
        /* [in] */ const String& interfaceName);

private:
    NativeProxyBusObject(const NativeProxyBusObject& other);
    NativeProxyBusObject& operator =(const NativeProxyBusObject& other);

    Object mPropertiesChangedListenersLock;

    class Listener
        : public Object
    {
      public:
        const String mIfaceName;
        AutoPtr<IPropertiesChangedListener> mListener;

        Listener(
            /* [in] */ const String& ifaceName,
            /* [in] */ IPropertiesChangedListener* listener)
            : mIfaceName(ifaceName), mListener(listener)
        {}

      private:
        Listener& operator =(const Listener& other);
    };
    List< AutoPtr<Listener> > mPropertiesChangedListeners;

public:
    NativeBusAttachment* mBusPtr;
    AutoPtr<IWeakReference> mPbo;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__
