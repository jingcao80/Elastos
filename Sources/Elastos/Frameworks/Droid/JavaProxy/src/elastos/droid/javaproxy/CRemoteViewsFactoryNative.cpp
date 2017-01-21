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

#include "elastos/droid/javaproxy/CRemoteViewsFactoryNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Widget::EIID_IIRemoteViewsFactory;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CRemoteViewsFactoryNative::TAG("CRemoteViewsFactoryNative");

CAR_INTERFACE_IMPL_2(CRemoteViewsFactoryNative, Object, IIRemoteViewsFactory, IBinder)

CAR_OBJECT_IMPL(CRemoteViewsFactoryNative)

CRemoteViewsFactoryNative::~CRemoteViewsFactoryNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CRemoteViewsFactoryNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CRemoteViewsFactoryNative::ToString()");
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

    // LOGGERD(TAG, "- CRemoteViewsFactoryNative::ToString()");
    return NOERROR;
}

ECode CRemoteViewsFactoryNative::OnDataSetChanged()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "onDataSetChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onDataSetChanged : %d!\n", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onDataSetChanged : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    return NOERROR;
}

ECode CRemoteViewsFactoryNative::OnDataSetChangedAsync()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "onDataSetChangedAsync", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onDataSetChangedAsync : %d!\n", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onDataSetChangedAsync : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    return NOERROR;
}

ECode CRemoteViewsFactoryNative::OnDestroy(
    /* [in] */ IIntent* intent)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "onDestroy", "(Landroid/content/Intent;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onDestroy : %d!\n", __LINE__);

    jobject jintent = Util::ToJavaIntent(env, intent);

    env->CallVoidMethod(mJInstance, m, jintent);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onDestroy : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    env->DeleteLocalRef(jintent);

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::GetCount(
    /* [out] */ Int32* count)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "getCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getCount : %d!\n", __LINE__);

    jint jcount = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getCount : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    *count = jcount;

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::GetViewAt(
    /* [in] */ Int32 position,
    /* [out] */ IParcelable** views)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    *views = NULL;

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "getViewAt", "(I)Landroid/widget/RemoteViews;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getViewAt : %d!\n", __LINE__);

    jobject jremoteViews = env->CallObjectMethod(mJInstance, m, (Int32)position);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getViewAt : %d!\n", __LINE__);

    AutoPtr<IRemoteViews> remoteViews;
    Util::GetElRemoteViews(env, jremoteViews, (IRemoteViews**)&remoteViews);
    env->DeleteLocalRef(jremoteViews);

    if(remoteViews != NULL){
        *views = IParcelable::Probe(remoteViews.Get());
        if (*views) (*views)->AddRef();
    }
    else{
        Logger::E(TAG, "CRemoteViewsFactoryNative::GetViewAt(), got remoteViews null");
    }

    env->DeleteLocalRef(fvfClass);

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::GetLoadingView(
    /* [out] */ IParcelable** views)
{
    assert(views != NULL);
    *views = NULL;

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "getLoadingView", "()Landroid/widget/RemoteViews;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getLoadingView : %d!\n", __LINE__);

    jobject jremoteViews = env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getLoadingView : %d!\n", __LINE__);

    AutoPtr<IRemoteViews> remoteViews;
    Util::GetElRemoteViews(env, jremoteViews, (IRemoteViews**)&remoteViews);
    env->DeleteLocalRef(jremoteViews);

    if(remoteViews != NULL){
        *views = IParcelable::Probe(remoteViews.Get());
        if (*views) (*views)->AddRef();
    }

    env->DeleteLocalRef(fvfClass);

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "getViewTypeCount", "()I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getViewTypeCount : %d!\n", __LINE__);

    jint jcount = env->CallIntMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getViewTypeCount : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    *count = jcount;

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "getItemId", "(I)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getItemId : %d!\n", __LINE__);

    jlong jid = env->CallLongMethod(mJInstance, m, position);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getItemId : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    *id = jid;

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::HasStableIds(
    /* [out] */ Boolean* res)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "hasStableIds", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: hasStableIds : %d!\n", __LINE__);

    jboolean jresult = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: hasStableIds : %d!\n", __LINE__);

    env->DeleteLocalRef(fvfClass);
    *res = jresult;

    return NOERROR;
}

ECode CRemoteViewsFactoryNative::IsCreated(
    /* [out] */ Boolean* res)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass fvfClass = env->FindClass("com/android/internal/widget/IRemoteViewsFactory");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRemoteViewsFactory %d", __LINE__);

    jmethodID m = env->GetMethodID(fvfClass, "isCreated", "()Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: isCreated : %d!\n", __LINE__);

    jboolean result = env->CallBooleanMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: isCreated : %d!\n", __LINE__);

    *res = (Boolean)result;

    env->DeleteLocalRef(fvfClass);
    return NOERROR;
}

}
}
}

