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

#include "elastos/droid/javaproxy/CIWallpaperManagerCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IIWallpaperManagerCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperManagerCallbackNative::TAG("CIWallpaperManagerCallbackNative");

CAR_INTERFACE_IMPL_2(CIWallpaperManagerCallbackNative, Object, IIWallpaperManagerCallback, IBinder)

CAR_OBJECT_IMPL(CIWallpaperManagerCallbackNative)

CIWallpaperManagerCallbackNative::~CIWallpaperManagerCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperManagerCallbackNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperManagerCallbackNative::OnWallpaperChanged()
{
    // LOGGERD(TAG, "+ CIWallpaperManagerCallbackNative::OnWallpaperChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IIWallpaperManagerCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIWallpaperManagerCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onWallpaperChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onWallpaperChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onWallpaperChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWallpaperManagerCallbackNative::OnWallpaperChanged()");
    return NOERROR;
}

ECode CIWallpaperManagerCallbackNative::OnKeyguardWallpaperChanged()
{
    assert(0);
    return NOERROR;
}

ECode CIWallpaperManagerCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIWallpaperManagerCallbackNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CIWallpaperManagerCallbackNative::ToString()");
    return NOERROR;
}

}
}
}

