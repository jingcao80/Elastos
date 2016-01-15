
#include "elastos/droid/webkit/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/WebIconDatabaseAdapter.h"

using Elastos::Droid::Webkit::AndroidWebview::AwContents;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                        WebIconDatabaseAdapter
//=====================================================================
CAR_INTERFACE_IMPL(WebIconDatabaseAdapter, Object, IWebIconDatabase)

ECode WebIconDatabaseAdapter::Open(
    /* [in] */ const String& path)
{
    // ==================before translated======================
    // AwContents.setShouldDownloadFavicons();

    assert(0); // parm is unused, is that ok?
    AwContents::SetShouldDownloadFavicons();
    return NOERROR;
}

ECode WebIconDatabaseAdapter::Close()
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RemoveAllIcons()
{
    // ==================before translated======================
    // // Intentional no-op: we have no database so nothing to remove.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RequestIconForPageUrl(
    /* [in] */ const String& url,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::BulkRequestIconForPageUrl(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& where,
    /* [in] */ IWebIconDatabaseIconListener* listener)
{
    VALIDATE_NOT_NULL(cr);
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // // Intentional no-op: hidden in base class.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::RetainIconForPageUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

ECode WebIconDatabaseAdapter::ReleaseIconForPageUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // Intentional no-op.

    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


