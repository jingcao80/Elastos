
#ifndef __ELASTOS_DROID_TEXT_CSPANNABLESTRING_H__
#define __ELASTOS_DROID_TEXT_CSPANNABLESTRING_H__

#include "_Elastos_Droid_Text_CSpannableString.h"
#include "elastos/droid/text/SpannableStringInternal.h"

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CSpannableString)
    , public SpannableStringInternal
    , public ISpannableString
    , public ICharSequence
    , public IGetChars
    , public ISpannable
    , public ISpanned
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ICharSequence* source);

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI GetLength(
        /* [out] */ Int32* number);

    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* c);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetChars(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 destoff);

    CARAPI GetSpans(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const InterfaceID& type,
        /* [out, callee] */ ArrayOf<IInterface*>** objs);

    CARAPI GetSpanStart(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* start);

    CARAPI GetSpanEnd(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* end);

    CARAPI GetSpanFlags(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* flags);

    CARAPI NextSpanTransition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ const InterfaceID& type,
        /* [out] */ Int32* offset);

    CARAPI SetSpan(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    CARAPI RemoveSpan(
        /* [in] */ IInterface* what);

    static AutoPtr<ISpannableString> ValueOf(
        /* [in] */ ICharSequence* source);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CSPANNABLESTRING_H__
