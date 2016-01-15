
#include "elastos/droid/content/CClipDataHelper.h"
#include "elastos/droid/webkit/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/native/ui/base/Clipboard.h"
#include "elastos/droid/webkit/native/ui/api/Clipboard_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Droid::Content::CClipDataHelper;
using Elastos::Droid::Content::IClipDataHelper;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Webkit::Base::ApiCompatibilityUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

//=====================================================================
//                              Clipboard
//=====================================================================
Clipboard::Clipboard(
    /* [in] */ IContext* context)
    : mContext(context)
{
    // ==================before translated======================
    // mContext = context;
    // mClipboardManager = (ClipboardManager)
    //         context.getSystemService(Context.CLIPBOARD_SERVICE);

    assert(0);
    AutoPtr<IInterface> tmp;
    context->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&tmp);
    mClipboardManager = IClipboardManager::Probe(tmp);
}

ECode Clipboard::SetText(
    /* [in] */ const String& label,
    /* [in] */ const String& text)
{
    // ==================before translated======================
    // setPrimaryClipNoException(ClipData.newPlainText(label, text));

    assert(0);
    AutoPtr<IClipDataHelper> helper;
    CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);

    AutoPtr<ICharSequence> labelCharSequence;
    CString::New(label, (ICharSequence**)&labelCharSequence);
    AutoPtr<ICharSequence> textCharSequence;
    CString::New(text, (ICharSequence**)&textCharSequence);

    AutoPtr<IClipData> clipData;
    helper->NewPlainText(labelCharSequence, textCharSequence, (IClipData**)&clipData);
    SetPrimaryClipNoException(clipData);
    return NOERROR;
}

ECode Clipboard::SetText(
    /* [in] */ const String& text)
{
    // ==================before translated======================
    // setText(null, text);

    SetText(String(""), text);
    return NOERROR;
}

ECode Clipboard::SetHTMLText(
    /* [in] */ const String& html,
    /* [in] */ const String& label,
    /* [in] */ const String& text)
{
    // ==================before translated======================
    // if (isHTMLClipboardSupported()) {
    //     setPrimaryClipNoException(ClipData.newHtmlText(label, text, html));
    // }

    assert(0);
    if (IsHTMLClipboardSupported()) {
        AutoPtr<IClipDataHelper> helper;
        CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);

        AutoPtr<ICharSequence> labelCharSequence;
        CString::New(label, (ICharSequence**)&labelCharSequence);
        AutoPtr<ICharSequence> textCharSequence;
        CString::New(text, (ICharSequence**)&textCharSequence);

        AutoPtr<IClipData> clipData;
        helper->NewHtmlText(labelCharSequence, textCharSequence, html, (IClipData**)&clipData);
        SetPrimaryClipNoException(clipData);
    }
    return NOERROR;
}

ECode Clipboard::SetHTMLText(
    /* [in] */ const String& html,
    /* [in] */ const String& text)
{
    // ==================before translated======================
    // setHTMLText(html, null, text);

    SetHTMLText(html, String(""), text);
    return NOERROR;
}

AutoPtr<Clipboard> Clipboard::Create(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // return new Clipboard(context);

    AutoPtr<Clipboard> result = new Clipboard(context);
    return result;
}

String Clipboard::GetCoercedText()
{
    // ==================before translated======================
    // final ClipData clip = mClipboardManager.getPrimaryClip();
    // if (clip != null && clip.getItemCount() > 0) {
    //     final CharSequence sequence = clip.getItemAt(0).coerceToText(mContext);
    //     if (sequence != null) {
    //         return sequence.toString();
    //     }
    // }
    // return null;

    assert(0);
    AutoPtr<IClipData> clip;
    mClipboardManager->GetPrimaryClip((IClipData**)&clip);

    Int32 itemCount = 0;
    clip->GetItemCount(&itemCount);
    if (clip != NULL && itemCount > 0) {
        AutoPtr<IClipDataItem> clipDataItem;
        clip->GetItemAt(0, (IClipDataItem**)&clipDataItem);

        AutoPtr<ICharSequence> sequence;
        clipDataItem->CoerceToText(mContext, (ICharSequence**)&sequence);
        if (sequence != NULL) {
            String result;
            sequence->ToString(&result);
            return result;
        }
    }
    return String("");
}

String Clipboard::GetHTMLText()
{
    // ==================before translated======================
    // if (isHTMLClipboardSupported()) {
    //     final ClipData clip = mClipboardManager.getPrimaryClip();
    //     if (clip != null && clip.getItemCount() > 0) {
    //         return clip.getItemAt(0).getHtmlText();
    //     }
    // }
    // return null;

    assert(0);
    if (IsHTMLClipboardSupported()) {
        AutoPtr<IClipData> clip;
        mClipboardManager->GetPrimaryClip((IClipData**)&clip);

        Int32 itemCount;
        clip->GetItemCount(&itemCount);
        if (clip != NULL && itemCount > 0) {
            AutoPtr<IClipDataItem> clipDataItem;
            clip->GetItemAt(0, (IClipDataItem**)&clipDataItem);

            String result;
            clipDataItem->GetHtmlText(&result);
            return result;
        }
    }
    return String("");
}

Boolean Clipboard::IsHTMLClipboardSupported()
{
    // ==================before translated======================
    // return ApiCompatibilityUtils.isHTMLClipboardSupported();

    return ApiCompatibilityUtils::IsHTMLClipboardSupported();
}

ECode Clipboard::SetPrimaryClipNoException(
    /* [in] */ IClipData* clip)
{
    VALIDATE_NOT_NULL(clip);
    // ==================before translated======================
    // try {
    //     mClipboardManager.setPrimaryClip(clip);
    // } catch (Exception ex) {
    //     // Ignore any exceptions here as certain devices have bugs and will fail.
    //     String text = mContext.getString(R.string.copy_to_clipboard_failure_message);
    //     Toast.makeText(mContext, text, Toast.LENGTH_SHORT).show();
    // }

    //try {
        mClipboardManager->SetPrimaryClip(clip);
    //} catch (Exception ex) {
        // Ignore any exceptions here as certain devices have bugs and will fail.
    //    String text = mContext.getString(R.string.copy_to_clipboard_failure_message);
    //    Toast.makeText(mContext, text, Toast.LENGTH_SHORT).show();
    //}
    return NOERROR;
}

AutoPtr<IInterface> Clipboard::Create(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return TO_IINTERFACE(Create(c));
}

String Clipboard::GetCoercedText(
    /* [in] */ IInterface* obj)
{
    Clipboard* mObj = (Clipboard*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("Clipboard", "Clipboard::GetCoercedText, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetCoercedText();
}

String Clipboard::GetHTMLText(
    /* [in] */ IInterface* obj)
{
    Clipboard* mObj = (Clipboard*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("Clipboard", "Clipboard::GetHTMLText, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetHTMLText();
}

void Clipboard::SetText(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& text)
{
    Clipboard* mObj = (Clipboard*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("Clipboard", "Clipboard::SetText, mObj is NULL");
        return;
    }
    mObj->SetText(text);
}

void Clipboard::SetHTMLText(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& html,
    /* [in] */ const String& text)
{
    Clipboard* mObj = (Clipboard*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("Clipboard", "Clipboard::SetHTMLText, mObj is NULL");
        return;
    }
    mObj->SetHTMLText(html, text);
}

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


