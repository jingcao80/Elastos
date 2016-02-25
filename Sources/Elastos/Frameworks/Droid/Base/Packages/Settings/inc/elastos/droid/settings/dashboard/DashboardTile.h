

package com.android.settings.dashboard;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Text::ITextUtils;

/**
 * Description of a single dashboard tile that the user can select.
 */
public class DashboardTile implements Parcelable {
    /**
     * Default value for {@link com.android.settings.dashboard.DashboardTile#id DashboardTile.id}
     * indicating that no identifier value is set.  All other values (including those below -1)
     * are valid.
     */
    public static const Int64 TILE_ID_UNDEFINED = -1;

    /**
     * Identifier for this tile, to correlate with a new list when
     * it is updated.  The default value is
     * {@link com.android.settings.dashboard.DashboardTile#TILE_ID_UNDEFINED}, meaning no id.
     * @attr ref android.R.styleable#PreferenceHeader_id
     */
    public Int64 id = TILE_ID_UNDEFINED;

    /**
     * Resource ID of title of the tile that is shown to the user.
     * @attr ref android.R.styleable#PreferenceHeader_title
     */
    public Int32 titleRes;

    /**
     * Title of the tile that is shown to the user.
     * @attr ref android.R.styleable#PreferenceHeader_title
     */
    public CharSequence title;

    /**
     * Resource ID of optional summary describing what this tile controls.
     * @attr ref android.R.styleable#PreferenceHeader_summary
     */
    public Int32 summaryRes;

    /**
     * Optional summary describing what this tile controls.
     * @attr ref android.R.styleable#PreferenceHeader_summary
     */
    public CharSequence summary;

    /**
     * Optional icon resource to show for this tile.
     * @attr ref android.R.styleable#PreferenceHeader_icon
     */
    public Int32 iconRes;

    /**
     * Full class name of the fragment to display when this tile is
     * selected.
     * @attr ref android.R.styleable#PreferenceHeader_fragment
     */
    public String fragment;

    /**
     * Optional arguments to supply to the fragment when it is
     * instantiated.
     */
    public Bundle fragmentArguments;

    /**
     * Intent to launch when the preference is selected.
     */
    public Intent intent;

    /**
     * Optional additional data for use by subclasses of the activity
     */
    public Bundle extras;

    public DashboardTile() {
        // Empty
    }

    /**
     * Return the currently set title.  If {@link #titleRes} is set,
     * this resource is loaded from <var>res</var> and returned.  Otherwise
     * {@link #title} is returned.
     */
    public CharSequence GetTitle(Resources res) {
        if (titleRes != 0) {
            return res->GetText(titleRes);
        }
        return title;
    }

    /**
     * Return the currently set summary.  If {@link #summaryRes} is set,
     * this resource is loaded from <var>res</var> and returned.  Otherwise
     * {@link #summary} is returned.
     */
    public CharSequence GetSummary(Resources res) {
        if (summaryRes != 0) {
            return res->GetText(summaryRes);
        }
        return summary;
    }

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteLong(id);
        dest->WriteInt(titleRes);
        TextUtils->WriteToParcel(title, dest, flags);
        dest->WriteInt(summaryRes);
        TextUtils->WriteToParcel(summary, dest, flags);
        dest->WriteInt(iconRes);
        dest->WriteString(fragment);
        dest->WriteBundle(fragmentArguments);
        if (intent != NULL) {
            dest->WriteInt(1);
            intent->WriteToParcel(dest, flags);
        } else {
            dest->WriteInt(0);
        }
        dest->WriteBundle(extras);
    }

    CARAPI ReadFromParcel(Parcel in) {
        id = in->ReadLong();
        titleRes = in->ReadInt();
        title = TextUtils.CHAR_SEQUENCE_CREATOR->CreateFromParcel(in);
        summaryRes = in->ReadInt();
        summary = TextUtils.CHAR_SEQUENCE_CREATOR->CreateFromParcel(in);
        iconRes = in->ReadInt();
        fragment = in->ReadString();
        fragmentArguments = in->ReadBundle();
        if (in->ReadInt() != 0) {
            intent = Intent.CREATOR->CreateFromParcel(in);
        }
        extras = in->ReadBundle();
    }

    DashboardTile(Parcel in) {
        ReadFromParcel(in);
    }

    public static const Creator<DashboardTile> CREATOR = new Creator<DashboardTile>() {
        public DashboardTile CreateFromParcel(Parcel source) {
            return new DashboardTile(source);
        }
        public DashboardTile[] NewArray(Int32 size) {
            return new DashboardTile[size];
        }
    };
}
