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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIProcessObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IIProcessObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIProcessObserverNative)
    , public Object
    , public IIProcessObserver
    , public IBinder
{
public:
    ~CIProcessObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI OnForegroundActivitiesChanged(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean foregroundActivities);

    CARAPI OnProcessStateChanged(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 importance);

    CARAPI OnProcessDied(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPROCESSOBSERVERNATIVE_H__
