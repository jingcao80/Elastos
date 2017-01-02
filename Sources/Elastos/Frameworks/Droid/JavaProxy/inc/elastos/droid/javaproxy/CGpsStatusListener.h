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

#ifndef __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__
#define __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__

#include "_Elastos_Droid_JavaProxy_CGpsStatusListener.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::IIGpsStatusListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CGpsStatusListener)
    , public Object
    , public IIGpsStatusListener
    , public IBinder
{
public:
    ~CGpsStatusListener();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnGpsStarted();

    CARAPI OnGpsStopped();

    CARAPI OnFirstFix(
        /* [in] */ Int32 ttff);

    CARAPI OnSvStatusChanged(
        /* [in] */ Int32 svCount,
        /* [in] */ ArrayOf<Int32>* prns,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ Int32 ephemerisMask,
        /* [in] */ Int32 almanacMask,
        /* [in] */ Int32 usedInFixMask);

    CARAPI OnNmeaReceived(
        /* [in] */ Int64 timestamp,
        /* [in] */ const String& nmea);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__
