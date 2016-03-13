
#include "elastos/droid/javaproxy/CRemoteViewsAdapterConnectionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Widget::Internal::IIRemoteViewsFactory;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CRemoteViewsAdapterConnectionNative::TAG("CRemoteViewsAdapterConnectionNative");

CRemoteViewsAdapterConnectionNative::CRemoteViewsAdapterConnectionNative(){
}

CRemoteViewsAdapterConnectionNative::~CRemoteViewsAdapterConnectionNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}


ECode CRemoteViewsAdapterConnectionNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
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
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    AutoPtr<IIRemoteViewsFactory> factory = IIRemoteViewsFactory::Probe(service);

    if(factory != NULL){
        jclass proxyClass = env->FindClass("com/android/internal/widget/ElIRemoteViewsFactoryProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElIRemoteViewsFactoryProxy %d", __LINE__);

        jmethodID mid = env->GetMethodID(proxyClass, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "Fail GetMethodID: ElIRemoteViewsFactoryProxy(I) %d", __LINE__);

        jobject jfactoryProxy = env->NewObject(proxyClass, mid, (Int32)factory.Get());
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

    return NOERROR;
}

ECode CRemoteViewsAdapterConnectionNative::OnServiceDisconnected()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass connClass = env->FindClass("android/widget/RemoteViewsAdapter$RemoteViewsAdapterServiceConnection");
    Util::CheckErrorAndLog(env, TAG, "FindClass: RemoteViewsAdapter$RemoteViewsAdapterServiceConnection %d", __LINE__);

    jmethodID m = env->GetMethodID(connClass, "onServiceDisconnected", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onServiceDisconnected : %d!\n", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onServiceDisconnected : %d!\n", __LINE__);

    env->DeleteLocalRef(connClass);

    return NOERROR;
}

ECode CRemoteViewsAdapterConnectionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CRemoteViewsAdapterConnectionNative::ToString()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CRemoteViewsAdapterConnectionNative::ToString()"));
    return NOERROR;
}

}
}
}

