
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/FileChooserParamsAdapter.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                       FileChooserParamsAdapter
//=====================================================================
CAR_INTERFACE_IMPL(FileChooserParamsAdapter, Object, IWebChromeClientFileChooserParams)

FileChooserParamsAdapter::FileChooserParamsAdapter(
    /* [in] */ AwContentsClient::FileChooserParams* params,
    /* [in] */ IContext* context)
    : mParams(params)
{
    // ==================before translated======================
    // mParams = params;
}

AutoPtr< ArrayOf<IUri*> > FileChooserParamsAdapter::ParseFileChooserResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // if (resultCode == Activity.RESULT_CANCELED) {
    //     return null;
    // }
    // Uri result = intent == null || resultCode != Activity.RESULT_OK ? null
    //         : intent.getData();
    //
    // Uri[] uris = null;
    // if (result != null) {
    //     uris = new Uri[1];
    //     uris[0] = result;
    // }
    // return uris;

    if (resultCode == IActivity::RESULT_CANCELED) {
        return NULL;
    }

    AutoPtr<IUri> result;
    if (intent == NULL || resultCode != IActivity::RESULT_OK) {
        result = NULL;
    }
    else {
        intent->GetData((IUri**)&result);
    }

    AutoPtr< ArrayOf<IUri*> > uris;
    if (result != NULL) {
        uris = ArrayOf<IUri*>::Alloc(1);
        uris->Set(0, result);
    }

    return uris;
}

ECode FileChooserParamsAdapter::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    // ==================before translated======================
    // return mParams.mode;

    *mode = mParams->mode;
    return NOERROR;
}

ECode FileChooserParamsAdapter::GetAcceptTypes(
    /* [out] */ ArrayOf<String>** acceptTypes)
{
    VALIDATE_NOT_NULL(acceptTypes);
    // ==================before translated======================
    // if (mParams.acceptTypes == null)
    //     return new String[0];
    // return mParams.acceptTypes.split(";");

    if (mParams->acceptTypes == String("")) {
        *acceptTypes = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*acceptTypes);
    }
    else {
        StringUtils::Split(mParams->acceptTypes, String(";"), acceptTypes);
    }

    return NOERROR;
}

ECode FileChooserParamsAdapter::IsCaptureEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParams.capture;

    *result = mParams->capture;
    return NOERROR;
}

ECode FileChooserParamsAdapter::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // return mParams.title;

    AutoPtr<ICharSequence> charSquenceStr;
    CString::New(mParams->title, title);
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode FileChooserParamsAdapter::GetFilenameHint(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    // ==================before translated======================
    // return mParams.defaultFilename;

    *name = mParams->defaultFilename;
    return NOERROR;
}

ECode FileChooserParamsAdapter::CreateIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // // TODO: Move this code to Aw. Once code is moved
    // // and merged to M37 get rid of this.
    // String mimeType = "*/*";
    // if (mParams.acceptTypes != null && !mParams.acceptTypes.trim().isEmpty())
    //     mimeType = mParams.acceptTypes.split(";")[0];
    //
    // Intent i = new Intent(Intent.ACTION_GET_CONTENT);
    // i.addCategory(Intent.CATEGORY_OPENABLE);
    // i.setType(mimeType);
    // return i;

    // TODO: Move this code to Aw. Once code is moved
    // and merged to M37 get rid of this.

    String mimeType("*/*");
    if (!mParams->acceptTypes.Trim().IsEmpty()) {
        AutoPtr< ArrayOf<String> > arrayTmp;
        StringUtils::Split(mParams->acceptTypes, String(";"), (ArrayOf<String>**)&arrayTmp);
        if (arrayTmp->GetLength() > 0) {
            mimeType = (*arrayTmp)[0];
        }
    }

    AutoPtr<IIntent> i;
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&i);
    i->AddCategory(IIntent::CATEGORY_OPENABLE);
    i->SetType(mimeType);

    *intent = i;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


