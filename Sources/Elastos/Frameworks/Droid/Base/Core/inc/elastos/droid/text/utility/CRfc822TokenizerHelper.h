
#ifndef __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZERHELPER_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZERHELPER_H__

#include "_Elastos_Droid_Text_Utility_CRfc822TokenizerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CRfc822TokenizerHelper)
    , public Singleton
    , public IRfc822TokenizerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Tokenize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ICollection* ret);

    CARAPI Tokenize(
        /* [in] */ ICharSequence* text,
        /* [out, callee] */ ArrayOf<IRfc822Token*>** ret);
};

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZERHELPER_H__
