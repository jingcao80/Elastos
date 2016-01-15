
#include "CAppWidgetHostCallbacksNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CAppWidgetHostCallbacksNative::TAG("CAppWidgetHostCallbacksNative");

CAppWidgetHostCallbacksNative::CAppWidgetHostCallbacksNative(){

}

CAppWidgetHostCallbacksNative::~CAppWidgetHostCallbacksNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CAppWidgetHostCallbacksNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::UpdateAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IParcelable* views)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IAppWidgetHost"), __LINE__);
    jmethodID m = env->GetMethodID(c, "updateAppWidget", "(ILandroid/widget/RemoteViews;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: updateAppWidget"), __LINE__);

    AutoPtr<IRemoteViews> remoteViews;

    if(views != NULL){
        remoteViews = IRemoteViews::Probe(views);
    }

    jobject jremoteviews = Util::ToJavaRemoteViews(env, remoteViews.Get());

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, jremoteviews);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:updateAppWidget()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jremoteviews);
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ProviderChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* info)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IAppWidgetHost"), __LINE__);

    jmethodID m = env->GetMethodID(c, "providerChanged", "(ILandroid/appwidget/AppWidgetProviderInfo;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: providerChanged"), __LINE__);

    jobject jinfo = Util::ToJavaAppWidgetProviderInfo(env, info);

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, jinfo);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:providerChanged()"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jinfo);
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ProvidersChanged()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IAppWidgetHost"), __LINE__);

    jmethodID m = env->GetMethodID(c, "providersChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: providersChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:providersChanged()"), __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CAppWidgetHostCallbacksNative::ViewDataChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/appwidget/IAppWidgetHost");
    Util::CheckErrorAndLog(env, TAG, String("FindClass: IAppWidgetHost"), __LINE__);

    jmethodID m = env->GetMethodID(c, "viewDataChanged", "(II)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: viewDataChanged"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)appWidgetId, (jint)viewId);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod:viewDataChanged()"), __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

}
}
}

