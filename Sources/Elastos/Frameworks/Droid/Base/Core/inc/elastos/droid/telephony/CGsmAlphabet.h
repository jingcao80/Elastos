
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABET_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABET_H__

#include "_Elastos_Droid_Internal_Telephony_CGsmAlphabet.h"
#include "elastos/droid/utility/SparseInt32Array.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Utility::SparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CGsmAlphabet)
{
public:

    class TextEncodingDetails
    : public ElRefBase
    , public IGsmAlphabetTextEncodingDetails
    {
    public:

        CAR_INTERFACE_DECL();

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

    static CARAPI CharToGsm(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    static CARAPI CharToGsm(
        /* [in] */ Char32 c,
        /* [in] */ Boolean throwException,
        /* [out] */ Int32* val);

    static CARAPI CharToGsmExtended(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    static CARAPI GsmToChar(
        /* [in] */ Int32 gsmChar,
        /* [out] */ Char32* res);

    static CARAPI GsmExtendedToChar(
        /* [in] */ Int32 gsmChar,
        /* [out] */ Char32* res);

    static CARAPI StringToGsm7BitPackedWithHeader(
        /* [in] */ const String& data,
        /* [in] */ ArrayOf<Byte>* header,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI StringToGsm7BitPackedWithHeader(
        /* [in] */ const String& data,
        /* [in] */ ArrayOf<Byte>* header,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI StringToGsm7BitPacked(
        /* [in] */ const String& data,
        /* [in] */ Int32 startingSeptetOffset,
        /* [in] */ Boolean throwException,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI Gsm7BitPackedToString(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 lengthSeptets,
        /* [out] */ String* res);

    static CARAPI Gsm7BitPackedToString(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 lengthSeptets,
        /* [in] */ Int32 numPaddingBits,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 shiftTable,
        /* [out] */ String* res);

    static CARAPI Gsm8BitUnpackedToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    static CARAPI Gsm8BitUnpackedToString(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ const String& characterset,
        /* [out] */ String* res);

    static CARAPI StringToGsm8BitPacked(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI StringToGsm8BitUnpackedField(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Byte>* dest,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI CountGsmSeptets(
        /* [in] */ Char32 c,
        /* [out] */ Int32* val);

    static CARAPI CountGsmSeptets(
        /* [in] */ Char32 c,
        /* [in] */ Boolean throwsException,
        /* [out] */ Int32* val);

    static CARAPI CountGsmSeptetsUsingTables(
        /* [in] */ ICharSequence* s,
        /* [in] */ Boolean use7bitOnly,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out] */ Int32* val);

    static CARAPI CountGsmSeptets(
        /* [in] */ ICharSequence* s,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** res);

    static CARAPI FindGsmSeptetLimitIndex(
        /* [in] */ const String& s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ Int32 langTable,
        /* [in] */ Int32 langShiftTable,
        /* [out] */ Int32* val);

    static CARAPI SetEnabledSingleShiftTables(
        /* [in] */ ArrayOf<Int32>* tables);

    static CARAPI SetEnabledLockingShiftTables(
        /* [in] */ ArrayOf<Int32>* tables);

    static CARAPI GetEnabledSingleShiftTables(
        /* [out, callee] */ ArrayOf<Int32>** res);

    static CARAPI GetEnabledLockingShiftTables(
        /* [out, callee] */ ArrayOf<Int32>** res);

    static CARAPI_(Boolean) InitStaticVar();

    CARAPI constructor();

    static Boolean sIsInitedStaticVarOnce;

private:

    static CARAPI PackSmsChar(
        /* [in] */ ArrayOf<Byte>* packedChars,
        /* [in] */ Int32 bitOffset,
        /* [in] */ Int32 value);

    static CARAPI EnableCountrySpecificEncodings();

    static const String TAG/* = "GSM"*/;
    /** Reverse mapping from Unicode characters to indexes into language tables. */
    static AutoPtr < ArrayOf< AutoPtr<SparseInt32Array> > > sCharsToGsmTables;

    /** Reverse mapping from Unicode characters to indexes into language shift tables. */
    static AutoPtr < ArrayOf< AutoPtr<SparseInt32Array> > > sCharsToShiftTables;

    /** OEM configured list of enabled national language single shift tables for encoding. */
    static AutoPtr < ArrayOf<Int32> > sEnabledSingleShiftTables;

    /** OEM configured list of enabled national language locking shift tables for encoding. */
    static AutoPtr < ArrayOf<Int32> > sEnabledLockingShiftTables;

    /** Highest language code to include in array of single shift counters. */
    static Int32 sHighestEnabledSingleShiftCode;

    /** Flag to bypass check for country-specific overlays (for test cases only). */
    static Boolean sDisableCountryEncodingCheck/* = FALSE*/;

    static AutoPtr< ArrayOf<String> > sLanguageTables;
    static AutoPtr< ArrayOf<String> > sLanguageShiftTables;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CGSMALPHABET_H__
