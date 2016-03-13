
#include "elastos/droid/javaproxy/CIFullBackupRestoreObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIFullBackupRestoreObserverNative::TAG("CIFullBackupRestoreObserverNative");

CIFullBackupRestoreObserverNative::~CIFullBackupRestoreObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIFullBackupRestoreObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnStartBackup()
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnStartBackup()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStartBackup", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStartBackup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStartBackup %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnStartBackup()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnBackupPackage(
    /* [in] */ const String& name)
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnBackupPackage()"));

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

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnBackupPackage()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndBackup()
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnEndBackup()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndBackup", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndBackup %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndBackup %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnEndBackup()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnStartRestore()
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnStartRestore()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStartRestore", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStartRestore %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStartRestore %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnStartRestore()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnRestorePackage(
    /* [in] */ const String& name)
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnRestorePackage()"));

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

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnRestorePackage()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnEndRestore()
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnEndRestore()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onEndRestore", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onEndRestore %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onEndRestore %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnEndRestore()"));
    return NOERROR;
}

ECode CIFullBackupRestoreObserverNative::OnTimeout()
{
    LOGGERD(TAG, String("+ CIFullBackupRestoreObserverNative::OnTimeout()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IFullBackupRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IFullBackupRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onTimeout", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onTimeout %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onTimeout %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIFullBackupRestoreObserverNative::OnTimeout()"));
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

