
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/content/CNativeLibraryHelper.h"
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SELinux.h"
#include <elastos/droid/system/Os.h>
#include <elastos/droid/system/OsConstants.h>
#include <elastos/utility/logging/Slogger.h>

#include <androidfw/ZipFileRO.h>
#include <androidfw/ZipUtils.h>
#include <utils/UniquePtr.h>
#include <utils/Log.h>
#include <zlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define APK_LIB "lib/"
#define APK_LIB_LEN (sizeof(APK_LIB) - 1)

#define LIB_PREFIX "/lib"
#define LIB_PREFIX_LEN (sizeof(LIB_PREFIX) - 1)

#define LIB_SUFFIX ".so"
#define LIB_SUFFIX_LEN (sizeof(LIB_SUFFIX) - 1)

#define EL_LIB_SUFFIX ".eco"
#define EL_LIB_SUFFIX_LEN (sizeof(EL_LIB_SUFFIX) - 1)

#define RS_BITCODE_SUFFIX ".bc"
#define RS_BITCODE_SUFFIX_LEN (sizeof(RS_BITCODE_SUFFIX) -1)

#define GDBSERVER "gdbserver"
#define GDBSERVER_LEN (sizeof(GDBSERVER) - 1)

#define TMP_FILE_PATTERN "/tmp.XXXXXX"
#define TMP_FILE_PATTERN_LEN (sizeof(TMP_FILE_PATTERN) - 1)

using namespace android;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::System::OsConstants;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

// These match PackageManager.java install codes
enum install_status_t {
    INSTALL_SUCCEEDED = 1,
    INSTALL_FAILED_INVALID_APK = -2,
    INSTALL_FAILED_INSUFFICIENT_STORAGE = -4,
    INSTALL_FAILED_CONTAINER_ERROR = -18,
    INSTALL_FAILED_INTERNAL_ERROR = -110,
    INSTALL_FAILED_NO_MATCHING_ABIS = -113,
    NO_NATIVE_LIBRARIES = -114
};

enum bitcode_scan_result_t {
  APK_SCAN_ERROR = -1,
  NO_BITCODE_PRESENT = 0,
  BITCODE_PRESENT = 1,
};

typedef install_status_t (*iterFunc)(void*, ZipFileRO*, ZipEntryRO, const char*);
static bool isFilenameSafe(const char* filename);
static bool isFileDifferent(const char* filePath, size_t fileSize, time_t modifiedTime, long zipCrc, struct stat64* st);
static install_status_t sumFiles(void* arg, ZipFileRO* zipFile, ZipEntryRO zipEntry, const char* fileName);
static install_status_t copyFileIfChanged(void* arg, ZipFileRO* zipFile, ZipEntryRO zipEntry, const char* fileName);
static install_status_t iterateOverNativeFiles(Int64 apkHandle, const String& javaCpuAbi, iterFunc callFunc, void* callArg);
static int findSupportedAbi(Int64 apkHandle, ArrayOf<String>* supportedAbis);

const String CNativeLibraryHelper::TAG("CNativeLibraryHelper");
const Boolean CNativeLibraryHelper::DEBUG_NATIVE = FALSE;
const Int32 CNativeLibraryHelper::BITCODE_PRESENT = 1;

CAR_INTERFACE_IMPL(CNativeLibraryHelper, Singleton, INativeLibraryHelper)
CAR_SINGLETON_IMPL(CNativeLibraryHelper)

Int64 CNativeLibraryHelper::SumNativeBinaries(
    /* [in] */ INativeLibraryHelperHandle* _handle,
    /* [in] */ const String& abi)
{
    CNativeLibraryHelperHandle* handle = (CNativeLibraryHelperHandle*)_handle;
    Int64 sum = 0;
    for (Int32 i = 0; i < handle->mApkHandles->GetLength(); i++) {
        Int64 apkHandle = (*handle->mApkHandles)[i];
        sum += NativeSumNativeBinaries(apkHandle, abi);
    }
    return sum;
}

/**
 * Copies native binaries to a shared library directory.
 *
 * @param handle APK file to scan for native libraries
 * @param sharedLibraryDir directory for libraries to be copied to
 * @return {@link PackageManager#INSTALL_SUCCEEDED} if successful or another
 *         error code from that class if not
 */
