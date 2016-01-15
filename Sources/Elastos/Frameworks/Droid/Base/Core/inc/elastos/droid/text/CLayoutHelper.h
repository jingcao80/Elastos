#ifndef __ELASTOS_DROID_TEXT_CLAYOUTHELPER_H__
#define __ELASTOS_DROID_TEXT_CLAYOUTHELPER_H__

#include "_Elastos_Droid_Text_CLayoutHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CLayoutHelper)
    , public Singleton
    , public ILayoutHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return how wide a layout must be in order to display the
     * specified text with one line per paragraph.
     */
    //static
    CARAPI GetDesiredWidth(
        /* [in] */ ICharSequence* source,
        /* [in] */ ITextPaint* paint,
        /* [out] */ Float* ret);

    /**
     * Return how wide a layout must be in order to display the
     * specified text slice with one line per paragraph.
     */
    //static
    CARAPI GetDesiredWidth(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ITextPaint* paint,
        /* [out] */ Float* ret);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_TEXT_CLAYOUTHELPER_H__
