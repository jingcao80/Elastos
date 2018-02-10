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

#include "elastos/droid/javaproxy/CMediaContainerServiceNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::App::EIID_IIMediaContainerService;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMediaContainerServiceNative::TAG("CMediaContainerServiceNative");

CAR_INTERFACE_IMPL_2(CMediaContainerServiceNative, Object, IIMediaContainerService, IBinder)

CAR_OBJECT_IMPL(CMediaContainerServiceNative)

CMediaContainerServiceNative::~CMediaContainerServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMediaContainerServiceNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMediaContainerServiceNative::CopyPackageToContainer(
    /* [in] */ const String& packagePath,
    /* [in] */ const String& containerId,
    /* [in] */ const String& key,
    /* [in] */ Boolean isExternal,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ String* path)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::CopyPackageToContainer()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);
    jobject jcontainerId = Util::ToJavaString(env, containerId);
    jobject jkey = Util::ToJavaString(env, key);
    jobject jabiOverride = Util::ToJavaString(env, abiOverride);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "copyPackageToContainer", "(Ljava/lang/String;"
        "Ljava/lang/String;Ljava/lang/String;ZZLjava/lang/String;)Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: copyPackageToContainer %d", __LINE__);

    jstring jpkgLite = (jstring)env->CallObjectMethod(mJInstance, m, jpackagePath, jcontainerId, jkey,
        (jboolean)isExternal, (jboolean)isForwardLocked, jabiOverride);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: copyPackageToContainer %d", __LINE__);

    *path = Util::GetElString(env, jpkgLite);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);
    env->DeleteLocalRef(jcontainerId);
    env->DeleteLocalRef(jkey);
    env->DeleteLocalRef(jabiOverride);
    env->DeleteLocalRef(jpkgLite);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::CopyPackageToContainer()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CopyPackage(
    /* [in] */ const String& packagePath,
    /* [in] */ IIParcelFileDescriptorFactory* target,
    /* [out] */ Int32* res)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::CopyPackage()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);

    jobject jtarget = NULL;
    if (target != NULL) {
        LOGGERE(TAG, "TODO:CopyPackage target != NULL!");
    }

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "copyPackage", "(Ljava/lang/String;Lcom/android/internal/os/IParcelFileDescriptorFactory;)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: copyPackage %d", __LINE__);

    *res = env->CallIntMethod(mJInstance, m, jpackagePath, jtarget);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: copyPackage %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);
    env->DeleteLocalRef(jtarget);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::CopyPackage()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::GetMinimalPackageInfo(
    /* [in] */ const String& packagePath,
    /* [in] */ Int32 flags,
    /* [in] */ const String& abiOverride,
    /* [out] */ IPackageInfoLite** pkgLite)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::GetMinimalPackageInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);
    jobject jabiOverride = Util::ToJavaString(env, abiOverride);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getMinimalPackageInfo", "(Ljava/lang/String;ILjava/lang/String;)Landroid/content/pm/PackageInfoLite;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getMinimalPackageInfo %d", __LINE__);

    jobject jpkgLite = env->CallObjectMethod(mJInstance, m, jpackagePath, (jint)flags, jabiOverride);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getMinimalPackageInfo %d", __LINE__);

    if (!Util::GetElPackageInfoLite(env, jpkgLite, pkgLite))
        LOGGERE("GetMinimalPackageInfo", "GetElPackageInfoLite failed");

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);
    env->DeleteLocalRef(jabiOverride);
    env->DeleteLocalRef(jpkgLite);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::GetMinimalPackageInfo()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::GetObbInfo(
    /* [in] */ const String& filename,
    /* [out] */ IObbInfo** obbInfo)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::GetObbInfo()");

    if (obbInfo == NULL) {
        LOGGERE(TAG, "GetObbInfo() Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfilename = Util::ToJavaString(env, filename);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getObbInfo", "(Ljava/lang/String;)Landroid/content/res/ObbInfo;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getObbInfo %d", __LINE__);

    jobject jobbInfo = env->CallObjectMethod(mJInstance, m, jfilename);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: getObbInfo %d", __LINE__);

    if (jobbInfo != NULL) {
        if (!Util::GetElObbInfo(env, jobbInfo, obbInfo)) {
            LOGGERE(TAG, "GetObbInfo() GetElObbInfo fail!");
        }
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfilename);
    env->DeleteLocalRef(jobbInfo);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::GetObbInfo()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CalculateDirectorySize(
    /* [in] */ const String& directory,
    /* [out] */ Int64* size)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::CalculateDirectorySize()");

    if (size == NULL) {
        LOGGERE(TAG, "CalculateDirectorySize() Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdirectory = Util::ToJavaString(env, directory);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "calculateDirectorySize", "(Ljava/lang/String;)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: calculateDirectorySize %d", __LINE__);

    *size = (Int64)env->CallLongMethod(mJInstance, m, jdirectory);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: calculateDirectorySize %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdirectory);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::CalculateDirectorySize()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::GetFileSystemStats(
    /* [in] */ const String& path,
    /* [out, callee] */ ArrayOf<Int64>** stats)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::GetFileSystemStats()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpath = Util::ToJavaString(env, path);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getFileSystemStats", "(Ljava/lang/String;)[J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getFileSystemStats %d", __LINE__);

    jlongArray jstats = (jlongArray)env->CallObjectMethod(mJInstance, m, jpath);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getFileSystemStats %d", __LINE__);
    if (jstats != NULL) {
        if (!Util::GetElInt64Array(env, jstats, stats)) {
            LOGGERE(TAG, "CalculateInstalledSize() GetElInt64Array fail!");
        }
    }

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(jstats);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::GetFileSystemStats()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::ClearDirectory(
    /* [in] */ const String& directory)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::ClearDirectory()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jdirectory = Util::ToJavaString(env, directory);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "clearDirectory", "(Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: ClearDirectory %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jdirectory);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: ClearDirectory %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jdirectory);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::ClearDirectory()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CalculateInstalledSize(
    /* [in] */ const String& packagePath,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int64* size)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::CalculateInstalledSize()");

    if (size == NULL) {
        LOGGERE("CMediaContainerServiceNative", "Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);
    jobject jabiOverride = Util::ToJavaString(env, abiOverride);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "calculateInstalledSize", "(Ljava/lang/String;ZLjava/lang/String;)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: calculateInstalledSize %d", __LINE__);

    *size = (Int64)(jlong)env->CallObjectMethod(mJInstance, m, jpackagePath, (jboolean)isForwardLocked, jabiOverride);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getMinimalPackageInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);
    env->DeleteLocalRef(jabiOverride);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::CalculateInstalledSize()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::ToString()");

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

    // LOGGERD(TAG, "- CMediaContainerServiceNative::ToString()");
    return NOERROR;
}

}
}
}