ECode CNativeLibraryHelper::CopyNativeBinaries(
    /* [in] */ INativeLibraryHelperHandle* _handle,
    /* [in] */ IFile* sharedLibraryDir,
    /* [in] */ const String& abi,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    CNativeLibraryHelperHandle* handle = (CNativeLibraryHelperHandle*)_handle;
    for (Int32 i = 0; i < handle->mApkHandles->GetLength(); i++) {
        Int64 apkHandle = (*handle->mApkHandles)[i];
        String path;
        sharedLibraryDir->GetPath(&path);
        Int32 res = NativeCopyNativeBinaries(apkHandle, path, abi);
        if (res != IPackageManager::INSTALL_SUCCEEDED) {
            *result = res;
            return NOERROR;
        }
    }
    *result = IPackageManager::INSTALL_SUCCEEDED;
    return NOERROR;
}

/**
 * Checks if a given APK contains native code for any of the provided
 * {@code supportedAbis}. Returns an index into {@code supportedAbis} if a matching
 * ABI is found, {@link PackageManager#NO_NATIVE_LIBRARIES} if the
 * APK doesn't contain any native code, and
 * {@link PackageManager#INSTALL_FAILED_NO_MATCHING_ABIS} if none of the ABIs match.
 */
ECode CNativeLibraryHelper::FindSupportedAbi(
    /* [in] */ INativeLibraryHelperHandle* _handle,
    /* [in] */ ArrayOf<String>* supportedAbis,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    CNativeLibraryHelperHandle* handle = (CNativeLibraryHelperHandle*)_handle;
    Int32 finalRes = IPackageManager::NO_NATIVE_LIBRARIES;
    for (Int32 i = 0; i < handle->mApkHandles->GetLength(); i++) {
        Int64 apkHandle = (*handle->mApkHandles)[i];
        Int32 res = NativeFindSupportedAbi(apkHandle, supportedAbis);
        if (res == IPackageManager::NO_NATIVE_LIBRARIES) {
            // No native code, keep looking through all APKs.
        }
        else if (res == IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS) {
            // Found some native code, but no ABI match; update our final
            // result if we haven't found other valid code.
            if (finalRes < 0) {
                finalRes = IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS;
            }
        }
        else if (res >= 0) {
            // Found valid native code, track the best ABI match
            if (finalRes < 0 || res < finalRes) {
                finalRes = res;
            }
        }
        else {
            // Unexpected error; bail
            *result = res;
            return NOERROR;
        }
    }
    *result = finalRes;
    return NOERROR;
}

// Convenience method to call removeNativeBinariesFromDirLI(File)
ECode CNativeLibraryHelper::RemoveNativeBinariesLI(
    /* [in] */ const String& nativeLibraryPath)
{
    if (nativeLibraryPath.IsNull())
        return NOERROR;
    AutoPtr<IFile> file;
    CFile::New(nativeLibraryPath, (IFile**)&file);
    return RemoveNativeBinariesFromDirLI(file, FALSE /* delete root dir */);
}

/**
 * Remove the native binaries of a given package. This deletes the files
 */
ECode CNativeLibraryHelper::RemoveNativeBinariesFromDirLI(
    /* [in] */ IFile* nativeLibraryRoot,
    /* [in] */ Boolean deleteRootDir)
{
    if (DEBUG_NATIVE) {
        String path;
        nativeLibraryRoot->GetPath(&path);
        Slogger::W(TAG, "Deleting native binaries from: %s", path.string());
    }

    /*
     * Just remove any file in the directory. Since the directory is owned
     * by the 'system' UID, the application is not supposed to have written
     * anything there.
     */
    Boolean exists = FALSE;
    nativeLibraryRoot->Exists(&exists);
    if (exists) {
        AutoPtr<ArrayOf<IFile*> > files;
        nativeLibraryRoot->ListFiles((ArrayOf<IFile*>**)&files);
        if (files != NULL) {
            for (Int32 nn = 0; nn < files->GetLength(); nn++) {
                if (DEBUG_NATIVE) {
                    String name;
                    (*files)[nn]->GetName(&name);
                    Slogger::D(TAG, "    Deleting %s", name.string());
                }

                Boolean isDirectory, isDeleted;
                (*files)[nn]->IsDirectory(&isDirectory);
                if (isDirectory) {
                    RemoveNativeBinariesFromDirLI((*files)[nn], TRUE /* delete root dir */);
                }
                else if (!((*files)[nn]->Delete(&isDeleted), isDeleted)) {
                    String path;
                    (*files)[nn]->GetPath(&path);
                    Slogger::W(TAG, "Could not delete native binary: %s", path.string());
                }
            }
        }
        // Do not delete 'lib' directory itself, unless we're specifically
        // asked to or this will prevent installation of future updates.
        if (deleteRootDir) {
            Boolean isDeleted;
            nativeLibraryRoot->Delete(&isDeleted);
            if (!isDeleted) {
                String path;
                nativeLibraryRoot->GetPath(&path);
                Slogger::W(TAG, "Could not delete native binary directory: %s", path.string());
            }
        }
    }
    return NOERROR;
}

