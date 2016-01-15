#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSSTATICS_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSSTATICS_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/android_webview/ClientCertLookupTable.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Implementations of various static methods, and also a home for static
 * data structures that are meant to be shared between all webviews.
 */
//@JNINamespace("android_webview")
class AwContentsStatics
    : public Object
{
public:
    /**
     * Return the client certificate lookup table.
     */
    static CARAPI_(AutoPtr<ClientCertLookupTable>) GetClientCertLookupTable();

    /**
     * Clear client cert lookup table. Should only be called from UI thread.
     */
    static CARAPI_(void) ClearClientCertPreferences(
        /* [in] */ IRunnable* callback);

    /**
     * Set Data Reduction Proxy key for authentication.
     */
    static CARAPI_(void) SetDataReductionProxyKey(
        /* [in] */ const String& key);

    /*
     * Enable or disable data reduction proxy.
     */
    static CARAPI_(void) SetDataReductionProxyEnabled(
        /* [in] */ Boolean enabled);

    static CARAPI_(String) GetUnreachableWebDataUrl();

    static CARAPI_(void) SetRecordFullDocument(
        /* [in] */ Boolean recordFullDocument);

private:
    //@CalledByNative parameter callback is IRunnable
    static CARAPI_(void) ClientCertificatesCleared(
        /* [in] */ IInterface* callback);

    //--------------------------------------------------------------------------------------------
    //  Native methods
    //--------------------------------------------------------------------------------------------
    static CARAPI_(void) NativeClearClientCertPreferences(
        /* [in] */ IRunnable* callback);

    static CARAPI_(void) NativeSetDataReductionProxyKey(
        /* [in] */ const String& key);

    static CARAPI_(void) NativeSetDataReductionProxyEnabled(
        /* [in] */ Boolean enabled);

    static CARAPI_(String) NativeGetUnreachableWebDataUrl();

    static CARAPI_(void) NativeSetRecordFullDocument(
        /* [in] */ Boolean recordFullDocument);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwContentsStaticsCallback_Init();

private:
    static AutoPtr<ClientCertLookupTable> sClientCertLookupTable;

    static String sUnreachableWebDataUrl;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSSTATICS_H__
