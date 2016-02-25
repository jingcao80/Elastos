
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CFIXEDSIZEDRAWABLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CFIXEDSIZEDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CFixedSizeDrawable.h"

#include "elastos/droid/systemui/statusbar/policy/FixedSizeDrawable.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CFixedSizeDrawable), public FixedSizeDrawable
{
public:
    IDRAWABLE_METHODS_DECL()

    CARAPI constructor(
        /* [in] */ IDrawable* that);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI SetFixedBounds(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CFIXEDSIZEDRAWABLE_H__
