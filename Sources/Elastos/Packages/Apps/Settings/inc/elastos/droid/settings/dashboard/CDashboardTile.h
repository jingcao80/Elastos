//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTITLE_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTITLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Settings_Dashboard_CDashboardTile.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

/**
 * Description of a single dashboard tile that the user can select.
 */
CarClass(CDashboardTile)
    , public Object
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CDashboardTile();

    ~CDashboardTile();

    CARAPI constructor();

    /**
     * Return the currently set title.  If {@link #titleRes} is set,
     * this resource is loaded from <var>res</var> and returned.  Otherwise
     * {@link #title} is returned.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetTitle(
        /* [in] */ IResources* res);

    /**
     * Return the currently set summary.  If {@link #summaryRes} is set,
     * this resource is loaded from <var>res</var> and returned.  Otherwise
     * {@link #summary} is returned.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetSummary(
        /* [in] */ IResources* res);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

public:
    /**
     * Default value for {@link com.android.settings.dashboard.DashboardTile#id DashboardTile.id}
     * indicating that no identifier value is set.  All other values (including those below -1)
     * are valid.
     */
    static const Int64 TILE_ID_UNDEFINED;

    /**
     * Identifier for this tile, to correlate with a new list when
     * it is updated.  The default value is
     * {@link com.android.settings.dashboard.DashboardTile#TILE_ID_UNDEFINED}, meaning no id.
     * @attr ref android.R.styleable#PreferenceHeader_id
     */
    Int64 mId;

    /**
     * Resource ID of title of the tile that is shown to the user.
     * @attr ref android.R.styleable#PreferenceHeader_title
     */
    Int32 mTitleRes;

    /**
     * Title of the tile that is shown to the user.
     * @attr ref android.R.styleable#PreferenceHeader_title
     */
    AutoPtr<ICharSequence> mTitle;

    /**
     * Resource ID of optional summary describing what this tile controls.
     * @attr ref android.R.styleable#PreferenceHeader_summary
     */
    Int32 mSummaryRes;

    /**
     * Optional summary describing what this tile controls.
     * @attr ref android.R.styleable#PreferenceHeader_summary
     */
    AutoPtr<ICharSequence> mSummary;

    /**
     * Optional icon resource to show for this tile.
     * @attr ref android.R.styleable#PreferenceHeader_icon
     */
    Int32 mIconRes;

    /**
     * Full class name of the fragment to display when this tile is
     * selected.
     * @attr ref android.R.styleable#PreferenceHeader_fragment
     */
    String mFragment;

    /**
     * Optional arguments to supply to the fragment when it is
     * instantiated.
     */
    AutoPtr<IBundle> mFragmentArguments;

    /**
     * Intent to launch when the preference is selected.
     */
    AutoPtr<IIntent> mIntent;

    /**
     * Optional additional data for use by subclasses of the activity
     */
    AutoPtr<IBundle> mExtras;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTITLE_H__
