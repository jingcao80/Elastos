
#ifndef __ELASTOS_DROID_VIEW_CRenderNodeHELPER_H__
#define __ELASTOS_DROID_VIEW_CRenderNodeHELPER_H__

#include "_Elastos_Droid_View_CRenderNodeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CRenderNodeHelper)
    , public Singleton
    , public IRenderNodeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI constructor();

    CARAPI Create(
        /* [in] */ const String& name,
        /* [in] */ IView* owningView,
        /* [out] */ IRenderNode** node);

    CARAPI Adopt(
        /* [in] */ Handle64 nativePtr,
        /* [out] */ IRenderNode** node);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CRenderNodeHELPER_H__
