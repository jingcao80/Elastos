#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CLOCKSCREENSHORTCUTSHELPER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CLOCKSCREENSHORTCUTSHELPER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "_Elastos_Droid_Internal_Utility_Cm_CLockscreenShortcutsHelper.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CLockscreenShortcutsHelper)
    , public Object
    , public ILockscreenShortcutsHelper
{
public:
    class Shortcuts
        : public Object
        , public ILockscreenShortcutsHelperShortcuts
    {
    public:
        CAR_INTERFACE_DECL()

        Shortcuts(
            /* [in] */ Int32 index);

        CARAPI GetIndex(
            /* [out] */ Int32* result);

    private:
        static CARAPI_(Boolean) InitStatic();

    public:
        static AutoPtr<CLockscreenShortcutsHelper::Shortcuts> LEFT_SHORTCUT;
        static AutoPtr<CLockscreenShortcutsHelper::Shortcuts> RIGHT_SHORTCUT;

    private:
        Int32 mIndex;
        static Boolean sInit;
    };

    class TargetInfo
        : public Object
    {
    public:
        TargetInfo(
            /* [in] */ IDrawable* icon,
            /* [in] */ IColorFilter* colorFilter,
            /* [in] */ const String& uri);

    public:
        AutoPtr<IDrawable> mIcon;
        AutoPtr<IColorFilter> mColorFilter;
        String mUri;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CLockscreenShortcutsHelper::MyContentObserver")

        MyContentObserver(
            /* [in] */ CLockscreenShortcutsHelper* host)
            : mHost(host)
        {}

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CLockscreenShortcutsHelper* mHost;
    };

public:
    friend class CLockscreenShortcutsHelper::MyContentObserver;

    CLockscreenShortcutsHelper();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILockscreenShortcutsHelperOnChangeListener* listener);

    CARAPI GetDrawablesForTargets(
        /* [out] */ IList** result);

    CARAPI GetDrawableFromSystemUI(
        /* [in] */ const String& name,
        /* [out] */ IDrawable** result);

    CARAPI GetFriendlyNameForUri(
        /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
        /* [out] */ String* result);

    CARAPI IsTargetCustom(
        /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
        /* [out] */ Boolean* result);

    CARAPI IsTargetEmpty(
        /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
        /* [out] */ Boolean* result);

    CARAPI GetIntent(
        /* [in] */ ILockscreenShortcutsHelperShortcuts* shortcut,
        /* [out] */ IIntent** result);

    CARAPI SaveTargets(
        /* [in] */ IArrayList* targets);

private:
    CARAPI_(void) FetchTargets();

    CARAPI_(String) GetFriendlyActivityName(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean labelOnly);

    CARAPI_(String) GetFriendlyShortcutName(
        /* [in] */ IIntent* intent);

private:
    const static String DELIMITER;
    const static String SYSTEM_UI_PKGNAME;
    const static String PHONE_DEFAULT_ICON;
    const static String CAMERA_DEFAULT_ICON;

    AutoPtr<IContext> mContext;
    AutoPtr<IResources> mSystemUiResources;
    AutoPtr<ILockscreenShortcutsHelperOnChangeListener> mListener;
    AutoPtr<IList> mTargetActivities;

    AutoPtr<IContentObserver> mObserver;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CLOCKSCREENSHORTCUTSHELPER_H__
