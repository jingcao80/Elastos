
#include "elastos/droid/javaproxy/CMediaContainerServiceNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::Pm::IContainerEncryptionParams;
using Elastos::Droid::Content::Res::IObbInfo;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMediaContainerServiceNative::TAG("CMediaContainerServiceNative");

CMediaContainerServiceNative::~CMediaContainerServiceNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMediaContainerServiceNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMediaContainerServiceNative::CopyResourceToContainer(
    /* [in] */ IUri* packageURI,
    /* [in] */ const String& containerId,
    /* [in] */ const String& key,
    /* [in] */ const String& resFileName,
    /* [in] */ const String& publicResFileName,
    /* [in] */ Boolean isExternal,
    /* [in] */ Boolean isForwardLocked,
    /* [out] */ String* path)
{
    LOGGERD(TAG, "+ CMediaContainerServiceNative::CopyResourceToContainer()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackageURI = NULL;
    if (packageURI != NULL) {
        jpackageURI = Util::ToJavaUri(env, packageURI);
    }
    jobject jcontainerId = Util::ToJavaString(env, containerId);
    jobject jkey = Util::ToJavaString(env, key);
    jobject jresFileName = Util::ToJavaString(env, resFileName);
    jobject jpublicResFileName = Util::ToJavaString(env, publicResFileName);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "copyResourceToContainer", "(Landroid/net/Uri;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ)Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: copyResourceToContainer %d", __LINE__);

    jstring jpkgLite = (jstring)env->CallObjectMethod(mJInstance, m, jpackageURI, jcontainerId, jkey, jresFileName, jpublicResFileName,  (jboolean)isExternal, (jboolean)isForwardLocked);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: copyResourceToContainer %d", __LINE__);

    *path = Util::GetElString(env, jpkgLite);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageURI);
    env->DeleteLocalRef(jcontainerId);
    env->DeleteLocalRef(jkey);
    env->DeleteLocalRef(jresFileName);
    env->DeleteLocalRef(jpublicResFileName);
    env->DeleteLocalRef(jpkgLite);

    LOGGERD(TAG, "- CMediaContainerServiceNative::CopyResourceToContainer()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CopyResource(
    /* [in] */ IUri* packageURI,
    /* [in] */ IContainerEncryptionParams* encryptionParams,
    /* [in] */ IParcelFileDescriptor* outStream,
    /* [out] */ Int32* res)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::CopyResource()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackageURI = Util::ToJavaUri(env, packageURI);

    jobject jencryptionParams = NULL;
    if (encryptionParams != NULL) {
        jencryptionParams = Util::ToJavaContainerEncryptionParams(env, encryptionParams);
    }

    jobject joutStream = Util::ToJavaParcelFileDescriptor(env, outStream);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "copyResource", "(Landroid/net/Uri;Landroid/content/pm/ContainerEncryptionParams;Landroid/os/ParcelFileDescriptor;)I");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: copyResource %d", __LINE__);

    *res = env->CallIntMethod(mJInstance, m, jpackageURI, jencryptionParams, joutStream);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: copyResource %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageURI);
    env->DeleteLocalRef(jencryptionParams);
    env->DeleteLocalRef(joutStream);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::CopyResource()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::GetMinimalPackageInfo(
    /* [in] */ const String& packagePath,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 threshold,
    /* [out] */ IPackageInfoLite** pkgLite)
{
    // LOGGERD(TAG, "+ CMediaContainerServiceNative::GetMinimalPackageInfo()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getMinimalPackageInfo", "(Ljava/lang/String;IJ)Landroid/content/pm/PackageInfoLite;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getMinimalPackageInfo %d", __LINE__);

    jobject jpkgLite = env->CallObjectMethod(mJInstance, m, jpackagePath, (jint)flags, (jlong)threshold);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getMinimalPackageInfo %d", __LINE__);

    if (!Util::GetElPackageInfoLite(env, jpkgLite, pkgLite))
        LOGGERE("GetMinimalPackageInfo", "GetElPackageInfoLite failed");

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);
    env->DeleteLocalRef(jpkgLite);

    // LOGGERD(TAG, "- CMediaContainerServiceNative::GetMinimalPackageInfo()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CheckInternalFreeStorage(
    /* [in] */ IUri* fileUri,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ Int64 threshold,
    /* [out] */ Boolean* res)
{
    LOGGERD(TAG, "+ CMediaContainerServiceNative::CheckInternalFreeStorage()");
    if (res == NULL) {
        LOGGERE(TAG, "CheckInternalFreeStorage() Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfileUri = NULL;
    if (fileUri != NULL) {
        jfileUri = Util::ToJavaUri(env, fileUri);
    }

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "checkInternalFreeStorage", "(Landroid/net/Uri;ZJ)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: checkInternalFreeStorage %d", __LINE__);

    *res = (Boolean)env->CallBooleanMethod(mJInstance, m, jfileUri, (jboolean)isForwardLocked, (jlong)threshold);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: checkInternalFreeStorage %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfileUri);

    LOGGERD(TAG, "- CMediaContainerServiceNative::CheckInternalFreeStorage()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CheckExternalFreeStorage(
    /* [in] */ IUri* fileUri,
    /* [in] */ Boolean isForwardLocked,
    /* [out] */ Boolean* res)
{
    LOGGERD(TAG, "+ CMediaContainerServiceNative::CheckExternalFreeStorage()");
    if (res == NULL) {
        LOGGERE(TAG, "CheckExternalFreeStorage() Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jfileUri = NULL;
    if (fileUri != NULL) {
        jfileUri = Util::ToJavaUri(env, fileUri);
    }

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "checkExternalFreeStorage", "(Landroid/net/Uri;Z)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: checkExternalFreeStorage %d", __LINE__);

    *res = (Boolean)env->CallBooleanMethod(mJInstance, m, jfileUri, (jboolean)isForwardLocked);
    Util::CheckErrorAndLog(env, TAG, "CallLongMethod: checkExternalFreeStorage %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jfileUri);

    LOGGERD(TAG, "- CMediaContainerServiceNative::CheckExternalFreeStorage()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::GetObbInfo(
    /* [in] */ const String& filename,
    /* [out] */ IObbInfo** obbInfo)
{
    LOGGERD(TAG, "+ CMediaContainerServiceNative::GetObbInfo()");

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

    LOGGERD(TAG, "- CMediaContainerServiceNative::GetObbInfo()");
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
    LOGGERD(TAG, "+ CMediaContainerServiceNative::ClearDirectory()");

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

    LOGGERD(TAG, "- CMediaContainerServiceNative::ClearDirectory()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::CalculateInstalledSize(
    /* [in] */ const String& packagePath,
    /* [in] */ Boolean isForwardLocked,
    /* [out] */ Int64* size)
{
    LOGGERD(TAG, "+ CMediaContainerServiceNative::CalculateInstalledSize()");

    if (size == NULL) {
        LOGGERE("CMediaContainerServiceNative", "Invalid argumenet!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpackagePath = Util::ToJavaString(env, packagePath);

    jclass c = env->FindClass("com/android/internal/app/IMediaContainerService");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IMediaContainerService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "calculateInstalledSize", "(Ljava/lang/String;Z)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: calculateInstalledSize %d", __LINE__);

    *size = (Int64)(jlong)env->CallObjectMethod(mJInstance, m, jpackagePath, (jboolean)isForwardLocked);
    Util::CheckErrorAndLog(env, TAG, "CallObjectMethod: getMinimalPackageInfo %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackagePath);

    LOGGERD(TAG, "- CMediaContainerServiceNative::CalculateInstalledSize()");
    return NOERROR;
}

ECode CMediaContainerServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CMediaContainerServiceNative::ToString()"));

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

    // LOGGERD(TAG, String("- CMediaContainerServiceNative::ToString()"));
    return NOERROR;
}

}
}
}

