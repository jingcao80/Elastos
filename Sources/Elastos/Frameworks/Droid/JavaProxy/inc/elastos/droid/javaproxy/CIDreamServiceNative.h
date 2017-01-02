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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIDreamServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Service::Dreams::IIDreamService;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIDreamServiceNative)
    , public Object
    , public IIDreamService
    , public IBinder
{
public:
    ~CIDreamServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Attach(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Boolean canDoze);

    CARAPI Detach();

    CARAPI WakeUp();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__
