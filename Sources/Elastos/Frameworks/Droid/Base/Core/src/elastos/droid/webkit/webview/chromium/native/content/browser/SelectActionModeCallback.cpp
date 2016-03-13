
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/SelectActionModeCallback.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                       SelectActionModeCallback
//=====================================================================

SelectActionModeCallback::SelectActionModeCallback(
    /* [in] */ IContext* context,
    /* [in] */ ActionHandler* actionHandler,
    /* [in] */ Boolean incognito)
    : mContext(context)
    , mActionHandler(actionHandler)
    , mIncognito(incognito)
{
}

ECode SelectActionModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mode->SetTitle((ICharSequence*)NULL);
    mode->SetSubtitle((ICharSequence*)NULL);
    mActionHandler->IsSelectionEditable(&mEditable);
    mActionHandler->IsSelectionPassword(&mIsPasswordType);
    CreateActionMenu(mode, menu);
    *result = TRUE;
    return NOERROR;
}

ECode SelectActionModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isEditableNow;
    mActionHandler->IsSelectionEditable(&isEditableNow);
    Boolean isPasswordNow;
    mActionHandler->IsSelectionPassword(&isPasswordNow);
    if (mEditable != isEditableNow || mIsPasswordType != isPasswordNow) {
        mEditable = isEditableNow;
        mIsPasswordType = isPasswordNow;
        menu->Clear();
        CreateActionMenu(mode, menu);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode SelectActionModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    item->GetItemId(&id);

    if (id == R::id::select_action_menu_select_all) {
        mActionHandler->SelectAll();
    }
    else if (id == R::id::select_action_menu_cut) {
        mActionHandler->Cut();
    }
    else if (id == R::id::select_action_menu_copy) {
        mActionHandler->Copy();
        mode->Finish();
    }
    else if (id == R::id::select_action_menu_paste) {
        mActionHandler->Paste();
    }
    else if (id == R::id::select_action_menu_share) {
        mActionHandler->Share();
        mode->Finish();
    }
    else if (id == R::id::select_action_menu_web_search) {
        mActionHandler->Search();
        mode->Finish();
    }
    else {
        return FALSE;
    }

    *result = TRUE;

    return NOERROR;
}

ECode SelectActionModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    mActionHandler->OnDestroyActionMode();
    return NOERROR;
}

AutoPtr<IContext> SelectActionModeCallback::GetContext()
{
    return mContext;
}

ECode SelectActionModeCallback::CreateActionMenu(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu)
{
    AutoPtr<IMenuInflater> menuInflater;
    mode->GetMenuInflater((IMenuInflater**)&menuInflater);
    menuInflater->Inflate(R::menu::select_action_menu, menu);
    if (!mEditable || !CanPaste()) {
        menu->RemoveItem(R::id::select_action_menu_paste);
    }

    if (!mEditable) {
        menu->RemoveItem(R::id::select_action_menu_cut);
    }

    Boolean bIsShareAvailable;
    mActionHandler->IsShareAvailable(&bIsShareAvailable);
    if (mEditable || !bIsShareAvailable) {
        menu->RemoveItem(R::id::select_action_menu_share);
    }

    Boolean bIsWebSearchAvailable;
    if (mEditable || mIncognito || !(mActionHandler->IsWebSearchAvailable(&bIsWebSearchAvailable), bIsWebSearchAvailable)) {
        menu->RemoveItem(R::id::select_action_menu_web_search);
    }
    if (mIsPasswordType) {
        menu->RemoveItem(R::id::select_action_menu_copy);
        menu->RemoveItem(R::id::select_action_menu_cut);
    }

    return NOERROR;
}

Boolean SelectActionModeCallback::CanPaste()
{
    AutoPtr<IClipboardManager> clipMgr;
    AutoPtr<IInterface> obj;
    GetContext()->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&obj);
    clipMgr = IClipboardManager::Probe(obj);
    Boolean bFlag;
    clipMgr->HasPrimaryClip(&bFlag);
    return bFlag;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
