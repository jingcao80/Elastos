
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

PendingAddShortcutInfo::PendingAddShortcutInfo(
    /* [in] */ IActivityInfo* activityInfo)
    : mShortcutActivityInfo(activityInfo)
{
}

ECode PendingAddShortcutInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String name;
    mShortcutActivityInfo->GetPackageName(&name);
    *str = String("Shortcut: ") + name;
    return NOERROR;
}

PendingAddWidgetInfo::PendingAddWidgetInfo(
    /* [in] */ IAppWidgetProviderInfo* i,
    /* [in] */ const String& dataMimeType,
    /* [in] */ IParcelable* data)
{
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET;
    mInfo = i;
    i->GetProvider((IComponentName**)&mComponentName);
    if (!dataMimeType.IsNull() && data != NULL) {
        mMimeType = dataMimeType;
        mConfigurationData = data;
    }
}

PendingAddWidgetInfo::PendingAddWidgetInfo(
    /* [in] */ PendingAddWidgetInfo* copy)
{
    mInfo = copy->mInfo;
    mBoundWidget = copy->mBoundWidget;
    mMimeType = copy->mMimeType;
    mConfigurationData = copy->mConfigurationData;
    mComponentName = copy->mComponentName;
    mItemType = copy->mItemType;
    mSpanX = copy->mSpanX;
    mSpanY = copy->mSpanY;
    mMinSpanX = copy->mMinSpanX;
    mMinSpanY = copy->mMinSpanY;
    if (copy->mBindOptions == NULL) {
        mBindOptions = NULL;
    }
    else{
        AutoPtr<IInterface> obj;
        copy->mBindOptions->Clone((IInterface**)&obj);
        mBindOptions = IBundle::Probe(obj);
    }
}

ECode PendingAddWidgetInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String name;
    mComponentName->ToShortString(&name);
    *str = String("Widget: ") + name;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos