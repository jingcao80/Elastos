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

#include "elastos/droid/javaproxy/CAppWidgetHostCallbacksNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::AppWidget::EIID_IIAppWidgetHost;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CAppWidgetHostCallbacksNative::TAG("CAppWidgetHostCallbacksNative");

CAR_INTERFACE_IMPL_2(CAppWidgetHostCallbacksNative, Object, IIAppWidgetHost, IBinder)

CAR_OBJECT_IMPL(CAppWidgetHostCallbacksNative)

CAppWidgetHostCallbacksNative::~CAppWidgetHostCallbacksNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CAppWidgetHostCallbacksNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::UpdateAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* remoteViews)
{
    // LOGGERD(TAG, "+ UpdateAppWidget()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAppWidgetHost", __LINE__);
    jmethodID m = env->GetMethodID(c, "updateAppWidget", "(ILandroid/widget/RemoteViews;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: updateAppWidget", __LINE__);

    jobject jremoteviews = Util::ToJavaRemoteViews(env, remoteViews);

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, jremoteviews);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:updateAppWidget()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jremoteviews);
    // LOGGERD(TAG, "- UpdateAppWidget()");
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ProviderChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* info)
{
    // LOGGERD(TAG, "+ ProviderChanged()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAppWidgetHost", __LINE__);

    jmethodID m = env->GetMethodID(c, "providerChanged", "(ILandroid/appwidget/AppWidgetProviderInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: providerChanged", __LINE__);

    jobject jinfo = Util::ToJavaAppWidgetProviderInfo(env, info);

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, jinfo);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:providerChanged()", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jinfo);
    // LOGGERD(TAG, "- ProviderChanged()");
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ProvidersChanged()
{
    // LOGGERD(TAG, "+ ProvidersChanged()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAppWidgetHost", __LINE__);

    jmethodID m = env->GetMethodID(c, "providersChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: providersChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:providersChanged()", __LINE__);

    env->DeleteLocalRef(c);
    // LOGGERD(TAG, "- ProvidersChanged()");
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ViewDataChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    // LOGGERD(TAG, "+ ViewDataChanged()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAppWidgetHost", __LINE__);

    jmethodID m = env->GetMethodID(c, "viewDataChanged", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: viewDataChanged", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, (jint)viewId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod:viewDataChanged()", __LINE__);

    env->DeleteLocalRef(c);
    // LOGGERD(TAG, "- ViewDataChanged()");
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CAppWidgetHostCallbacksNative::ToString()");
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

    // LOGGERD(TAG, "- CAppWidgetHostCallbacksNative::ToString()");
    return NOERROR;
}

}
}
}

