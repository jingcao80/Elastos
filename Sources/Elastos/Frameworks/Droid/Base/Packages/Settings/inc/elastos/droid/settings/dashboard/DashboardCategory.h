

package com.android.settings.dashboard;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Os::Parcelable::ICreator;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class DashboardCategory implements Parcelable {

    /**
     * Default value for {@link com.android.settings.dashboard.DashboardCategory#id DashboardCategory.id}
     * indicating that no identifier value is set.  All other values (including those below -1)
     * are valid.
     */
    public static const Int64 CAT_ID_UNDEFINED = -1;

    /**
     * Identifier for this tile, to correlate with a new list when
     * it is updated.  The default value is
     * {@link com.android.settings.dashboard.DashboardTile#TILE_ID_UNDEFINED}, meaning no id.
     * @attr ref android.R.styleable#PreferenceHeader_id
     */
    public Int64 id = CAT_ID_UNDEFINED;

    /**
     * Resource ID of title of the category that is shown to the user.
     */
    public Int32 titleRes;

    /**
     * Title of the category that is shown to the user.
     */
    public CharSequence title;

    /**
     * List of the category's children
     */
    public List<DashboardTile> tiles = new ArrayList<DashboardTile>();


    public DashboardCategory() {
        // Empty
    }

    CARAPI AddTile(DashboardTile tile) {
        tiles->Add(tile);
    }

    CARAPI AddTile(Int32 n, DashboardTile tile) {
        tiles->Add(n, tile);
    }

    CARAPI RemoveTile(DashboardTile tile) {
        tiles->Remove(tile);
    }

    CARAPI RemoveTile(Int32 n) {
        tiles->Remove(n);
    }

    public Int32 GetTilesCount() {
        return tiles->Size();
    }

    public DashboardTile GetTile(Int32 n) {
        return tiles->Get(n);
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

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteInt(titleRes);
        TextUtils->WriteToParcel(title, dest, flags);

        final Int32 count = tiles->Size();
        dest->WriteInt(count);

        for (Int32 n = 0; n < count; n++) {
            DashboardTile tile = tiles->Get(n);
            tile->WriteToParcel(dest, flags);
        }
    }

    CARAPI ReadFromParcel(Parcel in) {
        titleRes = in->ReadInt();
        title = TextUtils.CHAR_SEQUENCE_CREATOR->CreateFromParcel(in);

        final Int32 count = in->ReadInt();

        for (Int32 n = 0; n < count; n++) {
            DashboardTile tile = DashboardTile.CREATOR->CreateFromParcel(in);
            tiles->Add(tile);
        }
    }

    DashboardCategory(Parcel in) {
        ReadFromParcel(in);
    }

    public static const Creator<DashboardCategory> CREATOR = new Creator<DashboardCategory>() {
        public DashboardCategory CreateFromParcel(Parcel source) {
            return new DashboardCategory(source);
        }

        public DashboardCategory[] NewArray(Int32 size) {
            return new DashboardCategory[size];
        }
    };
}
