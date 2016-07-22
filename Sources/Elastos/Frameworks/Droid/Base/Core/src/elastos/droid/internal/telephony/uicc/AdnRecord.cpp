
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecord.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

////=====================================================================
////                  AdnRecord::InnerParcelableCreator
////=====================================================================
//AdnRecord::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ AdnRecord* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode AdnRecord::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* source,
//    /* [out] */ AdnRecord** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // int efid;
//    // int recordNumber;
//    // String alphaTag;
//    // String number;
//    // String[] emails;
//    // String[] additionalNumbers;
//    // efid = source.readInt();
//    // recordNumber = source.readInt();
//    // alphaTag = source.readString();
//    // number = source.readString();
//    // emails = source.readStringArray();
//    // additionalNumbers = source.readStringArray();
//    //
//    // return new AdnRecord(efid, recordNumber, alphaTag, number, emails, additionalNumbers);
//    assert(0);
//    return NOERROR;
//}
//
//ECode AdnRecord::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ AdnRecord[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new AdnRecord[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                              AdnRecord
//=====================================================================
CAR_INTERFACE_IMPL_2(AdnRecord, Object, IAdnRecord, IParcelable);

const String AdnRecord::LOGTAG("AdnRecord");
const Int32 AdnRecord::FOOTER_SIZE_BYTES;
const Int32 AdnRecord::MAX_NUMBER_SIZE_BYTES;
const Int32 AdnRecord::EXT_RECORD_LENGTH_BYTES;
const Int32 AdnRecord::EXT_RECORD_TYPE_ADDITIONAL_DATA;
const Int32 AdnRecord::EXT_RECORD_TYPE_MASK;
const Int32 AdnRecord::MAX_EXT_CALLED_PARTY_LENGTH;
const Int32 AdnRecord::ADN_BCD_NUMBER_LENGTH;
const Int32 AdnRecord::ADN_TON_AND_NPI;
const Int32 AdnRecord::ADN_DIALING_NUMBER_START;
const Int32 AdnRecord::ADN_DIALING_NUMBER_END;
const Int32 AdnRecord::ADN_CAPABILITY_ID;
const Int32 AdnRecord::ADN_EXTENSION_ID;
//const AutoPtr<IParcelable> AutoPtr< ::Creator<AdnRecord> > AdnRecord::CREATOR = new InnerParcelableCreator(this);

AdnRecord::AdnRecord()
{
}

ECode AdnRecord::constructor()
{
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ ArrayOf<Byte>* record)
{
    return constructor(0, 0, record);
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ ArrayOf<Byte>* record)
{
    mEfid = efid;
    mRecordNumber = recordNumber;
    ParseRecord(record);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    return constructor(0, 0, alphaTag, number);
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails)
{
    return constructor(0, 0, alphaTag, number, emails);
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails,
    /* [in] */ ArrayOf<String>* additionalNumbers)
{
    return constructor(0, 0, alphaTag, number, emails, additionalNumbers);
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails)
{
    mEfid = efid;
    mRecordNumber = recordNumber;
    mAlphaTag = alphaTag;
    mNumber = number;
    mEmails = emails;
    mAdditionalNumbers = NULL;
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails,
    /* [in] */ ArrayOf<String>* additionalNumbers)
{
    mEfid = efid;
    mRecordNumber = recordNumber;
    mAlphaTag = alphaTag;
    mNumber = number;
    mEmails = emails;
    mAdditionalNumbers = additionalNumbers;
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    mEfid = efid;
    mRecordNumber = recordNumber;
    mAlphaTag = alphaTag;
    mNumber = number;
    mEmails = NULL;
    mAdditionalNumbers = NULL;
    return NOERROR;
}

ECode AdnRecord::GetAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAlphaTag;
    return NOERROR;
}

ECode AdnRecord::GetNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumber;
    return NOERROR;
}

ECode AdnRecord::GetEmails(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEmails;
    return NOERROR;
}

ECode AdnRecord::SetEmails(
    /* [in] */ ArrayOf<String>* emails)
{
    mEmails = emails;
    return NOERROR;
}

