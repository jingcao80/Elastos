#ifndef  __ELASTOS_DROID_LAUNCHER2_ITEMINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_ITEMINFO_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Represents an item in the launcher.
 */
class ItemInfo
    : public Object
{
public:
    ItemInfo();

    ItemInfo(
        /* [in] */ ItemInfo* info);

    /** Returns the package name that the intent will resolve to, or an empty string if
     *  none exists. */
    static CARAPI_(String) GetPackageName(
        /* [in] */ IIntent* intent);

    /**
     * Write the fields of this item to the DB
     *
     * @param context A context object to use for getting a UserManager
     *            instance.
     * @param values
     */
    CARAPI_(void) OnAddToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values);

    CARAPI_(void) UpdateValuesWithCoordinates(
        /* [in] */ IContentValues* values,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) FlattenBitmap(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(void) WriteBitmap(
        /* [in] */ IContentValues* values,
        /* [in] */ IBitmap* bitmap);

    /**
     * It is very important that sub-classes implement this if they contain any references
     * to the activity (anything in the view hierarchy etc.). If not, leaks can result since
     * ItemInfo objects persist across rotation and can hence leak by holding stale references
     * to the old view hierarchy / activity.
     */
    CARAPI_(void) Unbind();

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI_(void) UpdateUser(
        /* [in] */ IIntent* intent);

public:
    /**
     * Intent extra to store the profile. Format: UserHandle
     */
    static const String EXTRA_PROFILE;

    static const Int32 NO_ID;

    /**
     * The id in the settings database for this item
     */
    Int64 mId;

    /**
     * One of {@link LauncherSettings.Favorites#ITEM_TYPE_APPLICATION},
     * {@link LauncherSettings.Favorites#ITEM_TYPE_SHORTCUT},
     * {@link LauncherSettings.Favorites#ITEM_TYPE_FOLDER}, or
     * {@link LauncherSettings.Favorites#ITEM_TYPE_APPWIDGET}.
     */
    Int32 mItemType;

    /**
     * The id of the container that holds this item. For the desktop, this will be
     * {@link LauncherSettings.Favorites#CONTAINER_DESKTOP}. For the all applications folder it
     * will be {@link #NO_ID} (since it is not stored in the settings DB). For user folders
     * it will be the id of the folder.
     */
    Int64 mContainer;

    /**
     * Indicates the screen in which the shortcut appears.
     */
    Int32 mScreen;

    /**
     * Indicates the X position of the associated cell.
     */
    Int32 mCellX;

    /**
     * Indicates the Y position of the associated cell.
     */
    Int32 mCellY;

    /**
     * Indicates the X cell span.
     */
    Int32 mSpanX;

    /**
     * Indicates the Y cell span.
     */
    Int32 mSpanY;

    /**
     * Indicates the minimum X cell span.
     */
    Int32 mMinSpanX;

    /**
     * Indicates the minimum Y cell span.
     */
    Int32 mMinSpanY;

    /**
     * Indicates that this item needs to be updated in the db
     */
    Boolean mRequiresDbUpdate;

    /**
     * Title of the item
     */
    AutoPtr<ICharSequence> mTitle;

    /**
     * Content description for the item.
     */
    AutoPtr<ICharSequence> mContentDescription;

    /**
     * The position of the item in a drag-and-drop operation.
     */
    AutoPtr<ArrayOf<Int32> > mDropPos;

    AutoPtr<IUserHandle> mUser;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_ITEMINFO_H__