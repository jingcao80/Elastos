
#include "elastos/droid/javaproxy/CIBackupAgentNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIBackupAgentNative::TAG("CIBackupAgentNative");

CIBackupAgentNative::~CIBackupAgentNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIBackupAgentNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIBackupAgentNative::DoBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // LOGGERD(TAG, String("+ CIBackupAgentNative::DoBackup()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject joldState = NULL;
    if (oldState != NULL) {
        joldState = Util::ToJavaParcelFileDescriptor(env, oldState);
    }

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaParcelFileDescriptor(env, data);
    }

    jobject jnewState = NULL;
    if (newState != NULL) {
        jnewState = Util::ToJavaParcelFileDescriptor(env, newState);
    }

    jobject jcallbackBinder = NULL;
    jclass c = NULL;
    if (callbackBinder != NULL) {
        c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jint)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();

        env->DeleteLocalRef(c);
    }

    c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doBackup", "(Landroid/os/ParcelFileDescriptor;Landroid/os/ParcelFileDescriptor;Landroid/os/ParcelFileDescriptor;ILandroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doBackup Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, joldState, jdata, jnewState, (jint)token, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doBackup Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(joldState);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jnewState);
    env->DeleteLocalRef(jcallbackBinder);
    // LOGGERD(TAG, String("- CIBackupAgentNative::DoBackup()"));
    return NOERROR;
}

ECode CIBackupAgentNative::DoRestore(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    LOGGERD(TAG, String("+ CIBackupAgentNative::DoRestore()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaParcelFileDescriptor(env, data);
    }

    jobject jnewState = NULL;
    if (newState != NULL) {
        jnewState = Util::ToJavaParcelFileDescriptor(env, newState);
    }

    jobject jcallbackBinder = NULL;
    if (callbackBinder != NULL) {
        jclass c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jint)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();
        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doRestore", "(Landroid/os/ParcelFileDescriptor;Landroid/os/ParcelFileDescriptor;ILandroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doRestore Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m,  jdata, (jint)appVersionCode, jnewState, (jint)token, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doRestore Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jnewState);
    env->DeleteLocalRef(jcallbackBinder);
    LOGGERD(TAG, String("- CIBackupAgentNative::DoRestore()"));
    return NOERROR;
}

ECode CIBackupAgentNative::DoFullBackup(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    LOGGERD(TAG, String("+ CIBackupAgentNative::DoFullBackup()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaParcelFileDescriptor(env, data);
    }

    jobject jcallbackBinder = NULL;
    if (callbackBinder != NULL) {
        jclass c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jint)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doFullBackup", "(Landroid/os/ParcelFileDescriptor;ILandroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doFullBackup Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m,  jdata, (jint)token, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doFullBackup Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jcallbackBinder);
    LOGGERD(TAG, String("- CIBackupAgentNative::DoFullBackup()"));
    return NOERROR;
}

ECode CIBackupAgentNative::DoRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ const String& domainName,
    /* [in] */ const String& path,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
   LOGGERD(TAG, String("+ CIBackupAgentNative::DoRestoreFile()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaParcelFileDescriptor(env, data);
    }

    jstring jdomainName = Util::ToJavaString(env, domainName);
    jstring jpath = Util::ToJavaString(env, path);

    jobject jcallbackBinder = NULL;
    if (callbackBinder != NULL) {
        jclass c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(I)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jint)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doRestoreFile", "(Landroid/os/ParcelFileDescriptor;JILjava/lang/String;Ljava/lang/String;JJILandroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doRestoreFile Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m,  jdata, (jlong)size, (jint)type, jdomainName, jpath, (jlong)mode, (jlong)mtime, (jint)token, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doRestoreFile Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jdomainName);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(jcallbackBinder);
    LOGGERD(TAG, String("- CIBackupAgentNative::DoRestoreFile()"));
    return NOERROR;
}

ECode CIBackupAgentNative::ToString(
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

