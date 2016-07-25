
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__

#include "_Elastos_Droid_SystemUI_Qs_Tiles_CDataUsageDetailView.h"
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerDataUsageInfo;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Text::IDecimalFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/**
 * Layout for the data usage detail in quick settings.
 */
CarClass(CDataUsageDetailView)
    , public LinearLayout
    , public IDataUsageDetailView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDataUsageDetailView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Bind(
        /* [in] */ INetworkControllerDataUsageInfo* info);

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(String) FormatBytes(
        /* [in] */ Int64 bytes);

private:
    static const Double KB;
    static const Double MB;
    static const Double GB;

    AutoPtr<IDecimalFormat> FORMAT;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CDATAUSAGEDETAILVIEW_H__
