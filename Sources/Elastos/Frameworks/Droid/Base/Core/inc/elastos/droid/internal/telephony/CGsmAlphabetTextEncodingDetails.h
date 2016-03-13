
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETTEXTENCODINGDETAILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETTEXTENCODINGDETAILS_H__

#include "_Elastos_Droid_Internal_Telephony_CGsmAlphabetTextEncodingDetails.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CGsmAlphabetTextEncodingDetails)
    , public Object
    , public IGsmAlphabetTextEncodingDetails
{
public:
    CGsmAlphabetTextEncodingDetails();

    virtual ~CGsmAlphabetTextEncodingDetails();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI SetMsgCount(
        /* [in] */ Int32 msgCount);

    CARAPI GetMsgCount(
        /* [out] */ Int32* msgCount);

    CARAPI SetCodeUnitCount(
        /* [in] */ Int32 codeUnitCount);

    CARAPI GetCodeUnitCount(
        /* [out] */ Int32* codeUnitCount);

    CARAPI SetCodeUnitsRemaining(
        /* [in] */ Int32 codeUnitsRemaining);

    CARAPI GetCodeUnitsRemaining(
        /* [out] */ Int32* codeUnitsRemaining);

    CARAPI SetCodeUnitSize(
        /* [in] */ Int32 codeUnitSize);

    CARAPI GetCodeUnitSize(
        /* [out] */ Int32* codeUnitSize);

    CARAPI SetLanguageTable(
        /* [in] */ Int32 languageTable);

    CARAPI GetLanguageTable(
        /* [out] */ Int32* languageTable);

    CARAPI SetLanguageShiftTable(
        /* [in] */ Int32 languageShiftTable);

    CARAPI GetLanguageShiftTable(
        /* [out] */ Int32* languageShiftTable);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    /**
     *The number of SMS's required to encode the text.
     */
    Int32 mMsgCount;

    /**
     * The number of code units consumed so far, where code units
     * are basically characters in the encoding -- for example,
     * septets for the standard ASCII and GSM encodings, and 16
     * bits for Unicode.
     */
    Int32 mCodeUnitCount;

    /**
     * How many code units are still available without spilling
     * into an additional message.
     */
    Int32 mCodeUnitsRemaining;

    /**
     * The encoding code unit size (specified using
     * android.telephony.SmsMessage ENCODING_*).
     */
    Int32 mCodeUnitSize;

    /**
     * The GSM national language table to use, or 0 for the default 7-bit alphabet.
     */
    Int32 mLanguageTable;

    /**
     * The GSM national language shift table to use, or 0 for the default 7-bit extension table.
     */
    Int32 mLanguageShiftTable;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABETTEXTENCODINGDETAILS_H__
