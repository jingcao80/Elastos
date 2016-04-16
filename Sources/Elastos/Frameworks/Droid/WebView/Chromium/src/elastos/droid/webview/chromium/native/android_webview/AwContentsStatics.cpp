#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwContentsStatics_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

AutoPtr<ClientCertLookupTable> AwContentsStatics::sClientCertLookupTable;
String AwContentsStatics::sUnreachableWebDataUrl;

/**
 * Return the client certificate lookup table.
 */
AutoPtr<ClientCertLookupTable> AwContentsStatics::GetClientCertLookupTable()
{
    ThreadUtils::AssertOnUiThread();
    if (sClientCertLookupTable == NULL) {
        sClientCertLookupTable = new ClientCertLookupTable();
    }
    return sClientCertLookupTable;
}

/**
 * Clear client cert lookup table. Should only be called from UI thread.
 */
void AwContentsStatics::ClearClientCertPreferences(
    /* [in] */ IRunnable* callback)
{
    ThreadUtils::AssertOnUiThread();
    GetClientCertLookupTable()->Clear();
    NativeClearClientCertPreferences(callback);
}

//@CalledByNative
//param callback is IRunnable
void AwContentsStatics::ClientCertificatesCleared(
    /* [in] */ IInterface* callback)
{
    if (callback == NULL) return;
    AutoPtr<IRunnable> runCallback = IRunnable::Probe(callback);
    runCallback->Run();
}

/**
 * Set Data Reduction Proxy key for authentication.
 */
void AwContentsStatics::SetDataReductionProxyKey(
    /* [in] */ const String& key)
{
    ThreadUtils::AssertOnUiThread();
    NativeSetDataReductionProxyKey(key);
}

/*
 * Enable or disable data reduction proxy.
 */
void AwContentsStatics::SetDataReductionProxyEnabled(
    /* [in] */ Boolean enabled)
{
    ThreadUtils::AssertOnUiThread();
    NativeSetDataReductionProxyEnabled(enabled);
}

String AwContentsStatics::GetUnreachableWebDataUrl()
{
    if (sUnreachableWebDataUrl.IsNullOrEmpty()) {
        sUnreachableWebDataUrl = NativeGetUnreachableWebDataUrl();
    }
    return sUnreachableWebDataUrl;
}

void AwContentsStatics::SetRecordFullDocument(
    /* [in] */ Boolean recordFullDocument)
{
    NativeSetRecordFullDocument(recordFullDocument);
}

//--------------------------------------------------------------------------------------------
//  Native methods
//--------------------------------------------------------------------------------------------
void AwContentsStatics::NativeClearClientCertPreferences(
    /* [in] */ IRunnable* callback)
{
    Elastos_AwContentsStatics_nativeClearClientCertPreferences(TO_IINTERFACE(callback));
}

void AwContentsStatics::NativeSetDataReductionProxyKey(
    /* [in] */ const String& key)
{
    Elastos_AwContentsStatics_nativeSetDataReductionProxyKey(key);
}

void AwContentsStatics::NativeSetDataReductionProxyEnabled(
    /* [in] */ Boolean enabled)
{
    Elastos_AwContentsStatics_nativeSetDataReductionProxyEnabled(enabled);
}

String AwContentsStatics::NativeGetUnreachableWebDataUrl()
{
    return Elastos_AwContentsStatics_nativeGetUnreachableWebDataUrl();
}

void AwContentsStatics::NativeSetRecordFullDocument(
    /* [in] */ Boolean recordFullDocument)
{
    Elastos_AwContentsStatics_nativeSetRecordFullDocument(recordFullDocument);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
