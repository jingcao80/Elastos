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

#include "elastos/droid/javaproxy/CIBackupAgentNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IIBackupAgent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIBackupAgentNative::TAG("CIBackupAgentNative");

CAR_INTERFACE_IMPL_2(CIBackupAgentNative, Object, IIBackupAgent, IBinder)

CAR_OBJECT_IMPL(CIBackupAgentNative)

CIBackupAgentNative::~CIBackupAgentNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIBackupAgentNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
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
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoBackup()");

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

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
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
    // LOGGERD(TAG, "- CIBackupAgentNative::DoBackup()");
    return NOERROR;
}

ECode CIBackupAgentNative::DoRestore(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoRestore()");

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

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
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
    // LOGGERD(TAG, "- CIBackupAgentNative::DoRestore()");
    return NOERROR;
}

ECode CIBackupAgentNative::DoFullBackup(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoFullBackup()");

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

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
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
    // LOGGERD(TAG, "- CIBackupAgentNative::DoFullBackup()");
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
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoRestoreFile()");

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

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
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
    // LOGGERD(TAG, "- CIBackupAgentNative::DoRestoreFile()");
    return NOERROR;
}

ECode CIBackupAgentNative::DoRestoreFinished(
    /* [in] */ Int32 token,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoRestoreFinished()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcallbackBinder = NULL;
    if (callbackBinder != NULL) {
        jclass c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doRestoreFinished", "(ILandroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doRestoreFinished Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)token, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doRestoreFinished Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcallbackBinder);
    // LOGGERD(TAG, "- CIBackupAgentNative::DoRestoreFinished()");
    return NOERROR;
}

ECode CIBackupAgentNative::Fail(
    /* [in] */ const String& message)
{
    // LOGGERD(TAG, "+ CIBackupAgentNative::Fail()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jmessage = NULL;
    if (message != NULL) {
        jmessage = Util::ToJavaString(env, message);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "fail", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: fail Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m,  jmessage);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: fail Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmessage);
    // LOGGERD(TAG, "- CIBackupAgentNative::Fail()");
    return NOERROR;
}

ECode CIBackupAgentNative::DoBackupFiles(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int32 token,
    /* [in] */ ArrayOf<String>* domainTokens,
    /* [in] */ const String& excludeFilesRegex,
    /* [in] */ IIBackupManager* callbackBinder)
{
    // LOGGERD(TAG, "+ CIBackupAgentNative::DoBackupFiles()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdata = NULL;
    if (data != NULL) {
        jdata = Util::ToJavaParcelFileDescriptor(env, data);
    }
    jobjectArray jdomainTokens = NULL;
    if (domainTokens != NULL) {
        jdomainTokens = Util::ToJavaStringArray(env, domainTokens);
    }
    jstring jexcludeFilesRegex = Util::ToJavaString(env, excludeFilesRegex);

    jobject jcallbackBinder = NULL;
    if (callbackBinder != NULL) {
        jclass c = env->FindClass("android/app/backup/ElBackupManagerProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBackupManagerProxy : %d", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBackupManagerProxy : %d", __LINE__);

        jcallbackBinder = env->NewObject(c, m, (jlong)callbackBinder);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBackupManagerProxy : %d", __LINE__);
        callbackBinder->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/app/IBackupAgent");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IBackupAgent %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "doBackupFiles", "(Landroid/os/ParcelFileDescriptor;"
        "I[Ljava/lang/String;Ljava/lang/String;Landroid/app/backup/IBackupManager;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: doBackupFiles Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m,  jdata, token, jdomainTokens, jexcludeFilesRegex, jcallbackBinder);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: doBackupFiles Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jdomainTokens);
    env->DeleteLocalRef(jexcludeFilesRegex);
    env->DeleteLocalRef(jcallbackBinder);
    // LOGGERD(TAG, "- CIBackupAgentNative::DoBackupFiles()");
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

