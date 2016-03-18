
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Webkit.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <android/dlext.h>

#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/R.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include "elastos/core/AutoLock.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {

static void* gReservedAddress = NULL;
static size_t gReservedSize = 0;

//const String WebViewFactory::CHROMIUM_WEBVIEW_FACTORY("com.android.webview.chromium.WebViewChromiumFactoryProvider");
const String WebViewFactory::CHROMIUM_WEBVIEW_FACTORY("LElastos/Droid/Webkit/Webview/Chromium/WebViewChromiumFactoryProvider;");
const String WebViewFactory::NULL_WEBVIEW_FACTORY("com.android.webview.nullwebview.NullWebViewFactoryProvider");
const String WebViewFactory::CHROMIUM_WEBVIEW_NATIVE_RELRO_32("/data/misc/shared_relro/libwebviewchromium32.relro");
const String WebViewFactory::CHROMIUM_WEBVIEW_NATIVE_RELRO_64("/data/misc/shared_relro/libwebviewchromium64.relro");

const Int64 WebViewFactory::CHROMIUM_WEBVIEW_DEFAULT_VMSIZE_BYTES;

const String WebViewFactory::LOGTAG("WebViewFactory");

const Boolean WebViewFactory::DEBUG = FALSE;

//============================================================================================
// native codes
//============================================================================================
//static Boolean nativeReserveAddressSpace(
//    /* [in] */ Int64 size)
//{
//    size_t vsize = static_cast<size_t>(size);
//
//    void* addr = mmap(NULL, vsize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//    if (addr == MAP_FAILED) {
//        ALOGE("Failed to reserve %zd bytes of address space for future load of "
//              "libwebviewchromium.so: %s",
//              vsize, strerror(errno));
//        return FALSE;
//    }
//    gReservedAddress = addr;
//    gReservedSize = vsize;
//    ALOGV("Reserved %zd bytes at %p", vsize, addr);
//    return TRUE;
//}

//============================================================================================
// WebViewFactory
//============================================================================================
// Cache the factory both for efficiency, and ensure any one process gets all webviews from the
// same provider.
AutoPtr<IWebViewFactoryProvider> WebViewFactory::sProviderInstance;
Object WebViewFactory::sProviderLock;
Boolean WebViewFactory::sAddressSpaceReserved = FALSE;
AutoPtr<IPackageInfo> WebViewFactory::sPackageInfo;

String WebViewFactory::GetWebViewPackageName()
{
    AutoPtr<IApplication> application = AppGlobals::GetInitialApplication();
    String result;
    IContext::Probe(application)->GetString(R::string::config_webViewPackageName, &result);
    //TODO the package name maybe need update
    Logger::D("WebViewFactory::GetWebViewPackageName", "packageName is: %s", result.string());
    return result;
}

AutoPtr<IPackageInfo> WebViewFactory::GetLoadedPackageInfo()
{
    return sPackageInfo;
}

