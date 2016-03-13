
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/ResourceBundle.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/ResourceBundle_dec.h"

using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Content::Res::IAssetManager;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//                            ResourceBundle
//=====================================================================
Boolean ResourceBundle::AssetContainedInApk(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& filename)
{
    // ==================before translated======================
    // try {
    //     AssetManager am = ctx.getAssets();
    //     AssetFileDescriptor afd = am.openFd(filename);
    //     afd.close();
    //     return true;
    // } catch (IOException e) {
    //     return false;
    // }

    //try {
        AutoPtr<IAssetManager> am;
        ctx->GetAssets((IAssetManager**)&am);

        AutoPtr<IAssetFileDescriptor> afd;
        am->OpenFd(filename, (IAssetFileDescriptor**)&afd);
        afd->Close();
        return TRUE;
    //} catch (IOException e) {
    //    return false;
    //}
}

Boolean ResourceBundle::AssetContainedInApk(
    /* [in] */ IInterface* ctx,
    /* [in] */ const String& filename)
{
    IContext* c = IContext::Probe(ctx);
    return AssetContainedInApk(c, filename);
}

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