ECode CNativeLibraryHelper::CreateNativeLibrarySubdir(
    /* [in] */ IFile* path)
{
    Boolean isDirectory;
    path->IsDirectory(&isDirectory);
    if (!isDirectory) {
        Boolean res;
        path->Delete(&res);

        String strPath;
        path->GetPath(&strPath);
        path->Mkdir(&res);
        if (!res) {
            // throw new IOException("Cannot create " + path.getPath());
            Slogger::E(TAG, "Cannot create %s", strPath.string());
            return E_IO_EXCEPTION;
        }

        if (Elastos::Droid::System::Os::Chmod(strPath, OsConstants::_S_IRWXU | OsConstants::_S_IRGRP | OsConstants::_S_IXGRP
            | OsConstants::_S_IROTH | OsConstants::_S_IXOTH) == (ECode)E_ERRNO_EXCEPTION) {
            Slogger::E(TAG, "Cannot chmod native library directory %s", strPath.string());
            return E_IO_EXCEPTION;
        }
    }
    else if (!SELinux::Restorecon(path)) {
        String strPath;
        path->GetPath(&strPath);
        Slogger::E(TAG, "Cannot et SELinux context for %s", strPath.string());
        return E_IO_EXCEPTION;
    }

    return NOERROR;
}

Int64 CNativeLibraryHelper::SumNativeBinariesForSupportedAbi(
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ ArrayOf<String>* abiList)
{
    Int32 abi;
    FindSupportedAbi(handle, abiList, &abi);
    if (abi >= 0) {
        return SumNativeBinaries(handle, (*abiList)[abi]);
    }
    else {
        return 0;
    }
}

ECode CNativeLibraryHelper::CopyNativeBinariesForSupportedAbi(
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ IFile* libraryRoot,
    /* [in] */ ArrayOf<String>* abiList,
    /* [in] */ Boolean useIsaSubdir,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    FAIL_RETURN(CreateNativeLibrarySubdir(libraryRoot));

    /*
     * If this is an internal application or our nativeLibraryPath points to
     * the app-lib directory, unpack the libraries if necessary.
     */
    Int32 abi;
    FindSupportedAbi(handle, abiList, &abi);
    if (abi >= 0) {
        /*
         * If we have a matching instruction set, construct a subdir under the native
         * library root that corresponds to this instruction set.
         */
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        String instructionSet;
        system->GetInstructionSet((*abiList)[abi], &instructionSet);
        if ((*abiList)[abi].Equals("armeabi"))
            instructionSet = "arm";
        else if ((*abiList)[abi].Equals("armeabi-v7a"))
            instructionSet = "arm";
        else if ((*abiList)[abi].Equals("mips"))
            instructionSet = "mips";
        else if ((*abiList)[abi].Equals("mips64"))
            instructionSet = "mips64";
        else if ((*abiList)[abi].Equals("x86"))
            instructionSet = "x86";
        else if ((*abiList)[abi].Equals("x86_64"))
            instructionSet = "x86_64";
        else if ((*abiList)[abi].Equals("arm64-v8a"))
            instructionSet = "arm64";
        else {
            Slogger::E(TAG, "Unsupported ABI: %d", abi);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IFile> subDir;
        if (useIsaSubdir) {
            AutoPtr<IFile> isaSubdir;
            CFile::New(libraryRoot, instructionSet, (IFile**)&isaSubdir);
            FAIL_RETURN(CreateNativeLibrarySubdir(isaSubdir));
            subDir = isaSubdir;
        }
        else {
            subDir = libraryRoot;
        }

        Int32 copyRet;
        FAIL_RETURN(CopyNativeBinaries(handle, subDir, (*abiList)[abi], &copyRet));
        if (copyRet != IPackageManager::INSTALL_SUCCEEDED) {
            *result = copyRet;
            return NOERROR;
        }
    }

    *result = abi;
    return NOERROR;
}

ECode CNativeLibraryHelper::CopyNativeBinariesWithOverride(
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ IFile* libraryRoot,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    do {
        ECode ec = NOERROR;
        if (((CNativeLibraryHelperHandle*)handle)->mMultiArch) {
            // Warn if we've set an abiOverride for multi-lib packages..
            // By definition, we need to copy both 32 and 64 bit libraries for
            // such packages.
            if (abiOverride != NULL && !CLEAR_ABI_OVERRIDE.Equals(abiOverride)) {
                Slogger::W(TAG, "Ignoring abiOverride for multi arch application.");
            }

            Int32 copyRet = IPackageManager::NO_NATIVE_LIBRARIES;
            if (Build::SUPPORTED_32_BIT_ABIS->GetLength() > 0) {
                ec = CopyNativeBinariesForSupportedAbi(handle, libraryRoot,
                        Build::SUPPORTED_32_BIT_ABIS, TRUE /* use isa specific subdirs */, &copyRet);
                if (ec == (ECode)E_IO_EXCEPTION)
                    break;

                if (copyRet < 0 && copyRet != IPackageManager::NO_NATIVE_LIBRARIES &&
                        copyRet != IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS) {
                    Slogger::W(TAG, "Failure copying 32 bit native libraries; copyRet=%d", copyRet);
                    *result = copyRet;
                    return NOERROR;
                }
            }

            if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0) {
                ec = CopyNativeBinariesForSupportedAbi(handle, libraryRoot,
                        Build::SUPPORTED_64_BIT_ABIS, TRUE /* use isa specific subdirs */, &copyRet);
                if (ec == (ECode)E_IO_EXCEPTION)
                    break;

                if (copyRet < 0 && copyRet != IPackageManager::NO_NATIVE_LIBRARIES &&
                        copyRet != IPackageManager::INSTALL_FAILED_NO_MATCHING_ABIS) {
                    Slogger::W(TAG, "Failure copying 64 bit native libraries; copyRet=%d", copyRet);
                    *result = copyRet;
                    return NOERROR;
                }
            }
        }
        else {
            String cpuAbiOverride;
            if (CLEAR_ABI_OVERRIDE.Equals(abiOverride)) {
                cpuAbiOverride = NULL;
            }
            else if (abiOverride != NULL) {
                cpuAbiOverride = abiOverride;
            }

            AutoPtr<ArrayOf<String> > abiList;
            if (cpuAbiOverride != NULL) {
                abiList = ArrayOf<String>::Alloc(1);
                (*abiList)[0] = cpuAbiOverride;
            }
            else
                abiList = Build::SUPPORTED_ABIS;

            if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0 && cpuAbiOverride == NULL) {
                Boolean res;
                ec = HasRenderscriptBitcode(handle, &res);
                if (ec == (ECode)E_IO_EXCEPTION)
                    break;
                if (res)
                    abiList = Build::SUPPORTED_32_BIT_ABIS;
            }

            Int32 copyRet;
            ec = CopyNativeBinariesForSupportedAbi(handle, libraryRoot, abiList,
                    TRUE /* use isa specific subdirs */, &copyRet);
            if (ec == (ECode)E_IO_EXCEPTION)
                break;

            if (copyRet < 0 && copyRet != IPackageManager::NO_NATIVE_LIBRARIES) {
                Slogger::W(TAG, "Failure copying native libraries [errorCode=%d]", copyRet);
                *result = copyRet;
                return NOERROR;
            }
        }

        *result = IPackageManager::INSTALL_SUCCEEDED;
        return NOERROR;
    } while (0);

    Slogger::E(TAG, "Copying native libraries failed");
    *result = IPackageManager::INSTALL_FAILED_INTERNAL_ERROR;
    return NOERROR;
}

