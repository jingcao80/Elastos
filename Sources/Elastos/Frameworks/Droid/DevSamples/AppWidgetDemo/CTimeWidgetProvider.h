
#ifndef __CTIMEWIDGETPROVIDER_H__
#define __CTIMEWIDGETPROVIDER_H__

#include "_CTimeWidgetProvider.h"
#include "appwidget/AppWidgetProvider.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::AppWidget::IAppWidgetManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {

class CTimeWidgetProvider: public Elastos::Droid::AppWidget::AppWidgetProvider
{
public:
    CTimeWidgetProvider();

    CARAPI OnUpdate(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetManager* appWidgetManager,
        /* [in] */ const ArrayOf<Int32>& appWidgetIds);

    CARAPI OnDeleted(
        /* [in] */ IContext* context,
        /* [in] */ const ArrayOf<Int32>& appWidgetIds);

    CARAPI OnEnabled(
        /* [in] */ IContext* context);

    CARAPI OnDisabled(
        /* [in] */ IContext* context);
};

} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  // __CTIMEWIDGETPROVIDER_H__
