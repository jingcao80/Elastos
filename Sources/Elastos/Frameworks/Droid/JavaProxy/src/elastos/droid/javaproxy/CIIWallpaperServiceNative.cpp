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

#include "elastos/droid/javaproxy/CIIWallpaperServiceNative.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Service::Wallpaper::EIID_IIWallpaperService;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIIWallpaperServiceNative::TAG("CIIWallpaperServiceNative");

CAR_INTERFACE_IMPL_2(CIIWallpaperServiceNative, Object, IIWallpaperService, IBinder)

CAR_OBJECT_IMPL(CIIWallpaperServiceNative)

CIIWallpaperServiceNative::~CIIWallpaperServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIIWallpaperServiceNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIIWallpaperServiceNative::Attach(
    /* [in] */ IIWallpaperConnection* connection,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 windowType,
    /* [in] */ Boolean isPreview,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight,
    /* [in] */ IRect* padding)
{
    // LOGGERD(TAG, "+ CIIWallpaperServiceNative::Attach()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jconnection = NULL;
    if (connection != NULL) {
        jclass c = env->FindClass("android/service/wallpaper/ElWallpaperConnectionProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElWallpaperConnectionProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElWallpaperConnectionProxy %d", __LINE__);

        jconnection = env->NewObject(c, m, (jlong)connection);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElWallpaperConnectionProxy %d", __LINE__);
        connection->AddRef();

        env->DeleteLocalRef(c);
    }

    jobject jwindowToken = NULL;
    if (windowToken != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy %d", __LINE__);

        jwindowToken = env->NewObject(c, m, (jlong)windowToken);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy %d", __LINE__);
        windowToken->AddRef();

        env->DeleteLocalRef(c);
    }

    jobject jpadding = NULL;
    if (padding != NULL) {
        jpadding = Util::ToJavaRect(env, padding);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperService");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "attach", "(Landroid/service/wallpaper/IWallpaperConnection;Landroid/os/IBinder;IZII;Landroid/graphics/Rect;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: attach %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jconnection, jwindowToken, (jint)windowType, (jboolean)isPreview, (jint)reqWidth, (jint)reqHeight, jpadding);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attach %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jconnection);
    env->DeleteLocalRef(jwindowToken);
    env->DeleteLocalRef(jpadding);

    // LOGGERD(TAG, "- CIIWallpaperServiceNative::Attach()");
    return NOERROR;
}

ECode CIIWallpaperServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIIWallpaperServiceNative::ToString()");

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

    // LOGGERD(TAG, "- CIIWallpaperServiceNative::ToString()");
    return NOERROR;
}

}
}
}

