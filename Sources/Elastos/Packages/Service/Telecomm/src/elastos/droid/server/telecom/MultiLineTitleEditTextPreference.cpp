
#include "elastos/droid/server/telecom/MultiLineTitleEditTextPreference.h"
#include <Elastos.Droid.View.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return EditTextPreference::constructor(context, attrs, defStyle);
}

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return EditTextPreference::constructor(context, attrs);
 }

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context)
{
    return EditTextPreference::constructor(context);
}

ECode MultiLineTitleEditTextPreference::OnBindView(
    /* [in] */ IView* view)
{
    EditTextPreference::OnBindView(view);
    AutoPtr<IView> titleView;
    view->FindViewById(Elastos::Droid::R::id::title, (IView**)&titleView);
    AutoPtr<ITextView> textView = ITextView::Probe(titleView);
    if (textView != NULL) {
        textView->SetSingleLine(FALSE);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
