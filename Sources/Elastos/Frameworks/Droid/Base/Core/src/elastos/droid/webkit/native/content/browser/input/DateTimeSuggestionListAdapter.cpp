
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/native/content/browser/input/DateTimeSuggestionListAdapter.h"
// TODO #include "elastos/droid/view/CLayoutInflaterHelper.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::ILayoutInflaterHelper;
// TODO using Elastos::Droid::View::CLayoutInflaterHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

DateTimeSuggestionListAdapter::DateTimeSuggestionListAdapter(
    /* [in] */ IContext* context,
    /* [in] */ IList* objects)
    : mContext(context)
{
    assert(0);
    // TODO
    // super(context, R::layout::date_time_suggestion, objects);
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
        AutoPtr<ILayoutInflaterHelper> helper;
        assert(0);
        // TODO
        // CLayoutInflaterHelper::AcquireSingleton((ILayoutInflaterHelper**)&helper);
        AutoPtr<ILayoutInflater> inflater;
        helper->From(mContext, (ILayoutInflater**)&inflater);
        assert(0);
        // TODO
        // inflater->Inflate(R::layout::date_time_suggestion, parent, false,
        //     (ILayoutInflater**)&layout);
    }

    AutoPtr<ITextView> labelView;
    assert(0);
    // TODO
    // layout->FindViewById(R::id::date_time_suggestion_value, (IView**)&labelView);
    AutoPtr<ITextView> sublabelView;
    assert(0);
    // TODO
    // layout->FindViewById(R::id::date_time_suggestion_label, (IView**)&sublabelView);

    assert(0);
    // TODO
    // if (position == GetCount() - 1) {
    //     AutoPtr<ICharSequence> text;
    //     mContext->GetText(R::string::date_picker_dialog_other_button_label, (ICharSequence**)&text);
    //     labelView->SetText(text);
    //     sublabelView->SetText(String(""));
    // }
    // else {
    //     labelView->SetText(GetItem(position)->LocalizedValue());
    //     sublabelView->SetText(GetItem(position)->Label());
    // }

    *outView = layout;

    return E_NOT_IMPLEMENTED;
}

//@Override
ECode DateTimeSuggestionListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    assert(0);
    // TODO
    // *count = super.getCount() + 1;
    return E_NOT_IMPLEMENTED;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
