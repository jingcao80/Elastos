
#ifndef __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__

#include "_Elastos_Droid_Text_Utility_CRfc822TokenHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CRfc822TokenHelper)
    , public Singleton
    , public IRfc822TokenHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI QuoteNameIfNecessary(
        /* [in] */ const String& name,
        /* [out] */ String* ret);

    CARAPI QuoteName(
        /* [in] */ const String& name,
        /* [out] */ String* ret);

    CARAPI QuoteComment(
        /* [in] */ const String& comment,
        /* [out] */ String* ret);
};

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__
