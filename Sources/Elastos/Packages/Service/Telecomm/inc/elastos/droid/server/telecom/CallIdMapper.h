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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/** Utility to map {@link Call} objects to unique IDs. IDs are generated when a call is added. */
class CallIdMapper
    : public Object
    , public ICallIdMapper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& callIdPrefix);

    CARAPI ReplaceCall(
        /* [in] */ ICall* newCall,
        /* [in] */ ICall* callToReplace);

    CARAPI AddCall(
        /* [in] */ ICall* call,
        /* [in] */ const String& id);

    CARAPI AddCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ ICall* call);

    CARAPI RemoveCall(
        /* [in] */ const String& callId);

    CARAPI GetCallId(
        /* [in] */ ICall* call,
        /* [out] */ String* result);

    CARAPI GetCall(
        /* [in] */ IInterface* objId,
        /* [out] */ ICall** result);

    CARAPI Clear();

    CARAPI IsValidCallId(
        /* [in] */ const String& callId,
        /* [out] */ Boolean* result);

    CARAPI IsValidConferenceId(
        /* [in] */ const String& callId,
        /* [out] */ Boolean* result);

    CARAPI GetNewId(
        /* [out] */ String* result);

private:
    AutoPtr<IHashMap> mCalls; // HashBiMap

    String mCallIdPrefix;

    static Int32 sIdCount;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLIDMAPPER_H__
