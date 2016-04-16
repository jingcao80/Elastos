#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERSETTINGS_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERSETTINGS_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IBaseColumns;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Settings related utilities.
 */
class LauncherSettings
{
public:
    /**
     * Favorites.
     */
    class Favorites
        : public Object
        , public ILauncherSettingsBaseLauncherColumns
        , public IBaseColumns
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * The content:// style URL for a given row, identified by its id.
         *
         * @param id The row id.
         * @param notify True to send a notification is the content changes.
         *
         * @return The unique content URL for the specified row.
         */
        static CARAPI GetContentUri(
            /* [in] */ Int64 id,
            /* [in] */ Boolean notify,
            /* [out] */ IUri** uri);

    public:
        /**
         * The content:// style URL for this table
         */
        static AutoPtr<IUri> CONTENT_URI;

        /**
         * The content:// style URL for this table. When this Uri is used, no notification is
         * sent if the content changes.
         */
        static AutoPtr<IUri> CONTENT_URI_NO_NOTIFICATION;

        /**
         * The container holding the favorite
         * <P>Type: INTEGER</P>
         */
        static const String CONTAINER;

        /**
         * The icon is a resource identified by a package name and an integer id.
         */
        static const Int32 CONTAINER_DESKTOP = -100;
        static const Int32 CONTAINER_HOTSEAT = -101;

        /**
         * The screen holding the favorite (if container is CONTAINER_DESKTOP)
         * <P>Type: INTEGER</P>
         */
        static const String SCREEN;

        /**
         * The X coordinate of the cell holding the favorite
         * (if container is CONTAINER_HOTSEAT or CONTAINER_HOTSEAT)
         * <P>Type: INTEGER</P>
         */
        static const String CELLX;

        /**
         * The Y coordinate of the cell holding the favorite
         * (if container is CONTAINER_DESKTOP)
         * <P>Type: INTEGER</P>
         */
        static const String CELLY;

        /**
         * The X span of the cell holding the favorite
         * <P>Type: INTEGER</P>
         */
        static const String SPANX;

        /**
         * The Y span of the cell holding the favorite
         * <P>Type: INTEGER</P>
         */
        static const String SPANY;

        /**
         * The profile id of the item in the cell.
         * <P>
         * Type: INTEGER
         * </P>
         */
        static const String PROFILE_ID;

        /**
         * The favorite is a user created folder
         */
        static const Int32 ITEM_TYPE_FOLDER = 2;

        /**
        * The favorite is a live folder
        *
        * Note: live folders can no longer be added to Launcher, and any live folders which
        * exist within the launcher database will be ignored when loading.  That said, these
        * entries in the database may still exist, and are not automatically stripped.
        */
        static const Int32 ITEM_TYPE_LIVE_FOLDER;

        /**
         * The favorite is a widget
         */
        static const Int32 ITEM_TYPE_APPWIDGET = 4;

        /**
         * The favorite is a clock
         */
        static const Int32 ITEM_TYPE_WIDGET_CLOCK;

        /**
         * The favorite is a search widget
         */
        static const Int32 ITEM_TYPE_WIDGET_SEARCH;

        /**
         * The favorite is a photo frame
         */
        static const Int32 ITEM_TYPE_WIDGET_PHOTO_FRAME;

        /**
         * The appWidgetId of the widget
         *
         * <P>Type: INTEGER</P>
         */
        static const String APPWIDGET_ID;

        /**
         * Indicates whether this favorite is an application-created shortcut or not.
         * If the value is 0, the favorite is not an application-created shortcut, if the
         * value is 1, it is an application-created shortcut.
         * <P>Type: INTEGER</P>
         */
        //@Deprecated
        static const String IS_SHORTCUT;

        /**
         * The URI associated with the favorite. It is used, for instance, by
         * live folders to find the content provider.
         * <P>Type: TEXT</P>
         */
        static const String URI;

        /**
         * The display mode if the item is a live folder.
         * <P>Type: INTEGER</P>
         *
         * @see android.provider.LiveFolders#DISPLAY_MODE_GRID
         * @see android.provider.LiveFolders#DISPLAY_MODE_LIST
         */
        static const String DISPLAY_MODE;
    };
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERSETTINGS_H__