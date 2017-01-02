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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIILocationProviderNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::Location::IILocationProvider;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIILocationProviderNative)
    , public Object
    , public IILocationProvider
    , public IBinder
{
public:
    ~CIILocationProviderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Enable();

    CARAPI Disable();

    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* ws);

    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    CARAPI GetStatus(
        /* [out] */ IBundle** extras,
        /* [out] */ Int32* status);

    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* updateTime);

    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* inExtras,
        /* [out] */ IBundle** outExtras,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__
