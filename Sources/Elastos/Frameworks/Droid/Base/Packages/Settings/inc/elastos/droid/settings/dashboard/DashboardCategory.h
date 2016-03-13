
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDCATEGORY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDCATEGORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

class DashboardCategory
    : public Object
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    DashboardCategory();

    ~DashboardCategory();

    CARAPI AddTile(
        /* [in] */ DashboardTile* tile);

    CARAPI AddTile(
        /* [in] */ Int32 n,
        /* [in] */ DashboardTile* tile);

    CARAPI RemoveTile(
        /* [in] */ DashboardTile* tile);

    CARAPI RemoveTile(
        /* [in] */ Int32 n);

    CARAPI_(Int32) GetTilesCount();

    CARAPI_(AutoPtr<DashboardTile>) GetTile(
        /* [in] */ Int32 n);

    /**
     * Return the currently set title.  If {@link #titleRes} is set,
     * this resource is loaded from <var>res</var> and returned.  Otherwise
     * {@link #title} is returned.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetTitle(
        /* [in] */ IResources* res);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

public:
    /**
     * Default value for {@link com.android.settings.dashboard.DashboardCategory#id DashboardCategory.id}
     * indicating that no identifier value is set.  All other values (including those below -1)
     * are valid.
     */
    static const Int64 CAT_ID_UNDEFINED = -1;

    /**
     * Identifier for this tile, to correlate with a new list when
     * it is updated.  The default value is
     * {@link com.android.settings.dashboard.DashboardTile#TILE_ID_UNDEFINED}, meaning no id.
     * @attr ref android.R.styleable#PreferenceHeader_id
     */
    Int64 mId = CAT_ID_UNDEFINED;

    /**
     * Resource ID of title of the category that is shown to the user.
     */
    Int32 mTitleRes;

    /**
     * Title of the category that is shown to the user.
     */
    AutoPtr<ICharSequence> mTitle;

    /**
     * List of the category's children
     */
    /*public List<DashboardTile> tiles = new ArrayList<DashboardTile>();*/
    AutoPtr<IList> mTiles;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDCATEGORY_H__

