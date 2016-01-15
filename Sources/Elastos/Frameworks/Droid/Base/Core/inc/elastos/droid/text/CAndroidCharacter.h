
#ifndef __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__
#define __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__

#include "_Elastos_Droid_Text_CAndroidCharacter.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CAndroidCharacter)
    , public Singleton
    , public IAndroidCharacter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDirectionalities(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ ArrayOf<Byte>* dest,
        /* [in] */ Int32 count);

    CARAPI GetEastAsianWidth(
        /* [in] */ Char32 input,
        /* [out] */ Int32* result);

    CARAPI GetEastAsianWidths(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Byte>* dest);

    CARAPI Mirror(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [out] */ Boolean* result);

    CARAPI GetMirror(
        /* [in] */ Char32 ch,
        /* [out] */ Char32* result);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__