ECode CNativeLibraryHelper::SumNativeBinariesWithOverride(
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int64* sum)
{
    VALIDATE_NOT_NULL(sum)
    *sum = 0;
    if (((CNativeLibraryHelperHandle*)handle)->mMultiArch) {
        // Warn if we've set an abiOverride for multi-lib packages..
        // By definition, we need to copy both 32 and 64 bit libraries for
        // such packages.
        if (abiOverride != NULL && !CLEAR_ABI_OVERRIDE.Equals(abiOverride)) {
            Slogger::W(TAG, "Ignoring abiOverride for multi arch application.");
        }

        if (Build::SUPPORTED_32_BIT_ABIS->GetLength() > 0) {
            *sum += SumNativeBinariesForSupportedAbi(handle, Build::SUPPORTED_32_BIT_ABIS);
        }

        if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0) {
            *sum += SumNativeBinariesForSupportedAbi(handle, Build::SUPPORTED_64_BIT_ABIS);
        }
    }
    else {
        String cpuAbiOverride;
        if (CLEAR_ABI_OVERRIDE.Equals(abiOverride)) {
            cpuAbiOverride = NULL;
        }
        else if (abiOverride != NULL) {
            cpuAbiOverride = abiOverride;
        }

        AutoPtr<ArrayOf<String> > abiList;
        if (cpuAbiOverride != NULL) {
            abiList = ArrayOf<String>::Alloc(1);
            (*abiList)[0] = cpuAbiOverride;
        }
        else
            abiList = Build::SUPPORTED_ABIS;

        if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0 && cpuAbiOverride == NULL) {
            Boolean res;
            FAIL_RETURN(HasRenderscriptBitcode(handle, &res));
            if (res)
                abiList = Build::SUPPORTED_32_BIT_ABIS;
        }

        *sum += SumNativeBinariesForSupportedAbi(handle, abiList);
    }
    return NOERROR;
}

