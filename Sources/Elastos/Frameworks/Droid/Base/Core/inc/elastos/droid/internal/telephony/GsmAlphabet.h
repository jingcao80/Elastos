
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSMALPHABET_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSMALPHABET_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class GsmAlphabet
{
private:
    class LanguagePairCount
        : public Object
    {
    public:
        LanguagePairCount(
            /* [in] */ Int32 code);

    public:
        Int32 mLanguageCode;
        AutoPtr<ArrayOf<Int32> > mSeptetCounts;
        AutoPtr<ArrayOf<Int32> > mUnencodableCounts;
    };

public:
    friend class GsmAlphabet::LanguagePairCount;

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

    static CARAPI IsGsmSeptets(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* result);

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

private:
    static CARAPI PackSmsChar(
        /* [in] */ ArrayOf<Byte>* packedChars,
        /* [in] */ Int32 bitOffset,
        /* [in] */ Int32 value);

    static CARAPI EnableCountrySpecificEncodings();

private:
    static const String TAG;
    /** Reverse mapping from Unicode characters to indexes into language tables. */
    static AutoPtr<ArrayOf<ISparseInt32Array*> > sCharsToGsmTables;

    /** Reverse mapping from Unicode characters to indexes into language shift tables. */
    static AutoPtr<ArrayOf<ISparseInt32Array*> > sCharsToShiftTables;

    /** OEM configured list of enabled national language single shift tables for encoding. */
    static AutoPtr<ArrayOf<Int32> > sEnabledSingleShiftTables;

    /** OEM configured list of enabled national language locking shift tables for encoding. */
    static AutoPtr<ArrayOf<Int32> > sEnabledLockingShiftTables;

    /** Highest language code to include in array of single shift counters. */
    static Int32 sHighestEnabledSingleShiftCode;

    /** Flag to bypass check for country-specific overlays (for test cases only). */
    static Boolean sDisableCountryEncodingCheck;

    static AutoPtr< ArrayOf<String> > sLanguageTables;
    static AutoPtr< ArrayOf<String> > sLanguageShiftTables;

    static Boolean sIsInitedStaticVarOnce;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSMALPHABET_H__
