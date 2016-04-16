
#include "elastos/droid/webkit/webview/chromium/native/ui/autofill/AutofillSuggestion.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Autofill {

//=====================================================================
//                          AutofillSuggestion
//=====================================================================
AutofillSuggestion::AutofillSuggestion(
    /* [in] */ const String& name,
    /* [in] */ const String& label,
    /* [in] */ Int32 uniqueId)
    : mLabel(name)
    , mSublabel(label)
    , mUniqueId(uniqueId)
{
    // ==================before translated======================
    // mLabel = name;
    // mSublabel = label;
    // mUniqueId = uniqueId;
}

String AutofillSuggestion::GetLabel()
{
    // ==================before translated======================
    // return mLabel;

    return mLabel;
}

String AutofillSuggestion::GetSublabel()
{
    // ==================before translated======================
    // return mSublabel;

    return mSublabel;
}

Boolean AutofillSuggestion::IsEnabled()
{
    // ==================before translated======================
    // return true;

    return TRUE;
}

Boolean AutofillSuggestion::IsGroupHeader()
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

Int32 AutofillSuggestion::GetUniqueId()
{
    // ==================before translated======================
    // return mUniqueId;

    return mUniqueId;
}

} // namespace Autofill
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