ECode AdnRecord::GetAdditionalNumbers(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdditionalNumbers;
    return NOERROR;
}

ECode AdnRecord::SetAdditionalNumbers(
    /* [in] */ ArrayOf<String>* additionalNumbers)
{
    mAdditionalNumbers = additionalNumbers;
    return NOERROR;
}

ECode AdnRecord::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // *result = String("ADN Record '") + mAlphaTag
    //         + String("' '") + mNumber
    //         + String(" ") + mEmails + String(" ")
    //         + mAdditionalNumbers + String("'");
    return NOERROR;
}

ECode AdnRecord::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextUtils::IsEmpty(mAlphaTag) && TextUtils::IsEmpty(mNumber) && mEmails == NULL
            && mAdditionalNumbers == NULL;
    return NOERROR;
}

ECode AdnRecord::HasExtendedRecord(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExtRecord != 0 && mExtRecord != 0xff;
    return NOERROR;
}

ECode AdnRecord::IsEqual(
    /* [in] */ IAdnRecord* _adn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<AdnRecord> adn = (AdnRecord*)_adn;
    *result = ( StringCompareNullEqualsEmpty(mAlphaTag, adn->mAlphaTag) &&
            StringCompareNullEqualsEmpty(mNumber, adn->mNumber) &&
            ArrayCompareNullEqualsEmpty(mEmails, adn->mEmails)
            && ArrayCompareNullEqualsEmpty(mAdditionalNumbers, adn->mAdditionalNumbers));
    return NOERROR;
}

ECode AdnRecord::UpdateAnrEmailArrayHelper(
    /* [in] */ ArrayOf<String>* dest,
    /* [in] */ ArrayOf<String>* src,
    /* [in] */ Int32 fileCount,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    if (fileCount == 0) {
        *result = NULL;
        return NOERROR;
    }

    // delete insert scenario
    if (dest == NULL || src == NULL) {
        *result = dest;
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > ref = ArrayOf<String>::Alloc(fileCount);
    for (Int32 i = 0; i < fileCount; i++) {
        (*ref)[i] = "";
    }

    // Find common elements and put in the ref
    // To save SIM_IO
    for (Int32 i = 0; i < src->GetLength(); i++) {
        if (TextUtils::IsEmpty((*src)[i])) {
            continue;
        }
        for (Int32 j = 0; j < dest->GetLength(); j++) {
            if ((*src)[i].Equals((*dest)[j])) {
                (*ref)[i] = (*src)[i];
                break;
            }
        }
    }

    // fill out none common element into the ""
    for (Int32 i = 0; i < dest->GetLength(); i++) {
        AutoPtr<IList> l;
        Arrays::AsList(ref, (IList**)&l);
        Boolean bContain = FALSE;
        l->Contains(CoreUtils::Convert((*dest)[i]), &bContain);
        if (bContain) {
            continue;
        }
        else {
            for (Int32 j = 0; j < ref->GetLength(); j++) {
                if (TextUtils::IsEmpty((*ref)[j])) {
                    (*ref)[j] = (*dest)[i];
                    break;
                }
            }
        }
    }
    *result = ref;
    return NOERROR;
}

ECode AdnRecord::UpdateAnrEmailArray(
    /* [in] */ IAdnRecord* _adn,
    /* [in] */ Int32 emailFileNum,
    /* [in] */ Int32 anrFileNum)
{
    AutoPtr<AdnRecord> adn = (AdnRecord*)_adn;
    UpdateAnrEmailArrayHelper(mEmails, adn->mEmails, emailFileNum, (ArrayOf<String>**)&mEmails);
    UpdateAnrEmailArrayHelper(mAdditionalNumbers,
                adn->mAdditionalNumbers, anrFileNum, (ArrayOf<String>**)&mAdditionalNumbers);
    return NOERROR;
}

//ECode AdnRecord::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode AdnRecord::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteInt32(mEfid);
    dest->WriteInt32(mRecordNumber);
    dest->WriteString(mAlphaTag);
    dest->WriteString(mNumber);
    assert(0 && "TODO");
    // dest->WriteStringArray(mEmails);
    // dest->WriteStringArray(mAdditionalNumbers);
    return NOERROR;
}