ECode CNativeLibraryHelper::HasRenderscriptBitcode(
    /* [in] */ INativeLibraryHelperHandle* _handle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    CNativeLibraryHelperHandle* handle = (CNativeLibraryHelperHandle*)_handle;
    for (Int32 i = 0; i < handle->mApkHandles->GetLength(); i++) {
        Int64 apkHandle = (*handle->mApkHandles)[i];
        Int32 res = HasRenderscriptBitcode(apkHandle);
        if (res < 0) {
            Slogger::E(TAG, "Error scanning APK, code: %d", res);
            return E_IO_EXCEPTION;
        }
        else if (res == BITCODE_PRESENT) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

// for epk
ECode CNativeLibraryHelper::CopyEcoLI(
    /* [in] */ const String& epkPath,
    /* [in] */ const String& ecoName,
    /* [in] */ const String& ecoDir,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    ZipFileRO* zipFile = ZipFileRO::open(epkPath.string());;
    if (zipFile == NULL) {
        Slogger::E(TAG, "Couldn't open EPK %s, target ecoName:%s, target dir:%s\n",
            epkPath.string(), ecoName.string(), ecoDir.string());
        *result = INSTALL_FAILED_INVALID_APK;
        return NOERROR;
    }

    const ZipEntryRO entry = zipFile->findEntryByName(ecoName.string());
    if (entry == NULL) {
        Slogger::E(TAG, "%s isn't found in %s", ecoName.string(), epkPath.string());
        *result = INSTALL_FAILED_INVALID_APK;
        delete zipFile;
        return NOERROR;
    }

    install_status_t ret = copyFileIfChanged(
        const_cast<char*>(ecoDir.string()), zipFile, entry, ecoName.string());

    if (ret != INSTALL_SUCCEEDED) {
        Slogger::E(TAG, "Failure to copy from %s to %s", epkPath.string(), ecoName.string());
    }
    *result = ret;
    delete zipFile;

    return NOERROR;
}

Int64 CNativeLibraryHelper::NativeOpenApk(
        /* [in] */ const String& path)
{
    ZipFileRO* zipFile = ZipFileRO::open(path.string());
    return reinterpret_cast<Int64>(zipFile);
}

void CNativeLibraryHelper::NativeClose(
    /* [in] */ Int64 handle)
{
    delete reinterpret_cast<ZipFileRO*>(handle);
}

Int64 CNativeLibraryHelper::NativeSumNativeBinaries(
    /* [in] */ Int64 handle,
    /* [in] */ const String& cpuAbi)
{
    size_t totalSize = 0;

    iterateOverNativeFiles(handle, cpuAbi, sumFiles, &totalSize);

    return (Int64)totalSize;
}

Int32 CNativeLibraryHelper::NativeCopyNativeBinaries(
    /* [in] */ Int64 handle,
    /* [in] */ const String& sharedLibraryPath,
    /* [in] */ const String& abiToCopy)
{
    return (Int32)iterateOverNativeFiles(handle, abiToCopy,
        copyFileIfChanged, const_cast<char*>(sharedLibraryPath.string()));
}

Int32 CNativeLibraryHelper::NativeFindSupportedAbi(
    /* [in] */ Int64 handle,
    /* [in] */ ArrayOf<String>* supportedAbis)
{
    return findSupportedAbi(handle, supportedAbis);
}

Int32 CNativeLibraryHelper::HasRenderscriptBitcode(
    /* [in] */ Int64 apkHandle)
{
    ZipFileRO* zipFile = reinterpret_cast<ZipFileRO*>(apkHandle);
    void* cookie = NULL;
    if (!zipFile->startIteration(&cookie)) {
        return APK_SCAN_ERROR;
    }

    char fileName[PATH_MAX];
    ZipEntryRO next = NULL;
    while ((next = zipFile->nextEntry(cookie)) != NULL) {
        if (zipFile->getEntryFileName(next, fileName, sizeof(fileName))) {
            continue;
        }

        const size_t fileNameLen = strlen(fileName);
        const char* lastSlash = strrchr(fileName, '/');
        const char* baseName = (lastSlash == NULL) ? fileName : fileName + 1;
        if (!strncmp(fileName + fileNameLen - RS_BITCODE_SUFFIX_LEN, RS_BITCODE_SUFFIX,
                     RS_BITCODE_SUFFIX_LEN) && isFilenameSafe(baseName)) {
            zipFile->endIteration(cookie);
            return BITCODE_PRESENT;
        }
    }

    zipFile->endIteration(cookie);
    return NO_BITCODE_PRESENT;
}

// Equivalent to android.os.FileUtils.isFilenameSafe
static bool isFilenameSafe(const char* filename)
{
    off_t offset = 0;
    for (;;) {
        switch (*(filename + offset)) {
        case 0:
            // Null.
            // If we've reached the end, all the other characters are good.
            return true;

        case 'A' ... 'Z':
        case 'a' ... 'z':
        case '0' ... '9':
        case '+':
        case ',':
        case '-':
        case '.':
        case '/':
        case '=':
        case '_':
            offset++;
            break;

        default:
            // We found something that is not good.
            return false;
        }
    }
    // Should not reach here.
}

static bool isFileDifferent(const char* filePath, size_t fileSize, time_t modifiedTime,
        long zipCrc, struct stat64* st)
{
    if (lstat64(filePath, st) < 0) {
        // File is not found or cannot be read.
        ALOGV("Couldn't stat %s, copying: %s\n", filePath, strerror(errno));
        return true;
    }

    if (!S_ISREG(st->st_mode)) {
        return true;
    }

    if (st->st_size != fileSize) {
        return true;
    }

    // For some reason, bionic doesn't define st_mtime as time_t
    if (time_t(st->st_mtime) != modifiedTime) {
        ALOGV("mod time doesn't match: %ld vs. %ld\n", st->st_mtime, modifiedTime);
        return true;
    }

    int fd = TEMP_FAILURE_RETRY(open(filePath, O_RDONLY));
    if (fd < 0) {
        ALOGV("Couldn't open file %s: %s", filePath, strerror(errno));
        return true;
    }

    long crc = crc32(0L, Z_NULL, 0);
    unsigned char crcBuffer[16384];
    ssize_t numBytes;
    while ((numBytes = TEMP_FAILURE_RETRY(read(fd, crcBuffer, sizeof(crcBuffer)))) > 0) {
        crc = crc32(crc, crcBuffer, numBytes);
    }
    close(fd);

    // ALOGV("%s: crc = %lx, zipCrc = %lx\n", filePath, crc, zipCrc);

    if (crc != zipCrc) {
        return true;
    }

    return false;
}

static install_status_t sumFiles(void* arg, ZipFileRO* zipFile, ZipEntryRO zipEntry, const char* fileName)
{
    size_t* total = (size_t*) arg;
    size_t uncompLen;

    if (!zipFile->getEntryInfo(zipEntry, NULL, &uncompLen, NULL, NULL, NULL, NULL)) {
        return INSTALL_FAILED_INVALID_APK;
    }

    *total += uncompLen;

    return INSTALL_SUCCEEDED;
}

/*
 * Copy the native library if needed.
 *
 * This function assumes the library and path names passed in are considered safe.
 */
static install_status_t copyFileIfChanged(void* arg, ZipFileRO* zipFile, ZipEntryRO zipEntry, const char* fileName)
{
    // jstring* javaNativeLibPath = (jstring*) arg;
    // ScopedUtfChars nativeLibPath(env, *javaNativeLibPath);
    String nativeLibPath((char*) arg);

    size_t uncompLen;
    long when;
    long crc;
    time_t modTime;

    if (!zipFile->getEntryInfo(zipEntry, NULL, &uncompLen, NULL, NULL, &when, &crc)) {
        ALOGD("Couldn't read zip entry info\n");
        return INSTALL_FAILED_INVALID_APK;
    } else {
        struct tm t;
        //memset(&t, 0, sizeof(t));
        ZipUtils::zipTimeToTimespec(when, &t);
        modTime = mktime(&t);
    }

    // Build local file path
    const size_t fileNameLen = strlen(fileName);
    char localFileName[nativeLibPath.GetByteLength() + fileNameLen + 2];

    if (strlcpy(localFileName, nativeLibPath.string(), sizeof(localFileName)) != (size_t)nativeLibPath.GetByteLength()) {
        ALOGD("Couldn't allocate local file name for library");
        return INSTALL_FAILED_INTERNAL_ERROR;
    }

    *(localFileName + nativeLibPath.GetByteLength()) = '/';

    if (strlcpy(localFileName + nativeLibPath.GetByteLength() + 1, fileName, sizeof(localFileName)
                    - nativeLibPath.GetByteLength() - 1) != fileNameLen) {
        ALOGD("Couldn't allocate local file name for library");
        return INSTALL_FAILED_INTERNAL_ERROR;
    }

    // Only copy out the native file if it's different.
    struct stat64 st;
    if (!isFileDifferent(localFileName, uncompLen, modTime, crc, &st)) {
        return INSTALL_SUCCEEDED;
    }

    char localTmpFileName[nativeLibPath.GetByteLength() + TMP_FILE_PATTERN_LEN + 2];
    if (strlcpy(localTmpFileName, nativeLibPath.string(), sizeof(localTmpFileName))
            != (size_t)nativeLibPath.GetByteLength()) {
        ALOGD("Couldn't allocate local file name for library");
        return INSTALL_FAILED_INTERNAL_ERROR;
    }

    *(localFileName + nativeLibPath.GetByteLength()) = '/';

    if (strlcpy(localTmpFileName + nativeLibPath.GetByteLength(), TMP_FILE_PATTERN,
                    TMP_FILE_PATTERN_LEN - nativeLibPath.GetByteLength()) != TMP_FILE_PATTERN_LEN) {
        ALOGI("Couldn't allocate temporary file name for library");
        return INSTALL_FAILED_INTERNAL_ERROR;
    }

    int fd = mkstemp(localTmpFileName);
    if (fd < 0) {
        ALOGI("Couldn't open temporary file name: %s: %s\n", localTmpFileName, strerror(errno));
        return INSTALL_FAILED_CONTAINER_ERROR;
    }

    if (!zipFile->uncompressEntry(zipEntry, fd)) {
        ALOGI("Failed uncompressing %s to %s\n", fileName, localTmpFileName);
        close(fd);
        unlink(localTmpFileName);
        return INSTALL_FAILED_CONTAINER_ERROR;
    }

    close(fd);

    // Set the modification time for this file to the ZIP's mod time.
    struct timeval times[2];
    times[0].tv_sec = st.st_atime;
    times[1].tv_sec = modTime;
    times[0].tv_usec = times[1].tv_usec = 0;
    if (utimes(localTmpFileName, times) < 0) {
        ALOGI("Couldn't change modification time on %s: %s\n", localTmpFileName, strerror(errno));
        unlink(localTmpFileName);
        return INSTALL_FAILED_CONTAINER_ERROR;
    }

    // Set the mode to 755
    static const mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH;
    if (chmod(localTmpFileName, mode) < 0) {
        ALOGI("Couldn't change permissions on %s: %s\n", localTmpFileName, strerror(errno));
        unlink(localTmpFileName);
        return INSTALL_FAILED_CONTAINER_ERROR;
    }

    // Finally, rename it to the final name.
    if (rename(localTmpFileName, localFileName) < 0) {
        ALOGI("Couldn't rename %s to %s: %s\n", localTmpFileName, localFileName, strerror(errno));
        unlink(localTmpFileName);
        return INSTALL_FAILED_CONTAINER_ERROR;
    }

    ALOGV("Successfully moved %s to %s\n", localTmpFileName, localFileName);

    return INSTALL_SUCCEEDED;
}


/*
 * An iterator over all shared libraries in a zip file. An entry is
 * considered to be a shared library if all of the conditions below are
 * satisfied :
 *
 * - The entry is under the lib/ directory.
 * - The entry name ends with ".so" and the entry name starts with "lib",
 *   an exception is made for entries whose name is "gdbserver".
 * - The entry filename is "safe" (as determined by isFilenameSafe).
 *
 */
class NativeLibrariesIterator {
private:
    NativeLibrariesIterator(ZipFileRO* zipFile, void* cookie)
        : mZipFile(zipFile), mCookie(cookie), mLastSlash(NULL) {
        fileName[0] = '\0';
    }

public:
    static NativeLibrariesIterator* create(ZipFileRO* zipFile) {
        void* cookie = NULL;
        if (!zipFile->startIteration(&cookie)) {
            return NULL;
        }

        return new NativeLibrariesIterator(zipFile, cookie);
    }

    ZipEntryRO next() {
        ZipEntryRO next = NULL;
        while ((next = mZipFile->nextEntry(mCookie)) != NULL) {
            // Make sure this entry has a filename.
            if (mZipFile->getEntryFileName(next, fileName, sizeof(fileName))) {
                continue;
            }

            // Make sure we're in the lib directory of the ZIP.
            if (strncmp(fileName, APK_LIB, APK_LIB_LEN)) {
                continue;
            }

            // Make sure the filename is at least to the minimum library name size.
            const size_t fileNameLen = strlen(fileName);
            static const size_t minLength = APK_LIB_LEN + 2 + LIB_PREFIX_LEN + 1 + LIB_SUFFIX_LEN;
            if (fileNameLen < minLength) {
                continue;
            }

            const char* lastSlash = strrchr(fileName, '/');
            ALOG_ASSERT(lastSlash != NULL, "last slash was null somehow for %s\n", fileName);

            // Exception: If we find the gdbserver binary, return it.
            if (!strncmp(lastSlash + 1, GDBSERVER, GDBSERVER_LEN)) {
                mLastSlash = lastSlash;
                break;
            }

            // Make sure the filename starts with lib and ends with ".so" or ends with ".eco".
            if ((strncmp(fileName + fileNameLen - LIB_SUFFIX_LEN, LIB_SUFFIX, LIB_SUFFIX_LEN)
                || strncmp(lastSlash, LIB_PREFIX, LIB_PREFIX_LEN))
                && strncmp(fileName + fileNameLen - EL_LIB_SUFFIX_LEN, EL_LIB_SUFFIX, EL_LIB_SUFFIX_LEN)) {
                continue;
            }

            // Make sure the filename is safe.
            if (!isFilenameSafe(lastSlash + 1)) {
                continue;
            }

            mLastSlash = lastSlash;
            break;
        }

        return next;
    }

    inline const char* currentEntry() const {
        return fileName;
    }

    inline const char* lastSlash() const {
        return mLastSlash;
    }

    virtual ~NativeLibrariesIterator() {
        mZipFile->endIteration(mCookie);
    }
private:

    char fileName[PATH_MAX];
    ZipFileRO* const mZipFile;
    void* mCookie;
    const char* mLastSlash;
};

static install_status_t iterateOverNativeFiles(Int64 apkHandle, const String& cpuAbi, iterFunc callFunc, void* callArg)
{
    ZipFileRO* zipFile = reinterpret_cast<ZipFileRO*>(apkHandle);
    if (zipFile == NULL) {
        return INSTALL_FAILED_INVALID_APK;
    }

    UniquePtr<NativeLibrariesIterator> it(NativeLibrariesIterator::create(zipFile));
    if (it.get() == NULL) {
        return INSTALL_FAILED_INVALID_APK;
    }

    // const ScopedUtfChars cpuAbi(env, javaCpuAbi);
    if (cpuAbi == NULL) {
        // This would've thrown, so this return code isn't observable by
        // Java.
        return INSTALL_FAILED_INVALID_APK;
    }
    ZipEntryRO entry = NULL;
    while ((entry = it->next()) != NULL) {
        const char* fileName = it->currentEntry();
        const char* lastSlash = it->lastSlash();

        // Check to make sure the CPU ABI of this file is one we support.
        const char* cpuAbiOffset = fileName + APK_LIB_LEN;
        const size_t cpuAbiRegionSize = lastSlash - cpuAbiOffset;

        if ((size_t)cpuAbi.GetByteLength() == cpuAbiRegionSize && !strncmp(cpuAbiOffset, cpuAbi.string(), cpuAbiRegionSize)) {
            install_status_t ret = callFunc(callArg, zipFile, entry, lastSlash + 1);

            if (ret != INSTALL_SUCCEEDED) {
                ALOGV("Failure for entry %s", lastSlash + 1);
                return ret;
            }
        }
    }

    return INSTALL_SUCCEEDED;
}

static int findSupportedAbi(Int64 apkHandle, ArrayOf<String>* supportedAbis)
{
    const int numAbis = supportedAbis->GetLength();

    ZipFileRO* zipFile = reinterpret_cast<ZipFileRO*>(apkHandle);
    if (zipFile == NULL) {
        return INSTALL_FAILED_INVALID_APK;
    }

    UniquePtr<NativeLibrariesIterator> it(NativeLibrariesIterator::create(zipFile));
    if (it.get() == NULL) {
        return INSTALL_FAILED_INVALID_APK;
    }

    ZipEntryRO entry = NULL;
    int status = NO_NATIVE_LIBRARIES;
    while ((entry = it->next()) != NULL) {
        // We're currently in the lib/ directory of the APK, so it does have some native
        // code. We should return INSTALL_FAILED_NO_MATCHING_ABIS if none of the
        // libraries match.
        if (status == NO_NATIVE_LIBRARIES) {
            status = INSTALL_FAILED_NO_MATCHING_ABIS;
        }

        const char* fileName = it->currentEntry();
        const char* lastSlash = it->lastSlash();

        // Check to see if this CPU ABI matches what we are looking for.
        const char* abiOffset = fileName + APK_LIB_LEN;
        const size_t abiSize = lastSlash - abiOffset;
        for (int i = 0; i < numAbis; i++) {
            String abi = (*supportedAbis)[i];
            if ((size_t)abi.GetByteLength() == abiSize && !strncmp(abiOffset, abi.string(), abiSize)) {
                // The entry that comes in first (i.e. with a lower index) has the higher priority.
                if (((i < status) && (status >= 0)) || (status < 0) ) {
                    status = i;
                }
            }
        }
    }

    return status;
}

}
}
}
}

