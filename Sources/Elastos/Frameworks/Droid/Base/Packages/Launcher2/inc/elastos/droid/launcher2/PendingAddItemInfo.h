#ifndef  __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class PendingAddItemInfo
    : public ItemInfo
{
public:
    /**
     * The component that will be created.
     */
    AutoPtr<IComponentName> mComponentName;
};

class PendingAddShortcutInfo
    : public PendingAddItemInfo
{
public:
    PendingAddShortcutInfo(
        /* [in] */ IActivityInfo* activityInfo);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IActivityInfo> mShortcutActivityInfo;
};

class PendingAddWidgetInfo
    : public PendingAddItemInfo
{
public:
    PendingAddWidgetInfo(
        /* [in] */ IAppWidgetProviderInfo* i,
        /* [in] */ const String& dataMimeType,
        /* [in] */ IParcelable* data);

    // Copy constructor
    PendingAddWidgetInfo(
        /* [in] */ PendingAddWidgetInfo* copy);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IAppWidgetProviderInfo> mInfo;
    AutoPtr<IAppWidgetHostView> mBoundWidget;
    AutoPtr<IBundle> mBindOptions;

    // Any configuration data that we want to pass to a configuration activity when
    // starting up a widget
    String mMimeType;
    AutoPtr<IParcelable> mConfigurationData;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__