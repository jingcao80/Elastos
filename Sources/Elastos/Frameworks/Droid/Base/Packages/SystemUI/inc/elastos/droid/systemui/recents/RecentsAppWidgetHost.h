
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"

using Elastos::Droid::AppWidget::AppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::SystemUI::Recents::IRecentsAppWidgetHostCallbacks;
using Elastos::Droid::SystemUI::Recents::RecentsConfiguration;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

class RecentsAppWidgetHost
    : public AppWidgetHost
{
public:
    RecentsAppWidgetHost(
        /* [in] */ IContext* context,
        /* [in] */ Int32 hostId);

    CARAPI StartListening(
        /* [in] */ IRecentsAppWidgetHostCallbacks* cb);

    // @Override
    CARAPI StopListening();

    CARAPI IsListening(
        /* [out] */ Boolean* result);
protected:
    // @Override
    CARAPI OnProviderChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidgetInfo);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IRecentsAppWidgetHostCallbacks> mCb;
    AutoPtr<RecentsConfiguration> mConfig;
    Boolean mIsListening;
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__