
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/location/CRadioButtonPreference.h"
#include "../R.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

CAR_INTERFACE_IMPL(CRadioButtonPreference, CheckBoxPreference, IRadioButtonPreference)

CAR_OBJECT_IMPL(CRadioButtonPreference)

CRadioButtonPreference::CRadioButtonPreference()
{}

CRadioButtonPreference::~CRadioButtonPreference()
{}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    CheckBoxPreference::constructor(context, attrs, defStyle);
    return SetWidgetLayoutResource(R::layout::preference_widget_radiobutton);
}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, Elastos::Droid::R::attr::checkBoxPreferenceStyle);
}

ECode CRadioButtonPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CRadioButtonPreference::SetOnClickListener(
    /* [in] */ IRadioButtonPreferenceOnClickListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CRadioButtonPreference::OnClick()
{
    if (mListener != NULL) {
        mListener->OnRadioButtonClicked(this);
    }
    return NOERROR;
}

ECode CRadioButtonPreference::OnBindView(
    /* [in] */ IView* view)
{
    CheckBoxPreference::OnBindView(view);

    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    ITextView* title = ITextView::Probe(tmp);
    if (title != NULL) {
        title->SetSingleLine(FALSE);
        title->SetMaxLines(3);
    }
    return NOERROR;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos