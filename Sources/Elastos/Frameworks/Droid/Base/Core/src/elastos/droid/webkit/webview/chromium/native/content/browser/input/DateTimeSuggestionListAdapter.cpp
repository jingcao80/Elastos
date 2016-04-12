
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestionListAdapter.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestion.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::ILayoutInflaterHelper;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

DateTimeSuggestionListAdapter::DateTimeSuggestionListAdapter(
    /* [in] */ IContext* context,
    /* [in] */ IList* objects)
    : mContext(context)
{
    ArrayAdapter::constructor(context, R::layout::date_time_suggestion, objects);
}

//@Override
ECode DateTimeSuggestionListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** outView)
{
    VALIDATE_NOT_NULL(outView);

    AutoPtr<IView> layout = convertView;
    if (convertView == NULL) {
        //AutoPtr<ILayoutInflaterHelper> helper;
        //CLayoutInflaterHelper::AcquireSingleton((ILayoutInflaterHelper**)&helper);
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        inflater->Inflate(R::layout::date_time_suggestion, parent, FALSE,
             (IView**)&layout);
    }

    AutoPtr<IView> labelViewView;
    layout->FindViewById(R::id::date_time_suggestion_value, (IView**)&labelViewView);
    ITextView* labelView = ITextView::Probe(labelViewView);
    AutoPtr<IView> sublabelViewView;
    layout->FindViewById(R::id::date_time_suggestion_label, (IView**)&sublabelViewView);
    ITextView* sublabelView = ITextView::Probe(sublabelViewView);

    Int32 count;
    GetCount(&count);
    if (position == count - 1) {
        AutoPtr<ICharSequence> text;
        mContext->GetText(R::string::date_picker_dialog_other_button_label, (ICharSequence**)&text);
        labelView->SetText(text);
        AutoPtr<ICharSequence> emptyCS;
        CString::New(String(""), (ICharSequence**)&emptyCS);
        sublabelView->SetText(emptyCS);
    }
    else {
        AutoPtr<IInterface> obj;
        GetItem(position, (IInterface**)&obj);
        DateTimeSuggestion* dts = (DateTimeSuggestion*)(IObject::Probe(obj));
        AutoPtr<ICharSequence> lvCS;
        CString::New(dts->LocalizedValue(), (ICharSequence**)&lvCS);
        labelView->SetText(lvCS);
        AutoPtr<ICharSequence> svCS;
        CString::New(dts->Label(), (ICharSequence**)&svCS);
        sublabelView->SetText(svCS);
    }

    *outView = layout;

    return NOERROR;
}

//@Override
ECode DateTimeSuggestionListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    GetCount(count);
    *count += 1;
    return NOERROR;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