ECode AdnRecord::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // Int32 efid;
    // Int32 recordNumber;
    // String alphaTag;
    // String number;
    // String[] emails;
    // String[] additionalNumbers;
    // efid = source.readInt();
    // recordNumber = source.readInt();
    // alphaTag = source.readString();
    // number = source.readString();
    // emails = source.readStringArray();
    // additionalNumbers = source.readStringArray();

    // return new AdnRecord(efid, recordNumber, alphaTag, number, emails, additionalNumbers);

    return NOERROR;
}

ECode AdnRecord::BuildAdnString(
    /* [in] */ Int32 recordSize,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > bcdNumber;
    AutoPtr<ArrayOf<Byte> > byteTag;
    AutoPtr<ArrayOf<Byte> > adnString;
    Int32 footerOffset = recordSize - FOOTER_SIZE_BYTES;

    // create an empty record
    adnString = ArrayOf<Byte>::Alloc(recordSize);
    for (Int32 i = 0; i < recordSize; i++) {
        (*adnString)[i] = (Byte) 0xFF;
    }

    if ((TextUtils::IsEmpty(mNumber)) && (TextUtils::IsEmpty(mAlphaTag))) {
        Logger::W(LOGTAG, String("[buildAdnString] Empty dialing number"));
        *result = adnString;   // return the empty record (for delete)
        return NOERROR;
    }
    else if ((!mNumber.IsNull()) && (mNumber.GetLength()
            > (ADN_DIALING_NUMBER_END - ADN_DIALING_NUMBER_START + 1) * 2)) {
        Logger::W(LOGTAG,
                String("[buildAdnString] Max length of dialing number is 20"));
        *result = NULL;
        return NOERROR;
    }
    else if (!mAlphaTag.IsNull() && mAlphaTag.GetLength() > footerOffset) {
        Logger::W(LOGTAG,
                String("[buildAdnString] Max length of tag is ")
                + StringUtils::ToString(footerOffset));
        *result = NULL;
        return NOERROR;
    }
    else {
        if (!(TextUtils::IsEmpty(mNumber))) {
            AutoPtr<IPhoneNumberUtils> pn;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pn);
            pn->NumberToCalledPartyBCD(mNumber, (ArrayOf<Byte>**)&bcdNumber);

            adnString->Copy(footerOffset + ADN_TON_AND_NPI, bcdNumber, 0, bcdNumber->GetLength());

            (*adnString)[footerOffset + ADN_BCD_NUMBER_LENGTH]
                    = (Byte) (bcdNumber->GetLength());
        }
        (*adnString)[footerOffset + ADN_CAPABILITY_ID]
                = (Byte) 0xFF; // Capability Id
        (*adnString)[footerOffset + ADN_EXTENSION_ID]
                = (Byte) 0xFF; // Extension Record Id

        if (!TextUtils::IsEmpty(mAlphaTag)) {
            AutoPtr<IIccUtils> iccu;
            CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
            iccu->StringToAdnStringField(mAlphaTag, (ArrayOf<Byte>**)&byteTag);
            adnString->Copy(0, byteTag, 0, byteTag->GetLength());
        }

        *result = adnString;
    }
    return NOERROR;
}

ECode AdnRecord::AppendExtRecord(
    /* [in] */ ArrayOf<Byte>* extRecord)
{
    // try {
        if (extRecord->GetLength() != EXT_RECORD_LENGTH_BYTES) {
            return NOERROR;
        }

        if (((*extRecord)[0] & EXT_RECORD_TYPE_MASK)
                != EXT_RECORD_TYPE_ADDITIONAL_DATA) {
            return NOERROR;
        }

        if ((0xff & (*extRecord)[1]) > MAX_EXT_CALLED_PARTY_LENGTH) {
            // invalid or empty record
            return NOERROR;
        }

        AutoPtr<IPhoneNumberUtils> pn;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pn);
        String num;
        pn->CalledPartyBCDFragmentToString(
                        extRecord, 2, 0xff & (*extRecord)[1], &num);
        mNumber += num;

        // We don't support ext record chaining.

    // } catch (RuntimeException ex) {
    //     Logger.w(LOGTAG, "Error parsing AdnRecord ext record", ex);
    // }
    return NOERROR;
}

