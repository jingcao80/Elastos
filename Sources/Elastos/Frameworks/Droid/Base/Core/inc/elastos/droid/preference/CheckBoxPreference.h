
#ifndef __ELASTOS_DROID_PREFERENCE_CHECKBOXPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_CHECKBOXPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/TwoStatePreference.h"

using Elastos::Droid::Preference::IPreference;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that provides checkbox widget
 * functionality.
 * <p>
 * This preference will store a boolean into the SharedPreferences.
 *
 * @attr ref android.R.styleable#CheckBoxPreference_summaryOff
 * @attr ref android.R.styleable#CheckBoxPreference_summaryOn
 * @attr ref android.R.styleable#CheckBoxPreference_disableDependentsState
 */
class CheckBoxPreference
    : public TwoStatePreference
    , public ICheckBoxPreference
{
public:
    CAR_INTERFACE_DECL()

    CheckBoxPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnBindView(
        /* [in]  */ IView* view);
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CHECKBOXPREFERENCE_H__