AutoPtr<IWebViewFactoryProvider> WebViewFactory::GetProvider()
{
    {
        AutoLock lock(sProviderLock);
        // For now the main purpose of this function (and the factory abstraction) is to keep
        // us honest and minimize usage of WebView internals when binding the proxy.
        if (sProviderInstance != NULL) return sProviderInstance;

        //Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "WebViewFactory.getProvider()");
        //try {
            //Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "WebViewFactory.loadNativeLibrary()");
            //loadNativeLibrary();
            //Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);

            //Class<WebViewFactoryProvider> providerClass;
            //Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "WebViewFactory.getFactoryClass()");
            //try {
            //    providerClass = getFactoryClass();
            //} catch (ClassNotFoundException e) {
            //    Log.e(LOGTAG, "error loading provider", e);
            //    throw new AndroidRuntimeException(e);
            //} finally {
            //    Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);
            //}
            AutoPtr<IClassInfo> providerClass = GetFactoryClass();
            if (providerClass == NULL) {
                Logger::E(LOGTAG, "error loading provider");
                return NULL;
            }

            //StrictMode.ThreadPolicy oldPolicy = StrictMode.allowThreadDiskReads();
            AutoPtr<IStrictMode> strictMode;
            CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
            AutoPtr<IStrictModeThreadPolicy> oldPolicy;
            strictMode->AllowThreadDiskReads((IStrictModeThreadPolicy**)&oldPolicy);
            //Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "providerClass.newInstance()");
            //try {
                //sProviderInstance = providerClass.newInstance();
                AutoPtr<IConstructorInfo> constructor;
                //ECode ec = providerClass->GetConstructorInfoByCount(0, (IConstructorInfo**)&constructor);
                ECode ec = providerClass->GetConstructorInfoByParamNames(String(""), (IConstructorInfo**)&constructor);
                if (FAILED(ec)) {
                    Logger::E(LOGTAG, "WebViewFactory: get constructor failed.");
                    return NULL;
                }
                AutoPtr<IArgumentList> args;
                constructor->CreateArgumentList((IArgumentList**)&args);
                AutoPtr<IInterface> object;
                ec = constructor->CreateObject(args, (IInterface**)&object);
                if (FAILED(ec)) {
                    Logger::E(LOGTAG, "WebViewFactory: Create webviewfactory object failed.");
                    return NULL;
                }
                sProviderInstance = IWebViewFactoryProvider::Probe(object);
                if (DEBUG) Logger::V(LOGTAG, "Loaded provider: %p", sProviderInstance.Get());
            //} catch (Exception e) {
            //    Log.e(LOGTAG, "error instantiating provider", e);
            //    throw new AndroidRuntimeException(e);
            //} finally {
            //    Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);
                //StrictMode.setThreadPolicy(oldPolicy);
                strictMode->SetThreadPolicy(oldPolicy);
            //}
        //} finally {
        //    Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);
        //}
    }
    return sProviderInstance;
}

//private static Class<WebViewFactoryProvider> getFactoryClass() throws ClassNotFoundException {
AutoPtr<IClassInfo> WebViewFactory::GetFactoryClass()
{
    AutoPtr<IApplication> initialApplication = AppGlobals::GetInitialApplication();
    //try {
        // First fetch the package info so we can log the webview package version.
        String packageName = GetWebViewPackageName();
        AutoPtr<IPackageManager> pm;
        IContext::Probe(initialApplication)->GetPackageManager((IPackageManager**)&pm);
        sPackageInfo  = NULL;
        pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&sPackageInfo);
        String versionName;
        Int32 versionCode;
        sPackageInfo->GetVersionName(&versionName);
        sPackageInfo->GetVersionCode(&versionCode);
        Logger::I(LOGTAG, "Loading %s versionName: %s, versionCode: %d",
                packageName.string(), versionName.string(), versionCode);

        // Construct a package context to load the Java code into the current app.
        AutoPtr<IContext> webViewContext;
        IContext::Probe(initialApplication)->CreatePackageContext(packageName,
                IContext::CONTEXT_INCLUDE_CODE | IContext::CONTEXT_IGNORE_SECURITY,
                (IContext**)&webViewContext);
        AutoPtr<IAssetManager> assetManager;
        IContext::Probe(initialApplication)->GetAssets((IAssetManager**)&assetManager);

        AutoPtr<IApplicationInfo> appInfo;
        webViewContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String srcDir;
        appInfo->GetSourceDir(&srcDir);
        Int32 cookie;
        assetManager->AddAssetPath(srcDir, &cookie);

        StringBuilder sb;
        sb.Append("/data/elastos/");
        sb.Append(packageName);
        sb.Append(".eco");

        String path = sb.ToString();
        AutoPtr<IModuleInfo> moduleInfo;
        ECode ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
        if (FAILED(ec)) {
            Logger::E(LOGTAG, "WebViewFactory: Cann't Find the path is %s", path.string());
            return NULL;
        }

        //String className = CHROMIUM_WEBVIEW_FACTORY;
        //Int32 index = className.LastIndexOf('.');
        //String shortClassName = index > 0 ? className.Substring(index + 1) : className;
        AutoPtr<IClassInfo> classInfo;
        ec = moduleInfo->GetClassInfo(CHROMIUM_WEBVIEW_FACTORY, (IClassInfo**)&classInfo);
        if (FAILED(ec)) {
            Logger::E(LOGTAG, "WebViewFactory: Get class info of %s failed.", CHROMIUM_WEBVIEW_FACTORY.string());
            return NULL;
        }

        return classInfo;

        //ClassLoader clazzLoader = webViewContext.getClassLoader();
        //Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "Class.forName()");
        //try {
        //    return (Class<WebViewFactoryProvider>) Class.forName(CHROMIUM_WEBVIEW_FACTORY, true,
        //                                                         clazzLoader);
        //} finally {
        //    Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);
        //}
    //} catch (PackageManager.NameNotFoundException e) {
    //    // If the package doesn't exist, then try loading the null WebView instead.
    //    // If that succeeds, then this is a device without WebView support; if it fails then
    //    // swallow the failure, complain that the real WebView is missing and rethrow the
    //    // original exception.
    //    try {
    //        return (Class<WebViewFactoryProvider>) Class.forName(NULL_WEBVIEW_FACTORY);
    //    } catch (ClassNotFoundException e2) {
    //        // Ignore.
    //    }
    //    Log.e(LOGTAG, "Chromium WebView package does not exist", e);
    //    throw new AndroidRuntimeException(e);
    //}
}

