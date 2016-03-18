
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEWFACTORY_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEWFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Pm::IPackageInfo;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Top level factory, used creating all the main WebView implementation classes.
 *
 * @hide
 */
class WebViewFactory
{
private:
    // TODO
    // private static class RelroFileCreator {
    //     // Called in an unprivileged child process to create the relro file.
    //     public static void main(String[] args) {
    //         boolean result = false;
    //         boolean is64Bit = VMRuntime.getRuntime().is64Bit();
    //         try{
    //             if (args.length != 2 || args[0] == null || args[1] == null) {
    //                 Log.e(LOGTAG, "Invalid RelroFileCreator args: " + Arrays.toString(args));
    //                 return;
    //             }
    //             Log.v(LOGTAG, "RelroFileCreator (64bit = " + is64Bit + "), " +
    //                     " 32-bit lib: " + args[0] + ", 64-bit lib: " + args[1]);
    //             if (!sAddressSpaceReserved) {
    //                 Log.e(LOGTAG, "can't create relro file; address space not reserved");
    //                 return;
    //             }
    //             result = nativeCreateRelroFile(args[0] /* path32 */,
    //                                            args[1] /* path64 */,
    //                                            CHROMIUM_WEBVIEW_NATIVE_RELRO_32,
    //                                            CHROMIUM_WEBVIEW_NATIVE_RELRO_64);
    //             if (result && DEBUG) Log.v(LOGTAG, "created relro file");
    //         } finally {
    //             // We must do our best to always notify the update service, even if something fails.
    //             try {
    //                 getUpdateService().notifyRelroCreationCompleted(is64Bit, result);
    //             } catch (RemoteException e) {
    //                 Log.e(LOGTAG, "error notifying update service", e);
    //             }

    //             if (!result) Log.e(LOGTAG, "failed to create relro file");

    //             // Must explicitly exit or else this process will just sit around after we return.
    //             System.exit(0);
    //         }
    //     }
    // };

public:
    static CARAPI_(String) GetWebViewPackageName();

    static CARAPI_(AutoPtr<IPackageInfo>) GetLoadedPackageInfo();

    static CARAPI_(AutoPtr<IWebViewFactoryProvider>) GetProvider();

    /**
     * Perform any WebView loading preparations that must happen in the zygote.
     * Currently, this means allocating address space to load the real JNI library later.
     */
    static CARAPI PrepareWebViewInZygote();

    /**
     * Perform any WebView loading preparations that must happen at boot from the system server,
     * after the package manager has started or after an update to the webview is installed.
     * This must be called in the system server.
     * Currently, this means spawning the child processes which will create the relro files.
     */
    static CARAPI PrepareWebViewInSystemServer();

    static CARAPI OnWebViewUpdateInstalled();

private:
    //static Class<WebViewFactoryProvider> getFactoryClass();
    static AutoPtr<IClassInfo> GetFactoryClass();

    static CARAPI PrepareWebViewInSystemServer(
        /* [in] */ ArrayOf<String>* nativeLibraryPaths);

    static CARAPI_(AutoPtr< ArrayOf<String> >) GetWebViewNativeLibraryPaths();

    static CARAPI_(void) CreateRelroFile(
        /* [in] */ Boolean is64Bit,
        /* [in] */ ArrayOf<String>* nativeLibraryPaths);

    static CARAPI_(void) LoadNativeLibrary();

// TODO    static CARAPI_(AutoPtr<IWebViewUpdateService>) GetUpdateService();

    static CARAPI_(Boolean) NativeReserveAddressSpace(
        /* [in] */ Int64 addressSpaceToReserve);

    static CARAPI_(Boolean) NativeCreateRelroFile(
        /* [in] */ const String& lib32,
        /* [in] */ const String& lib64,
        /* [in] */ const String& relro32,
        /* [in] */ const String& relro64);

    static CARAPI_(Boolean) NativeLoadWithRelroFile(
        /* [in] */ const String& lib32,
        /* [in] */ const String& lib64,
        /* [in] */ const String& relro32,
        /* [in] */ const String& relro64);

private:
    static const String CHROMIUM_WEBVIEW_FACTORY;
    static const String NULL_WEBVIEW_FACTORY;
    static const String CHROMIUM_WEBVIEW_NATIVE_RELRO_32;
    static const String CHROMIUM_WEBVIEW_NATIVE_RELRO_64;

    static const Int64 CHROMIUM_WEBVIEW_DEFAULT_VMSIZE_BYTES = 100 * 1024 * 1024;

    static const String LOGTAG;

    static const Boolean DEBUG;

    // Cache the factory both for efficiency, and ensure any one process gets all webviews from the
    // same provider.
    static AutoPtr<IWebViewFactoryProvider> sProviderInstance;
    static Object sProviderLock;
    static Boolean sAddressSpaceReserved;
    static AutoPtr<IPackageInfo> sPackageInfo;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEWFACTORY_H__
