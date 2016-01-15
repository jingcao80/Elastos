
#ifndef __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTERHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTERHELPER_H__

#include "_Elastos_Droid_Graphics_CTableMaskFilterHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CTableMaskFilterHelper)
    , public Singleton
    , public ITableMaskFilterHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI CreateClipTable(
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [out] */ ITableMaskFilter** filter);

    CARAPI CreateGammaTable(
        /* [in] */ Float gamma,
        /* [out] */ ITableMaskFilter** filter);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CTABLEMASKFILTERHELPER_H__