/**
 * Perform any WebView loading preparations that must happen in the zygote.
 * Currently, this means allocating address space to load the real JNI library later.
 */
ECode WebViewFactory::PrepareWebViewInZygote()
{
    // TODO
    // try {
    //     System.loadLibrary("webviewchromium_loader");
//TODO(leliang) the codes below used to reserved memory for the native so,
// do not think it is needed here
//    Int64 addressSpaceToReserve;
//    SystemProperties::GetInt64(IWebViewFactory::CHROMIUM_WEBVIEW_VMSIZE_SIZE_PROPERTY,
//        CHROMIUM_WEBVIEW_DEFAULT_VMSIZE_BYTES, &addressSpaceToReserve);
//    sAddressSpaceReserved = nativeReserveAddressSpace(addressSpaceToReserve);
//
//    if (sAddressSpaceReserved) {
//        if (DEBUG) {
//            Logger::V(LOGTAG, "address space reserved: %lld bytes", addressSpaceToReserve);
//        }
//    } else {
//        Logger::E(LOGTAG, "reserving %lld bytes of address space failed", addressSpaceToReserve);
//    }
    // } catch (Throwable t) {
    //     // Log and discard errors at this stage as we must not crash the zygote.
    //     Log.e(LOGTAG, "error preparing native loader", t);
    // }
    return NOERROR;
}

/**
 * Perform any WebView loading preparations that must happen at boot from the system server,
 * after the package manager has started or after an update to the webview is installed.
 * This must be called in the system server.
 * Currently, this means spawning the child processes which will create the relro files.
 */
ECode WebViewFactory::PrepareWebViewInSystemServer()
{
    AutoPtr< ArrayOf<String> > nativePaths;
    // try {
        nativePaths = GetWebViewNativeLibraryPaths();
    // } catch (Throwable t) {
    //     // Log and discard errors at this stage as we must not crash the system server.
    //     Log.e(LOGTAG, "error preparing webview native library", t);
    // }
    return PrepareWebViewInSystemServer(nativePaths);
}

ECode WebViewFactory::PrepareWebViewInSystemServer(
    /* [in] */ ArrayOf<String>* nativeLibraryPaths)
{
    Logger::D(LOGTAG, "WebViewFactory::PrepareWebViewInSystemServer(paths)");
////TODO now the relro not supported
//    if (DEBUG) {
//        Logger::V(LOGTAG, "creating relro files");
//    }
//
//    // We must always trigger createRelRo regardless of the value of nativeLibraryPaths. Any
//    // unexpected values will be handled there to ensure that we trigger notifying any process
//    // waiting on relreo creation.
//    if (Build::SUPPORTED_32_BIT_ABIS->GetLength() > 0) {
//        if (DEBUG) Logger::V(LOGTAG, "Create 32 bit relro");
//        CreateRelroFile(FALSE /* is64Bit */, nativeLibraryPaths);
//    }
//
//    if (Build::SUPPORTED_64_BIT_ABIS->GetLength() > 0) {
//        if (DEBUG) Logger::V(LOGTAG, "Create 64 bit relro");
//        CreateRelroFile(TRUE /* is64Bit */, nativeLibraryPaths);
//    }
//
    return NOERROR;
}