ECode AdnRecord::GetAnrNumbers(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    return GetAdditionalNumbers(result);
}

Boolean AdnRecord::StringCompareNullEqualsEmpty(
    /* [in] */ const String& s1,
    /* [in] */ const String& s2)
{
    String _s1 = s1;
    String _s2 = s2;
    // if (s1 == s2) {
    //     return TRUE;
    // }
    if (_s1.IsNull()) {
        _s1 = "";
    }
    if (_s2.IsNull()) {
        _s2 = "";
    }
    return (_s1.Equals(_s2));
}

Boolean AdnRecord::ArrayCompareNullEqualsEmpty(
    /* [in] */ ArrayOf<String>* s1,
    /* [in] */ ArrayOf<String>* s2)
{
    if (s1 == s2) {
        return TRUE;
    }

    if (s1 == NULL) {
        s1 = ArrayOf<String>::Alloc(1);
        (*s1)[0] = "";
    }

    if (s2 == NULL) {
        s2 = ArrayOf<String>::Alloc(1);
        (*s2)[0] = "";
    }

    for (Int32 i = 0; i < s1->GetLength(); i++) {
        String str = (*s1)[i];
        if (TextUtils::IsEmpty(str)) {
            continue;
        }
        else {
            AutoPtr<IList> l2;
            Arrays::AsList(s2, (IList**)&l2);
            Boolean bContain = FALSE;
            l2->Contains(CoreUtils::Convert(str), &bContain);
            if (bContain) {
                continue;
            }
            else {
                return FALSE;
            }
        }
    }

    for (Int32 i = 0; i < s2->GetLength(); i++) {
        String str = (*s2)[i];
        if (TextUtils::IsEmpty(str)) {
            continue;
        }
        else {
            AutoPtr<IList> l1;
            Arrays::AsList(s1, (IList**)&l1);
            Boolean bContain = FALSE;
            l1->Contains(CoreUtils::Convert(str), &bContain);
            if (bContain) {
                continue;
            }
            else {
                return FALSE;
            }
        }
    }

    return TRUE;
}

void AdnRecord::ParseRecord(
    /* [in] */ ArrayOf<Byte>* record)
{
    // try {
        AutoPtr<IIccUtils> iccu;
        CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
        iccu->AdnStringFieldToString(
                        record, 0, record->GetLength() - FOOTER_SIZE_BYTES,
                        &mAlphaTag);

        Int32 footerOffset = record->GetLength() - FOOTER_SIZE_BYTES;

        Int32 numberLength = 0xff & (*record)[footerOffset];

        if (numberLength > MAX_NUMBER_SIZE_BYTES) {
            // Invalid number length
            mNumber = "";
            return;
        }

        // Please note 51.011 10.5.1:
        //
        // "If the Dialling Number/SSC String does not contain
        // a dialling number, e.g. a control string deactivating
        // a service, the TON/NPI byte shall be set to 'FF' by
        // the ME (see note 2)."
        AutoPtr<IPhoneNumberUtils> pn;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pn);
        pn->CalledPartyBCDToString(
                    record, footerOffset + 1, numberLength, &mNumber);

        mExtRecord = 0xff & (*record)[record->GetLength() - 1];

        mEmails = NULL;
        mAdditionalNumbers = NULL;

    // } catch (RuntimeException ex) {
    //     Logger.w(LOGTAG, "Error parsing AdnRecord", ex);
    //     mNumber = "";
    //     mAlphaTag = "";
    //     mEmails = NULL;
    //     mAdditionalNumbers = NULL;
    // }
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
