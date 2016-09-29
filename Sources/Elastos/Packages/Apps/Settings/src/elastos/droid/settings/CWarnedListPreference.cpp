
#include "elastos/droid/settings/CWarnedListPreference.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_OBJECT_IMPL(CWarnedListPreference)

CWarnedListPreference::CWarnedListPreference()
{}

CWarnedListPreference::~CWarnedListPreference()
{}

ECode CWarnedListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ListPreference::constructor(context, attrs);
}

ECode CWarnedListPreference::OnClick()
{
    // Ignore this until an explicit call to Click()
    return NOERROR;
}

ECode CWarnedListPreference::Click()
{
    return ListPreference::OnClick();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos