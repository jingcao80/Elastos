
#ifndef __ELASTOS_DROID_TEXT_CLENGTHFILTER_H__
#define __ELASTOS_DROID_TEXT_CLENGTHFILTER_H__

#include "_Elastos_Droid_Text_CLengthFilter.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CLengthFilter)
    , public Object
    , public ILengthFilter
    , public IInputFilter
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLengthFilter();

    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** cs);

    CARAPI constructor(
        /* [in] */ Int32 max);

    /**
     * @return the maximum length enforced by this input filter
     */
    CARAPI GetMax(
        /* [out] */ Int32* max);
private:
    Int32 mMax;
};

}//namespace Text
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CLENGTHFILTER_H__
