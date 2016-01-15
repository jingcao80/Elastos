
#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__
#define  __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHostViewHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHostViewHelper)
    , public Singleton
    , public IAppWidgetHostViewHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL();
    /**
     * As of ICE_CREAM_SANDWICH we are automatically adding padding to widgets targeting
     * ICE_CREAM_SANDWICH and higher. The new widget design guidelines strongly recommend
     * that widget developers do not add extra padding to their widgets. This will help
     * achieve consistency among widgets.
     *
     * Note: this method is only needed by developers of AppWidgetHosts. The method is provided in
     * order for the AppWidgetHost to account for the automatic padding when computing the number
     * of cells to allocate to a particular widget.
     *
     * @param context the current context
     * @param component the component name of the widget
     * @param padding Rect in which to place the output, if null, a new Rect will be allocated and
     *                returned
     * @return default padding for this widget, in pixels
     */
    CARAPI GetDefaultPaddingForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* component,
        /* [in] */ IRect* padding,
        /* [out] */ IRect** rect);
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__
