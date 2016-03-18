
#include "elastos/droid/javaproxy/CIFullBackupRestoreObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Backup::EIID_IIFullBackupRestoreObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIFullBackupRestoreObserverNative::TAG("CIFullBackupRestoreObserverNative");

CAR_INTERFACE_IMPL_2(CIFullBackupRestoreObserverNative, Object, IIFullBackupRestoreObserver, IBinder)

CAR_OBJECT_IMPL(CIFullBackupRestoreObserverNative)

CIFullBackupRestoreObserverNative::~CIFullBackupRestoreObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIFullBackupRestoreObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnStartBackup()
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnStartBackup()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStartBackup", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStartBackup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStartBackup %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnStartBackup()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnBackupPackage(
    /* [in] */ const String& name)
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnBackupPackage()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jname = Util::ToJavaString(env, name);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onBackupPackage", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onBackupPackage %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onBackupPackage %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnBackupPackage()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndBackup()
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnEndBackup()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndBackup", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndBackup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndBackup %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnEndBackup()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndBackupWithResult(
    /* [in] */ Int32 result)
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnEndBackupWithResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndBackupWithResult", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndBackupWithResult %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, result);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndBackupWithResult %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnEndBackupWithResult()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnStartRestore()
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnStartRestore()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStartRestore", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStartRestore %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStartRestore %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnStartRestore()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnRestorePackage(
    /* [in] */ const String& name)
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnRestorePackage()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jname = Util::ToJavaString(env, name);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onRestorePackage", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onRestorePackage %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onRestorePackage %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnRestorePackage()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndRestore()
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnEndRestore()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndRestore", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndRestore %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndRestore %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnEndRestore()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndRestoreWithResult(
    /* [in] */ Int32 result)
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnEndRestoreWithResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndRestoreWithResult", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndRestoreWithResult %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, result);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndRestoreWithResult %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnEndRestoreWithResult()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnTimeout()
{
    // LOGGERD(TAG, "+ CIFullBackupRestoreObserverNative::OnTimeout()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onTimeout", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onTimeout %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onTimeout %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIFullBackupRestoreObserverNative::OnTimeout()");
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::ToString(
    /* [out] */ String* str)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);
    return NOERROR;
}

}
}
}

