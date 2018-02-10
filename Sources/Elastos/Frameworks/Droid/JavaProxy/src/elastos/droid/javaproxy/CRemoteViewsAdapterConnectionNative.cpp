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

#include "elastos/droid/javaproxy/CRemoteViewsAdapterConnectionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Widget::EIID_IIRemoteViewsAdapterConnection;
using Elastos::Droid::Internal::Widget::IIRemoteViewsFactory;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CRemoteViewsAdapterConnectionNative::TAG("CRemoteViewsAdapterConnectionNative");

CAR_INTERFACE_IMPL_2(CRemoteViewsAdapterConnectionNative, Object, IIRemoteViewsAdapterConnection, IBinder)

CAR_OBJECT_IMPL(CRemoteViewsAdapterConnectionNative)

CRemoteViewsAdapterConnectionNative::~CRemoteViewsAdapterConnectionNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CRemoteViewsAdapterConnectionNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CRemoteViewsAdapterConnectionNative::OnServiceConnected(
    /* [in] */ IBinder* service)
{
    // LOGGERD(TAG, "+ CRemoteViewsAdapterConnectionNative::OnServiceConnected()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    AutoPtr<IIRemoteViewsFactory> factory = IIRemoteViewsFactory::Probe(service);

    if(factory != NULL){
        jclass proxyClass = env->FindClass("com/android/internal/widget/ElIRemoteViewsFactoryProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIRemoteViewsFactoryProxy %d", __LINE__);

        jmethodID mid = env->GetMethodID(proxyClass, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "Fail GetMethodID: ElIRemoteViewsFactoryProxy(I) %d", __LINE__);

        jobject jfactoryProxy = env->NewObject(proxyClass, mid, (jlong)factory.Get());
        Util::CheckErrorAndLog(env, "ToJavaComponentName", "Fail NewObject: componentNameKlass", __LINE__);
        factory->AddRef();

        env->DeleteLocalRef(proxyClass);

        jclass connClass = env->FindClass("android/widget/RemoteViewsAdapter$RemoteViewsAdapterServiceConnection");
        Util::CheckErrorAndLog(env, TAG, "FindClass: RemoteViewsAdapter$RemoteViewsAdapterServiceConnection %d", __LINE__);

        jmethodID m = env->GetMethodID(connClass, "onServiceConnected", "(Landroid/os/IBinder;)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: onServiceConnected : %d!\n", __LINE__);

        env->CallVoidMethod(mJInstance, m, jfactoryProxy);
        Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onServiceConnected : %d!\n", __LINE__);

        env->DeleteLocalRef(connClass);
        env->DeleteLocalRef(jfactoryProxy);
    }else{
        Logger::E(TAG, "CRemoteViewsAdapterConnectionNative::OnServiceConnected() factory null");
    }

    // LOGGERD(TAG, "- CRemoteViewsAdapterConnectionNative::OnServiceConnected()");

    return NOERROR;
}

ECode CRemoteViewsAdapterConnectionNative::OnServiceDisconnected()
{
    // LOGGERD(TAG, "+ CRemoteViewsAdapterConnectionNative::OnServiceDisconnected()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass connClass = env->FindClass("android/widget/RemoteViewsAdapter$RemoteViewsAdapterServiceConnection");
    Util::CheckErrorAndLog(env, TAG, "FindClass: RemoteViewsAdapter$RemoteViewsAdapterServiceConnection %d", __LINE__);

    jmethodID m = env->GetMethodID(connClass, "onServiceDisconnected", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onServiceDisconnected : %d!\n", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onServiceDisconnected : %d!\n", __LINE__);

    env->DeleteLocalRef(connClass);
    // LOGGERD(TAG, "- CRemoteViewsAdapterConnectionNative::OnServiceDisconnected()");

    return NOERROR;
}

ECode CRemoteViewsAdapterConnectionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CRemoteViewsAdapterConnectionNative::ToString()");
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

    // LOGGERD(TAG, "- CRemoteViewsAdapterConnectionNative::ToString()");
    return NOERROR;
}

}
}
}

