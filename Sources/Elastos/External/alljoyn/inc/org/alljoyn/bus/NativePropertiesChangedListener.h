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

#ifndef __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/ProxyBusObject.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativePropertiesChangedListener : public ajn::ProxyBusObject::PropertiesChangedListener
{
public:
    NativePropertiesChangedListener(
        /* [in] */ IWeakReference* listener,
        /* [in] */ const String& changedSig,
        /* [in] */ const String& invalidated);

    ~NativePropertiesChangedListener();

    void PropertiesChanged(
        /* [in] */ ajn::ProxyBusObject& obj,
        /* [in] */ const char* ifaceName,
        /* [in] */ const ajn::MsgArg& changed,
        /* [in] */ const ajn::MsgArg& invalidated,
        /* [in] */ void* context);

private:
    NativePropertiesChangedListener();
    NativePropertiesChangedListener(const NativePropertiesChangedListener& other);
    NativePropertiesChangedListener& operator =(const NativePropertiesChangedListener& other);

    AutoPtr<IWeakReference> mListener;

    String mChangedSignature;
    String mInvalidatedSignature;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__
