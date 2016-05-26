#ifndef __ELASTOS_DROID_TEXT_CSPANNEDSTRING_H__
#define __ELASTOS_DROID_TEXT_CSPANNEDSTRING_H__

#include "_Elastos_Droid_Text_CSpannedString.h"
#include "elastos/droid/text/SpannableStringInternal.h"

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * This is the class for text whose content and markup are immutable.
 * For mutable markup, see {@link SpannableString}; for mutable text,
 * see {@link SpannableStringBuilder}.
 */
CarClass(CSpannedString)
    , public SpannableStringInternal
    , public ISpannedString
    , public ICharSequence
    , public IGetChars
    , public ISpanned
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSpannedString();

    virtual ~CSpannedString();

    CARAPI constructor(
        /* [in] */ ICharSequence* source);

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI GetLength(
        /* [out] */ Int32* number);

    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* c);

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

    static AutoPtr<ISpannedString> ValueOf(
        /* [in] */ ICharSequence* source);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CSPANNEDSTRING_H__
