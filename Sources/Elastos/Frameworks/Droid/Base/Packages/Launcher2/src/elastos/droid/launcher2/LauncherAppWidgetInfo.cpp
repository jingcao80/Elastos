
#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int32 LauncherAppWidgetInfo::NO_ID = -1;

CAR_INTERFACE_IMPL(LauncherAppWidgetInfo, ItemInfo, ILauncherAppWidgetInfo);

LauncherAppWidgetInfo::LauncherAppWidgetInfo(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* providerName)
    : mAppWidgetId(NO_ID)
    , mMinWidth(-1)
    , mMinHeight(-1)
    , mHasNotifiedInitialWidgetSizeChanged(FALSE)
{
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET;
    mAppWidgetId = appWidgetId;
    mProviderName = providerName;

    // Since the widget isn't instantiated yet, we don't know these values. Set them to -1
    // to indicate that they should be calculated based on the layout and minWidth/minHeight
    mSpanX = -1;
    mSpanY = -1;
}

ECode LauncherAppWidgetInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    ItemInfo::OnAddToDatabase(context, values);
    return values->Put(LauncherSettings::Favorites::APPWIDGET_ID, mAppWidgetId);
}

ECode LauncherAppWidgetInfo::OnBindAppWidget(
    /* [in] */ ILauncher* launcher)
{
    if (!mHasNotifiedInitialWidgetSizeChanged) {
        return NotifyWidgetSizeChanged(launcher);
    }
    return NOERROR;
}

ECode LauncherAppWidgetInfo::NotifyWidgetSizeChanged(
    /* [in] */ ILauncher* launcher)
{
    AppWidgetResizeFrame->updateWidgetSizeRanges(mHostView, launcher, mSpanX, mSpanY);
    mHasNotifiedInitialWidgetSizeChanged = TRUE;
    return NOERROR;
}

ECode LauncherAppWidgetInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "AppWidget(id=";
    sb += mAppWidgetId;
    sb += ")";
    return sb.ToString(str);
}

ECode LauncherAppWidgetInfo::Unbind()
{
    ItemInfo::Unbind();
    mHostView = NULL;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos