#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__

#include "_Elastos_Droid_Text_CBidiFormatterHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatterHelper)
    , public Singleton
    , public IBidiFormatterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Factory for creating an instance of BidiFormatter for the default locale directionality.
     *
     */
    CARAPI GetInstance(
        /* [out] */ IBidiFormatter** instance);

    /**
     * Factory for creating an instance of BidiFormatter given the context directionality.
     *
     * @param rtlContext Whether the context directionality is RTL.
     */
    CARAPI GetInstance(
        /* [in] */ Boolean rtlContext,
        /* [out] */ IBidiFormatter** instance);

    /**
     * Factory for creating an instance of BidiFormatter given the context locale.
     *
     * @param locale The context locale.
     */
    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IBidiFormatter** instance);
};

}//namespace Text
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__
