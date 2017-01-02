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

#include "elastos/droid/javaproxy/CIGpsMeasurementsListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IIGpsMeasurementsListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIGpsMeasurementsListenerNative::TAG("CIGpsMeasurementsListenerNative");

CAR_INTERFACE_IMPL_2(CIGpsMeasurementsListenerNative, Object, IIGpsMeasurementsListener, IBinder)

CAR_OBJECT_IMPL(CIGpsMeasurementsListenerNative)

CIGpsMeasurementsListenerNative::~CIGpsMeasurementsListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIGpsMeasurementsListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIGpsMeasurementsListenerNative::OnGpsMeasurementsReceived(
    /* [in] */ IGpsMeasurementsEvent* event)
{
    assert(0);
    return NOERROR;
}

ECode CIGpsMeasurementsListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIGpsMeasurementsListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIGpsMeasurementsListenerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
