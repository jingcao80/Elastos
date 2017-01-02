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

#include "elastos/droid/javaproxy/CLocationListener.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IILocationListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CLocationListener::TAG("CLocationListener");

CAR_INTERFACE_IMPL_2(CLocationListener, Object, IILocationListener, IBinder)

CAR_OBJECT_IMPL(CLocationListener)

CLocationListener::~CLocationListener()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CLocationListener::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    // LOGGERD(TAG, "+ CLocationListener::OnLocationChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jlocation = Util::ToJavaLocation(env, location);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onLocationChanged", "(Landroid/location/Location;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onLocationChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, jlocation);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:onLocationChanged()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jlocation);

    // LOGGERD(TAG, "- CLocationListener::OnLocationChanged()");
    return NOERROR;
}

ECode CLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    // LOGGERD(TAG, "+ CLocationListener::OnStatusChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);
    jobject jextras = Util::ToJavaBundle(env, extras);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStatusChanged", "(Ljava/lang/String;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStatusChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider, status, jextras);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:onStatusChanged()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CLocationListener::OnStatusChanged()");
    return NOERROR;
}

ECode CLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    // LOGGERD(TAG, "+ CLocationListener::OnProviderEnabled()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onProviderEnabled", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onProviderEnabled", __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:onProviderEnabled()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    // LOGGERD(TAG, "- CLocationListener::OnProviderEnabled()");
    return NOERROR;
}

ECode CLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    // LOGGERD(TAG, "+ CLocationListener::OnProviderDisabled()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jprovider = Util::ToJavaString(env, provider);

    jclass c = env->FindClass("android/location/ILocationListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ILocationListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onProviderDisabled", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onProviderDisabled", __LINE__);

    env->CallVoidMethod(mJInstance, m, jprovider);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:onProviderDisabled()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jprovider);
    // LOGGERD(TAG, "- CLocationListener::OnProviderDisabled()");
    return NOERROR;
}

ECode CLocationListener::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CLocationListener::ToString()");
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

    // LOGGERD(TAG, "- CLocationListener::ToString()");
    return NOERROR;
}

}
}
}

