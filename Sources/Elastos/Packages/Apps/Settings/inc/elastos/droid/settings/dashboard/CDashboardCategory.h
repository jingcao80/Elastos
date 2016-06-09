
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCATEGORY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCATEGORY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_Dashboard_CDashboardCategory.h"
#include "elastos/droid/settings/dashboard/CDashboardTile.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CarClass(CDashboardCategory)
    , public Object
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CDashboardCategory();

    ~CDashboardCategory();

    CARAPI constructor();

    CARAPI AddTile(
        /* [in] */ CDashboardTile* tile);

    CARAPI AddTile(
        /* [in] */ Int32 n,
        /* [in] */ CDashboardTile* tile);

    CARAPI RemoveTile(
        /* [in] */ CDashboardTile* tile);

    CARAPI RemoveTile(
        /* [in] */ Int32 n);

    CARAPI_(Int32) GetTilesCount();

    CARAPI_(AutoPtr<CDashboardTile>) GetTile(
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
    static const Int64 CAT_ID_UNDEFINED;

    /**
     * Identifier for this tile, to correlate with a new list when
     * it is updated.  The default value is
     * {@link com.android.settings.dashboard.DashboardTile#TILE_ID_UNDEFINED}, meaning no id.
     * @attr ref android.R.styleable#PreferenceHeader_id
     */
    Int64 mId;

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
    AutoPtr<IList> mTiles;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCATEGORY_H__