ECode WebViewFactory::OnWebViewUpdateInstalled()
{
    assert(0);
    // TODO
    // String[] nativeLibs = null;
    // try {
    //     nativeLibs = WebViewFactory.getWebViewNativeLibraryPaths();
    //     if (nativeLibs != null) {
    //         long newVmSize = 0L;

    //         for (String path : nativeLibs) {
    //             if (DEBUG) Log.d(LOGTAG, "Checking file size of " + path);
    //             if (path == null) continue;
    //             File f = new File(path);
    //             if (f.exists()) {
    //                 long length = f.length();
    //                 if (length > newVmSize) {
    //                     newVmSize = length;
    //                 }
    //             }
    //         }

    //         if (DEBUG) {
    //             Log.v(LOGTAG, "Based on library size, need " + newVmSize +
    //                     " bytes of address space.");
    //         }
    //         // The required memory can be larger than the file on disk (due to .bss), and an
    //         // upgraded version of the library will likely be larger, so always attempt to
    //         // reserve twice as much as we think to allow for the library to grow during this
    //         // boot cycle.
    //         newVmSize = Math.max(2 * newVmSize, CHROMIUM_WEBVIEW_DEFAULT_VMSIZE_BYTES);
    //         Log.d(LOGTAG, "Setting new address space to " + newVmSize);
    //         SystemProperties.set(CHROMIUM_WEBVIEW_VMSIZE_SIZE_PROPERTY,
    //                 Long.toString(newVmSize));
    //     }
    // } catch (Throwable t) {
    //     // Log and discard errors at this stage as we must not crash the system server.
    //     Log.e(LOGTAG, "error preparing webview native library", t);
    // }
    // prepareWebViewInSystemServer(nativeLibs);
    return E_NOT_IMPLEMENTED;
}

AutoPtr< ArrayOf<String> > WebViewFactory::GetWebViewNativeLibraryPaths()
{
//TODO(leliang) do not needed
//    assert(0);
//    // TODO
//    // final String NATIVE_LIB_FILE_NAME = "libwebviewchromium.so";
//
//    // PackageManager pm = AppGlobals.getInitialApplication().getPackageManager();
//    // ApplicationInfo ai = pm.getApplicationInfo(getWebViewPackageName(), 0);
//
//    AutoPtr<ISystem> system;
//    CSystem::AcquireSingleton((ISystem**)&system);
//    // String path32;
//    // String path64;
//    // boolean primaryArchIs64bit;
//    // system->Is64BitAbi(ai.primaryCpuAbi, &primaryArchIs64bit);
//    // if (!TextUtils.isEmpty(ai.secondaryCpuAbi)) {
//    //     // Multi-arch case.
//    //     if (primaryArchIs64bit) {
//    //         // Primary arch: 64-bit, secondary: 32-bit.
//    //         path64 = ai.nativeLibraryDir;
//    //         path32 = ai.secondaryNativeLibraryDir;
//    //     } else {
//    //         // Primary arch: 32-bit, secondary: 64-bit.
//    //         path64 = ai.secondaryNativeLibraryDir;
//    //         path32 = ai.nativeLibraryDir;
//    //     }
//    // } else if (primaryArchIs64bit) {
//    //     // Single-arch 64-bit.
//    //     path64 = ai.nativeLibraryDir;
//    //     path32 = "";
//    // } else {
//    //     // Single-arch 32-bit.
//    //     path32 = ai.nativeLibraryDir;
//    //     path64 = "";
//    // }
//    // if (!TextUtils.isEmpty(path32)) path32 += "/" + NATIVE_LIB_FILE_NAME;
//    // if (!TextUtils.isEmpty(path64)) path64 += "/" + NATIVE_LIB_FILE_NAME;
//    // return new String[] { path32, path64 };

    return NULL;
}

