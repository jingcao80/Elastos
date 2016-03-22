#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELLHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELLHELPER_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternViewCellHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternViewCellHelper)
    , public Singleton
    , public ILockPatternViewCellHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Of(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [in] */ Byte size,
        /* [out] */ ILockPatternViewCell** cell);

    CARAPI UpdateSize(
        /* [in] */ Byte size);
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELLHELPER_H__
