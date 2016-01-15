
#ifndef __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKEN_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKEN_H__

#include "_Elastos_Droid_Text_Utility_CRfc822Token.h"
#include "elastos/droid/text/utility/Rfc822Token.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CRfc822Token)
    , public Rfc822Token
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKEN_H__
