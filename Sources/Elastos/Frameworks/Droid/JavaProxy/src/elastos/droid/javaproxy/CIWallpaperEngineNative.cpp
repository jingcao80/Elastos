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

#include "elastos/droid/javaproxy/CIWallpaperEngineNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Service::Wallpaper::EIID_IIWallpaperEngine;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWallpaperEngineNative::TAG("CIWallpaperEngineNative");

CAR_INTERFACE_IMPL_2(CIWallpaperEngineNative, Object, IIWallpaperEngine, IBinder)

CAR_OBJECT_IMPL(CIWallpaperEngineNative)

CIWallpaperEngineNative::~CIWallpaperEngineNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWallpaperEngineNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWallpaperEngineNative::SetDesiredSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::SetDesiredSize()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setDesiredSize", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setDesiredSize %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)width, (jint)height);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setDesiredSize %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::SetDesiredSize()");
    return NOERROR;
}

ECode CIWallpaperEngineNative::SetDisplayPadding(
    /* [in] */ IRect* padding)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::SetDisplayPadding()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpadding = NULL;
    if (padding != NULL) {
        jpadding = Util::ToJavaRect(env, padding);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setDisplayPadding", "(Landroid/graphics/Rect;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setDisplayPadding %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpadding);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setDisplayPadding %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpadding);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::SetDisplayPadding()");
    return NOERROR;
}


ECode CIWallpaperEngineNative::SetVisibility(
    /* [in] */ Boolean visible)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::SetVisibility()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setVisibility", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setVisibility %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)visible);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setVisibility %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::SetVisibility()");
    return NOERROR;
}

ECode CIWallpaperEngineNative::DispatchPointer(
    /* [in] */ IMotionEvent* event)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::DispatchPointer()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jevent = NULL;
    if (event != NULL) {
        jevent = Util::ToJavaMotionEvent(env, event);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPointer", "(Landroid/view/MotionEvent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchPointer %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jevent);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchPointer %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jevent);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::DispatchPointer()");
    return NOERROR;
}

ECode CIWallpaperEngineNative::DispatchWallpaperCommand(
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::DispatchWallpaperCommand()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jaction = Util::ToJavaString(env, action);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchWallpaperCommand", "(Ljava/lang/String;IIILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchWallpaperCommand %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jaction, (jint)x, (jint)y, (jint)z, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchWallpaperCommand %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jaction);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::DispatchWallpaperCommand()");
    return NOERROR;
}

ECode CIWallpaperEngineNative::Destroy()
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::Destroy()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/wallpaper/IWallpaperEngine");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IWallpaperEngine %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "destroy", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: destroy %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: destroy %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::Destroy()");
    return NOERROR;
}

ECode CIWallpaperEngineNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIWallpaperEngineNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CIWallpaperEngineNative::ToString()");
    return NOERROR;
}

}
}
}

