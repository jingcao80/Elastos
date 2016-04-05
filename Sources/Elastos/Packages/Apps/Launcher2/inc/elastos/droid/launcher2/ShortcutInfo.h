#ifndef  __ELASTOS_DROID_LAUNCHER2_SHORTCUTINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_SHORTCUTINFO_H__

#include "_Launcher2.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/IconCache.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntentShortcutIconResource;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class ApplicationInfo;
/**
 * Represents a launchable icon on the workspaces and in folders.
 */
class ShortcutInfo
    : public ItemInfo
    , public IShortcutInfo
{
public:
    CAR_INTERFACE_DECL();

    ShortcutInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ShortcutInfo* info);

    /** TODO: Remove this.  It's only called by ApplicationInfo.makeShortcut. */
    CARAPI constructor(
        /* [in] */ ApplicationInfo* info);

    CARAPI_(void) SetIcon(
        /* [in] */ IBitmap* b);

    CARAPI_(AutoPtr<IBitmap>) GetIcon(
        /* [in] */ IconCache* iconCache);

    CARAPI_(void) UpdateIcon(
        /* [in] */ IconCache* iconCache);

    /**
     * Creates the application intent based on a component name and various launch flags.
     * Sets {@link #itemType} to {@link LauncherSettings.BaseLauncherColumns#ITEM_TYPE_APPLICATION}.
     *
     * @param className the class name of the component representing the intent
     * @param launchFlags the launch flags
     */
    CARAPI_(void) SetActivity(
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI_(void) OnAddToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI_(void) DumpShortcutInfoList(
        /* [in] */ const String& tag,
        /* [in] */ const String& label,
        /* [in] */ IArrayList* list);

public:
    /**
     * The intent used to start the application.
     */
    AutoPtr<IIntent> mIntent;

    /**
     * Indicates whether the icon comes from an application's resource (if false)
     * or from a custom Bitmap (if true.)
     */
    Boolean mCustomIcon;

    /**
     * Indicates whether we're using the default fallback icon instead of something from the
     * app.
     */
    Boolean mUsingFallbackIcon;

    /**
     * If isShortcut=true and customIcon=false, this contains a reference to the
     * shortcut icon as an application's resource.
     */
    AutoPtr<IIntentShortcutIconResource> mIconResource;

private:
    /**
     * The application icon.
     */
    AutoPtr<IBitmap> mIcon;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SHORTCUTINFO_H__