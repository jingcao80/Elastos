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

#include "org/alljoyn/bus/NativePropertiesChangedListener.h"
#include "org/alljoyn/bus/NativeProxyBusObject.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativePropertiesChangedListener");
static const Boolean DEBUG = FALSE;

NativePropertiesChangedListener::NativePropertiesChangedListener(
    /* [in] */ IWeakReference* listener,
    /* [in] */ const String& changed,
    /* [in] */ const String& invalidated)
    : mListener(listener)
    , mChangedSignature(changed)
    , mInvalidatedSignature(invalidated)
{}

NativePropertiesChangedListener::~NativePropertiesChangedListener()
{}

void NativePropertiesChangedListener::PropertiesChanged(
    /* [in] */ ajn::ProxyBusObject& obj,
    /* [in] */ const char* ifaceName,
    /* [in] */ const ajn::MsgArg& changed,
    /* [in] */ const ajn::MsgArg& invalidated,
    /* [in] */ void* context)
{
    ECode ec = NOERROR;
    /*
     * Translate the C++ formal parameters into their JNI counterparts.
     */
    String jifaceName(ifaceName);

    AutoPtr<IInterface> changedObj;
    ec = MsgArg::UnmarshalInterface((Int64)&changed, mChangedSignature, (IInterface**)&changedObj);
    if (FAILED(ec)) {
        Logger::E(TAG, "PropertiesChanged failed to extracting changed dictionary with signature: %s",
            mChangedSignature.string());
    }
    AutoPtr<IMap> jchanged = IMap::Probe(changedObj);

    AutoPtr<ArrayOf<String> > jinvalidated;
    ec = MsgArg::Unmarshal((Int64)&invalidated, (PVoid)&jinvalidated);
    if (FAILED(ec)) {
        Logger::E(TAG, "PropertiesChanged failed to extracting invalidated list with signature: %s",
            mChangedSignature.string());
    }

    /*
     * The weak global reference jbusListener cannot be directly used.  We have to get
     * a "hard" reference to it and then use that.  If you try to use a weak reference
     * directly you will crash and burn.
     */
    AutoPtr<IPropertiesChangedListener> jo;
    mListener->Resolve(EIID_IPropertiesChangedListener, (IInterface**)&jo);
    if (!jo) {
        Logger::E(TAG, "NativePropertiesChangedListener::PropertiesChanged():"
            " Can't get new local reference to ProxyBusObjectListener");
        return;
    }

    /*
     * This call out to the property changed handler implies that the Java method must be
     * MT-safe.  This is implied by the definition of the listener.
     */
    NativeProxyBusObject* npbo = (NativeProxyBusObject*)context;
    AutoPtr<IProxyBusObject> pbo;
    npbo->mPbo->Resolve(EIID_IProxyBusObject, (IInterface**)&pbo);
    if (pbo) {
        if (DEBUG) {
            Logger::I(TAG, " >> PropertiesChanged: %s, changed: %s, invalidated: %s",
                jifaceName.string(), TO_CSTR(jchanged), Arrays::ToString(jinvalidated).string());
        }
        jo->PropertiesChanged(pbo, jifaceName, jchanged, jinvalidated);
    }
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org