
#ifndef __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__
#define __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__

#include "_Elastos_Droid_Text_CNoCopySpanConcrete.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Convenience equivalent for when you would just want a new Object() for
 * a span but want it to be no-copy.  Use this instead.
 */
CarClass(CNoCopySpanConcrete)
    , public Object
    , public INoCopySpan
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__
