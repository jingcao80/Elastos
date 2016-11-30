
#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSAGEBARPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSAGEBARPREFERENCE_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CUsageBarPreference.h"
#include "elastos/droid/settings/deviceinfo/CPercentageBarChart.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Creates a percentage bar chart inside a preference.
 */
CarClass(CUsageBarPreference)
    , public Elastos::Droid::Preference::Preference
{
public:
    TO_STRING_IMPL("CUsageBarPreference")

    CAR_OBJECT_DECL()

    CUsageBarPreference();

    ~CUsageBarPreference();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI AddEntry(
        /* [in] */ Int32 order,
        /* [in] */ Float percentage,
        /* [in] */ Int32 color);

    CARAPI Commit();

    CARAPI Clear();

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<CPercentageBarChart> mChart;

    AutoPtr<IList> mEntries;// List<PercentageBarChart.Entry> mEntries = Lists->NewArrayList();
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CUSAGEBARPREFERENCE_H__