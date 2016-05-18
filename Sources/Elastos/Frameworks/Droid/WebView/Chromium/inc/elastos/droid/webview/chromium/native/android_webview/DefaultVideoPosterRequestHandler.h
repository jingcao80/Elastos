#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_DEFAULTVIDEOPOSTERREQUESTHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_DEFAULTVIDEOPOSTERREQUESTHANDLER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebResourceResponse.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::AsyncTask;

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
//TODO , interface in IO.h has no namespace prefix; using Elastos::IO::IPipedOutputStream;
class IPipedOutputStream;//TODO remove when above line is ok

// import java.io.IOException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This class takes advantage of shouldInterceptRequest(), returns the bitmap from
 * WebChromeClient.getDefaultVidoePoster() when the mDefaultVideoPosterURL is requested.
 *
 * The shouldInterceptRequest is used to get the default video poster, if the url is
 * the mDefaultVideoPosterURL.
 */
class DefaultVideoPosterRequestHandler
:public Object
{
private:
    class UIRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        UIRunnable(
            /* [in] */ DefaultVideoPosterRequestHandler* owner,
            /* [in] */ IPipedOutputStream* outputStream,
            /* [in] */ AwContentsClient* contentClient);

        CARAPI Run();

    private:
        DefaultVideoPosterRequestHandler* mOwner;
        IPipedOutputStream* mOutputStream;
        AwContentsClient* mContentClient;
    };

    class AsyncTaskRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        AsyncTaskRunnable(
            /* [in] */ DefaultVideoPosterRequestHandler* owner,
            /* [in] */ IBitmap* defaultVideoPoster,
            /* [in] */ IPipedOutputStream* outputStream);

        CARAPI Run();

    private:
        DefaultVideoPosterRequestHandler* mOwner;
        AutoPtr<IBitmap> mDefaultVideoPoster;
        AutoPtr<IPipedOutputStream> mOutputStream;
    };

public:
    DefaultVideoPosterRequestHandler(
        /* [in] */ AwContentsClient* contentClient);

    /**
     * Used to get the image if the url is mDefaultVideoPosterURL.
     *
     * @param url the url requested
     * @return AwWebResourceResponse which caller can get the image if the url is
     * the default video poster URL, otherwise null is returned.
     */
    CARAPI_(AutoPtr<AwWebResourceResponse>) ShouldInterceptRequest(
        /* [in] */ const String& url);

    CARAPI_(String) GetDefaultVideoPosterURL();

private:
    static CARAPI_(AutoPtr<IInputStream>) GetInputStream(
        /* [in] */ AwContentsClient* contentClient, DefaultVideoPosterRequestHandler* pThis);

    static CARAPI_(void) CloseOutputStream(
        /* [in] */ IOutputStream* outputStream);

    /**
     * @return a unique URL which has little chance to be used by application.
     */
    static CARAPI_(String) GenerateDefaulVideoPosterURL();

private:
    static const String TAG;
    String mDefaultVideoPosterURL;
    AutoPtr<AwContentsClient> mContentClient;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_DEFAULTVIDEOPOSTERREQUESTHANDLER_H__
