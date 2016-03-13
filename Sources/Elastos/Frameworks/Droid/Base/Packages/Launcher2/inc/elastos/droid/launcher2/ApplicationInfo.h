#ifndef  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Represents an app in AllAppsView.
 */
class ApplicationInfo
    : public ItemInfo
{
public:
    ApplicationInfo();

    /**
     * Must not hold the Context.
     */
    ApplicationInfo(
        /* [in] */ ILauncherActivityInfo* info,
        /* [in] */ IUserHandle* user,
        /* [in] */ IconCache* iconCache,
        /* [in] */ IHashMap* labelCache);

    ApplicationInfo(
        /* [in] */ ApplicationInfo* info);

    /**
     * Creates the application intent based on a component name and various launch flags.
     * Sets {@link #itemType} to {@link LauncherSettings.BaseLauncherColumns#ITEM_TYPE_APPLICATION}.
     *
     * @param className the class name of the component representing the intent
     * @param launchFlags the launch flags
     */
    CARAPI SetActivity(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 launchFlags);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI_(void) DumpApplicationInfoList(
        /* [in] */ const String& tag,
        /* [in] */ const String& label,
        /* [in] */ IArrayList* list);

    CARAPI_(AutoPtr<ShortcutInfo>) MakeShortcut();

public:
    /**
     * The intent used to start the application.
     */
    AutoPtr<IIntent> mIntent;

    /**
     * A bitmap version of the application icon.
     */
    AutoPtr<IBitmap> mIconBitmap;

    /**
     * The time at which the app was first installed.
     */
    Int64 mFirstInstallTime;

    AutoPtr<IComponentName> mComponentName;

    static const Int32 DOWNLOADED_FLAG;
    static const Int32 UPDATED_SYSTEM_APP_FLAG;

    Int32 mFlags = 0;

private:
    static const String TAG;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__