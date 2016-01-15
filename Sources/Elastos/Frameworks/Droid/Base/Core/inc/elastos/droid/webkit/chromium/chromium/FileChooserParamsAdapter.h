
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_FILECHOOSERPARAMSADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_FILECHOOSERPARAMSADAPTER_H__

// import android.app.Activity;
// import android.content.Context;
// import android.content.Intent;
// import android.net.Uri;
// import android.webkit.WebChromeClient.FileChooserParams;

// import org.chromium.android_webview.AwContentsClient;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

class FileChooserParamsAdapter : public FileChooserParams
{
public:
    FileChooserParamsAdapter(
        /* [in] */ AwContentsClient::FileChooserParams* params,
        /* [in] */ IContext* context);

    static AutoPtr< ArrayOf<IUri> > ParseFileChooserResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI GetMode(
        /* [out] */ Int32* mode);

    //@Override
    CARAPI GetAcceptTypes(
        /* [out] */ ArrayOf<String>** acceptTypes);

    //@Override
    CARAPI IsCaptureEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    //@Override
    CARAPI GetFilenameHint(
        /* [out] */ String* fileName);

    //@Override
    CARAPI CreateIntent(
        /* [out] */ IIntent** intent);

private:
    AutoPtr<AwContentsClient::FileChooserParams> mParams;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_FILECHOOSERPARAMSADAPTER_H__
