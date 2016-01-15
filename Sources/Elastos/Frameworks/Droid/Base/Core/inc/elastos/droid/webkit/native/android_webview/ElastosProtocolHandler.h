#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_ANDROIDPROTOCOLHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_ANDROIDPROTOCOLHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Net::IUri;
using Elastos::IO::IInputStream;
// import android.util.TypedValue;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

// import java.io.IOException;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Implements the Java side of Android URL protocol jobs.
 * See android_protocol_handler.cc.
 */
//@JNINamespace("android_webview")
class ElastosProtocolHandler
    : public Object
{
public:
    /**
     * Open an InputStream for an Android resource.
     * @param context The context manager.
     * @param url The url to load.
     * @return An InputStream to the Android resource.
     */
    //@CalledByNative return IInputStream IContext
    static CARAPI_(AutoPtr<IInterface>) Open(
        /* [in] */ IInterface* context,
        /* [in] */ const String& url);

    /**
     * Determine the mime type for an Android resource.
     * @param context The context manager.
     * @param stream The opened input stream which to examine.
     * @param url The url from which the stream was opened.
     * @return The mime type or null if the type is unknown.
     */
    //@CalledByNative IContext IInputStream
    static CARAPI_(String) GetMimeType(
        /* [in] */ IInterface* context,
        /* [in] */ IInterface* stream,
        /* [in] */ const String& url);

    /**
     * Set the context to be used for resolving resource queries.
     * @param context Context to be used, or null for the default application
     *                context.
     */
    static CARAPI_(void) SetResourceContextForTesting(
        /* [in] */ IContext* context);

//callback function declaration
public:
    static CARAPI_(void*) ElaElastosProtocolHandlerCallback_Init();

private:
    static const String TAG;

    // Supported URL schemes. This needs to be kept in sync with
    // clank/native/framework/chrome/url_request_android_job.cc.
    static const String FILE_SCHEME;
    static const String CONTENT_SCHEME;

    static Int32 GetFieldId(
        /* [in] */ IContext* context,
        /* [in] */ const String& assetType,
        /* [in] */ const String& assetName);

    static CARAPI_(Int32) GetValueType(
        /* [in] */ IContext* context,
        /* [in] */ Int32 fieldId);

    static CARAPI_(AutoPtr<IInputStream>) OpenResource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    static CARAPI_(AutoPtr<IInputStream>) OpenAsset(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    static CARAPI_(AutoPtr<IInputStream>) OpenContent(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    /**
     * Make sure the given string URL is correctly formed and parse it into a Uri.
     * @return a Uri instance, or null if the URL was invalid.
     */
    static CARAPI_(AutoPtr<IUri>) VerifyUrl(
        /* [in] */ const String& url);

    static CARAPI_(void) NativeSetResourceContextForTesting(
        /* [in] */ IContext* context);

    static CARAPI_(String) NativeGetAndroidAssetPath();

    static CARAPI_(String) NativeGetAndroidResourcePath();
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_ANDROIDPROTOCOLHANDLER_H__
