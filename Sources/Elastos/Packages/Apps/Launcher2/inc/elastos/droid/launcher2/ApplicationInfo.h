#ifndef  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/IconCache.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::ILauncherActivityInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class ShortcutInfo;
/**
 * Represents an app in AllAppsView.
 */
class ApplicationInfo
    : public ItemInfo
    , public IApplicationInfo
{
public:
    CAR_INTERFACE_DECL();

    ApplicationInfo();

    CARAPI constructor();

    /**
     * Must not hold the Context.
     */
    CARAPI constructor(
        /* [in] */ ILauncherActivityInfo* info,
        /* [in] */ IUserHandle* user,
        /* [in] */ IconCache* iconCache,
        /* [in] */ IHashMap* labelCache);

    CARAPI constructor(
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

    Int32 mFlags;

private:
    static const String TAG;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__