void WebViewFactory::CreateRelroFile(
    /* [in] */ Boolean is64Bit,
    /* [in] */ ArrayOf<String>* nativeLibraryPaths)
{
    // assert(0);
    // TODO
    // final String abi =
    //         is64Bit ? Build.SUPPORTED_64_BIT_ABIS[0] : Build.SUPPORTED_32_BIT_ABIS[0];

    // // crashHandler is invoked by the ActivityManagerService when the isolated process crashes.
    // Runnable crashHandler = new Runnable() {
    //     @Override
    //     public void run() {
    //         try {
    //             Log.e(LOGTAG, "relro file creator for " + abi + " crashed. Proceeding without");
    //             getUpdateService().notifyRelroCreationCompleted(is64Bit, false);
    //         } catch (RemoteException e) {
    //             Log.e(LOGTAG, "Cannot reach WebViewUpdateService. " + e.getMessage());
    //         }
    //     }
    // };

    // try {
    //     if (nativeLibraryPaths == null
    //             || nativeLibraryPaths[0] == null || nativeLibraryPaths[1] == null) {
    //         throw new IllegalArgumentException(
    //                 "Native library paths to the WebView RelRo process must not be null!");
    //     }
    //     int pid = LocalServices.getService(ActivityManagerInternal.class).startIsolatedProcess(
    //             RelroFileCreator.class.getName(), nativeLibraryPaths, "WebViewLoader-" + abi, abi,
    //             Process.SHARED_RELRO_UID, crashHandler);
    //     if (pid <= 0) throw new Exception("Failed to start the relro file creator process");
    // } catch (Throwable t) {
    //     // Log and discard errors as we must not crash the system server.
    //     Log.e(LOGTAG, "error starting relro file creator for abi " + abi, t);
    //     crashHandler.run();
    // }
}

