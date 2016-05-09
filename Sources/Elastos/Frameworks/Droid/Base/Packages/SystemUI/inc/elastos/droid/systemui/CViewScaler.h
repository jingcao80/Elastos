
#ifndef  __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__

#include "_Elastos_Droid_SystemUI_CViewScaler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::SystemUI::StatusBar::IExpandableView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CExpandHelper;

CarClass(CViewScaler)
    , public Object
    , public IViewScaler
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IExpandHelper* host);

    CARAPI SetView(
        /* [in] */ IExpandableView* v);

    CARAPI SetHeight(
        /* [in] */ Float h);

    CARAPI GetHeight(
        /* [out] */ Float* rst);

    CARAPI GetNaturalHeight(
        /* [in] */ Int32 maximum,
        /* [out] */ Int32* naturalHeight);

private:
    CExpandHelper* mHost;
    AutoPtr<IExpandableView> mView;
};

} // SystemUI
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CVIEWSCALER_H__
