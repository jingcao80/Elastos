
#ifndef __ELASTOS_DROID_SETTINGS_CWARNEDLISTPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CWARNEDLISTPREFERENCE_H__

#include "_Elastos_Droid_Settings_CWarnedListPreference.h"
#include "elastos/droid/preference/ListPreference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::ListPreference;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CWarnedListPreference)
    , public ListPreference
{
public:
    CAR_OBJECT_DECL()

    CWarnedListPreference();

    ~CWarnedListPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Click();

protected:
    //@Override
    CARAPI OnClick();

};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CWARNEDLISTPREFERENCE_H__