void WebViewFactory::LoadNativeLibrary()
{
    Logger::W(LOGTAG, "TODO WebViewFactory::LoadNativeLibrary()");
    return;

    if (!sAddressSpaceReserved) {
        Logger::E(LOGTAG, "can't load with relro file; address space not reserved");
        return;
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Boolean is64Bit;
    system->Is64Bit(&is64Bit);
    // TODO
    // try {
    //     getUpdateService().waitForRelroCreationCompleted(is64Bit);
    // } catch (RemoteException e) {
    //     Log.e(LOGTAG, "error waiting for relro creation, proceeding without", e);
    //     return;
    // }

    // try {
        AutoPtr< ArrayOf<String> > args = GetWebViewNativeLibraryPaths();
        Boolean result = NativeLoadWithRelroFile((*args)[0] /* path32 */,
                                                 (*args)[1] /* path64 */,
                                                 CHROMIUM_WEBVIEW_NATIVE_RELRO_32,
                                                 CHROMIUM_WEBVIEW_NATIVE_RELRO_64);
        if (!result) {
            Logger::W(LOGTAG, "failed to load with relro file, proceeding without");
        }
        else if (DEBUG) {
            Logger::V(LOGTAG, "loaded with relro file");
        }
    // } catch (PackageManager.NameNotFoundException e) {
    //     Log.e(LOGTAG, "Failed to list WebView package libraries for loadNativeLibrary", e);
    // }
}

// TODO
// AutoPtr<IWebViewUpdateService> WebViewFactory::GetUpdateService()
// {
//     return IWebViewUpdateService.Stub.asInterface(ServiceManager.getService("webviewupdate"));
// }

Boolean WebViewFactory::NativeReserveAddressSpace(
    /* [in] */ Int64 addressSpaceToReserve)
{
    size_t vsize = static_cast<size_t>(addressSpaceToReserve);
    void *addr = mmap(NULL, vsize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        Logger::E(LOGTAG, "Failed to reserve %zd bytes of address space for future load of libelwebviewchromium.so: %s", vsize, strerror(errno));
        return FALSE;
    }
    gReservedAddress = addr;
    gReservedSize = vsize;
    Logger::I(LOGTAG, "Reserved  %zd bytes at %p", vsize, addr);
    return TRUE;
}

Boolean DoCreateRelroFile(const char* lib, const char* relro) {
    // Try to unlink the old file, since if this is being called, the old one is
    // obsolete.
    if (unlink(relro) != 0 && errno != ENOENT) {
          // If something went wrong other than the file not existing, log a warning
          // but continue anyway in the hope that we can successfully overwrite the
          // existing file with rename() later.
        Logger::W("WebViewFactory", "Failed to unlink old file %s: %s", relro, strerror(errno));
    }
    static const char tmpsuffix[] = ".XXXXXX";
    char relro_tmp[strlen(relro) + sizeof(tmpsuffix)];
    strlcpy(relro_tmp, relro, sizeof(relro_tmp));
    strlcat(relro_tmp, tmpsuffix, sizeof(relro_tmp));
    int tmp_fd = TEMP_FAILURE_RETRY(mkstemp(relro_tmp));
    if (tmp_fd == -1) {
        Logger::W("WebViewFactory", "Failed to create temporary file %s: %s", relro_tmp, strerror(errno));
        return FALSE;
    }
    android_dlextinfo extinfo;
    extinfo.flags = ANDROID_DLEXT_RESERVED_ADDRESS | ANDROID_DLEXT_WRITE_RELRO;
    extinfo.reserved_addr = gReservedAddress;
    extinfo.reserved_size = gReservedSize;
    extinfo.relro_fd = tmp_fd;
    void* handle = android_dlopen_ext(lib, RTLD_NOW, &extinfo);
    int close_result = close(tmp_fd);
    if (handle == NULL) {
        Logger::W("WebViewFactory", "Failed to load library %s: %s", lib, dlerror());
        unlink(relro_tmp);
        return FALSE;
    }
    if (close_result != 0 ||
            chmod(relro_tmp, S_IRUSR | S_IRGRP | S_IROTH) != 0 ||
            rename(relro_tmp, relro) != 0) {
        Logger::W("WebViewFactory", "Failed to update relro file %s: %s", relro, strerror(errno));
        unlink(relro_tmp);
        return FALSE;
    }
    //Logger::W("WebViewFactory", "Created relro file %s for library %s", relro, lib);
    return TRUE;
}

Boolean WebViewFactory::NativeCreateRelroFile(
    /* [in] */ const String& lib32,
    /* [in] */ const String& lib64,
    /* [in] */ const String& relro32,
    /* [in] */ const String& relro64)
{
#ifdef __LP64__
    String lib = lib64;
    String relro = relro64;
    (void)lib32; (void)relro32;
#else
    String lib = lib32;
    String relro = relro32;
    (void)lib64; (void)relro64;
#endif
    Boolean ret = FALSE;
    if (!lib.IsNullOrEmpty() && !relro.IsNullOrEmpty()) {
        ret = DoCreateRelroFile(lib.string(), relro.string());
    }
    return ret;
}

Boolean DoLoadWithRelroFile(const char* lib, const char* relro) {
    int relro_fd = TEMP_FAILURE_RETRY(open(relro, O_RDONLY));
    if (relro_fd == -1) {
        Logger::W("WebViewFactory", "Failed to open relro file %s: %s", relro, strerror(errno));
        return FALSE;
    }
    android_dlextinfo extinfo;
    extinfo.flags = ANDROID_DLEXT_RESERVED_ADDRESS | ANDROID_DLEXT_USE_RELRO;
    extinfo.reserved_addr = gReservedAddress;
    extinfo.reserved_size = gReservedSize;
    extinfo.relro_fd = relro_fd;
    void* handle = android_dlopen_ext(lib, RTLD_NOW, &extinfo);
    close(relro_fd);
    if (handle == NULL) {
        Logger::W("WebViewFactory", "Failed to load library %s: %s", lib, dlerror());
        return FALSE;
    }
    //Logger::I("WebViewFactory", "Loaded library %s with relro file %s", lib, relro);
    return TRUE;
}

Boolean WebViewFactory::NativeLoadWithRelroFile(
    /* [in] */ const String& lib32,
    /* [in] */ const String& lib64,
    /* [in] */ const String& relro32,
    /* [in] */ const String& relro64)
{
#ifdef __LP64__
    String lib = lib64;
    String relro = relro64;
    (void)lib32; (void)relro32;
#else
    String lib = lib32;
    String relro = relro32;
    (void)lib64; (void)relro64;
#endif
    Boolean ret = FALSE;
    if (!lib.IsNullOrEmpty() && !relro.IsNullOrEmpty()) {
        ret = DoLoadWithRelroFile(lib.string(), relro.string());
    }
    return ret;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
