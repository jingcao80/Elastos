
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecord.h"

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
    // ==================before translated======================
    // this(0, 0, record);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ ArrayOf<Byte>* record)
{
    // ==================before translated======================
    // this.mEfid = efid;
    // this.mRecordNumber = recordNumber;
    // parseRecord(record);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // this(0, 0, alphaTag, number);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails)
{
    // ==================before translated======================
    // this(0, 0, alphaTag, number, emails);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails,
    /* [in] */ ArrayOf<String>* additionalNumbers)
{
    // ==================before translated======================
    // this(0, 0, alphaTag, number, emails, additionalNumbers);
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ ArrayOf<String>* emails)
{
    // ==================before translated======================
    // this.mEfid = efid;
    // this.mRecordNumber = recordNumber;
    // this.mAlphaTag = alphaTag;
    // this.mNumber = number;
    // this.mEmails = emails;
    // this.mAdditionalNumbers = null;
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
    // ==================before translated======================
    // this.mEfid = efid;
    // this.mRecordNumber = recordNumber;
    // this.mAlphaTag = alphaTag;
    // this.mNumber = number;
    // this.mEmails = emails;
    // this.mAdditionalNumbers = additionalNumbers;
    return NOERROR;
}

ECode AdnRecord::constructor(
    /* [in] */ Int32 efid,
    /* [in] */ Int32 recordNumber,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // this.mEfid = efid;
    // this.mRecordNumber = recordNumber;
    // this.mAlphaTag = alphaTag;
    // this.mNumber = number;
    // this.mEmails = null;
    // this.mAdditionalNumbers = null;
    return NOERROR;
}

ECode AdnRecord::GetAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAlphaTag;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::GetNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNumber;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::GetEmails(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEmails;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::SetEmails(
    /* [in] */ ArrayOf<String>* emails)
{
    // ==================before translated======================
    // this.mEmails = emails;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::GetAdditionalNumbers(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAdditionalNumbers;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::SetAdditionalNumbers(
    /* [in] */ ArrayOf<String>* additionalNumbers)
{
    // ==================before translated======================
    // this.mAdditionalNumbers = additionalNumbers;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "ADN Record '" + mAlphaTag + "' '" + mNumber + " " + mEmails + " "
    //         + mAdditionalNumbers + "'";
    assert(0);
    return NOERROR;
}

ECode AdnRecord::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return TextUtils.isEmpty(mAlphaTag) && TextUtils.isEmpty(mNumber) && mEmails == null
    //         && mAdditionalNumbers == null;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::HasExtendedRecord(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mExtRecord != 0 && mExtRecord != 0xff;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::IsEqual(
    /* [in] */ IAdnRecord* adn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ( stringCompareNullEqualsEmpty(mAlphaTag, adn.mAlphaTag) &&
    //         stringCompareNullEqualsEmpty(mNumber, adn.mNumber) &&
    //         arrayCompareNullEqualsEmpty(mEmails, adn.mEmails)
    //         && arrayCompareNullEqualsEmpty(mAdditionalNumbers, adn.mAdditionalNumbers));
    assert(0);
    return NOERROR;
}

ECode AdnRecord::UpdateAnrEmailArrayHelper(
    /* [in] */ ArrayOf<String>* dest,
    /* [in] */ ArrayOf<String>* src,
    /* [in] */ Int32 fileCount,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (fileCount == 0) {
    //     return null;
    // }
    //
    // // delete insert scenario
    // if (dest == null || src == null) {
    //     return dest;
    // }
    //
    // String[] ref = new String[fileCount];
    // for (int i = 0; i < fileCount; i++) {
    //     ref[i] = "";
    // }
    //
    // // Find common elements and put in the ref
    // // To save SIM_IO
    // for (int i = 0; i < src.length; i++) {
    //     if (TextUtils.isEmpty(src[i])) {
    //         continue;
    //     }
    //     for (int j = 0; j < dest.length; j++) {
    //         if (src[i].equals(dest[j])) {
    //             ref[i] = src[i];
    //             break;
    //         }
    //     }
    // }
    //
    // // fill out none common element into the ""
    // for (int i = 0; i < dest.length; i++) {
    //     if (Arrays.asList(ref).contains(dest[i])) {
    //         continue;
    //     } else {
    //         for (int j = 0; j < ref.length; j++) {
    //             if (TextUtils.isEmpty(ref[j])) {
    //                 ref[j] = dest[i];
    //                 break;
    //             }
    //         }
    //     }
    // }
    // return ref;
    assert(0);
    return NOERROR;
}

ECode AdnRecord::UpdateAnrEmailArray(
    /* [in] */ IAdnRecord* adn,
    /* [in] */ Int32 emailFileNum,
    /* [in] */ Int32 anrFileNum)
{
    // ==================before translated======================
    // mEmails = updateAnrEmailArrayHelper(mEmails, adn.mEmails, emailFileNum);
    // mAdditionalNumbers = updateAnrEmailArrayHelper(mAdditionalNumbers,
    //             adn.mAdditionalNumbers, anrFileNum);
    assert(0);
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
    // ==================before translated======================
    // dest.writeInt(mEfid);
    // dest.writeInt(mRecordNumber);
    // dest.writeString(mAlphaTag);
    // dest.writeString(mNumber);
    // dest.writeStringArray(mEmails);
    // dest.writeStringArray(mAdditionalNumbers);
    assert(0);
    return NOERROR;
}

ECode AdnRecord::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // ==================before translated======================
    // int efid;
    // int recordNumber;
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
    //
    // return new AdnRecord(efid, recordNumber, alphaTag, number, emails, additionalNumbers);

    return NOERROR;
}

ECode AdnRecord::BuildAdnString(
    /* [in] */ Int32 recordSize,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // byte[] bcdNumber;
    // byte[] byteTag;
    // byte[] adnString;
    // int footerOffset = recordSize - FOOTER_SIZE_BYTES;
    //
    // // create an empty record
    // adnString = new byte[recordSize];
    // for (int i = 0; i < recordSize; i++) {
    //     adnString[i] = (byte) 0xFF;
    // }
    //
    // if ((TextUtils.isEmpty(mNumber)) && (TextUtils.isEmpty(mAlphaTag))) {
    //     Rlog.w(LOGTAG, "[buildAdnString] Empty dialing number");
    //     return adnString;   // return the empty record (for delete)
    // } else if ((mNumber != null) && (mNumber.length()
    //         > (ADN_DIALING_NUMBER_END - ADN_DIALING_NUMBER_START + 1) * 2)) {
    //     Rlog.w(LOGTAG,
    //             "[buildAdnString] Max length of dialing number is 20");
    //     return null;
    // } else if (mAlphaTag != null && mAlphaTag.length() > footerOffset) {
    //     Rlog.w(LOGTAG,
    //             "[buildAdnString] Max length of tag is " + footerOffset);
    //     return null;
    // } else {
    //     if (!(TextUtils.isEmpty(mNumber))) {
    //         bcdNumber = PhoneNumberUtils.numberToCalledPartyBCD(mNumber);
    //
    //     System.arraycopy(bcdNumber, 0, adnString,
    //             footerOffset + ADN_TON_AND_NPI, bcdNumber.length);
    //
    //         adnString[footerOffset + ADN_BCD_NUMBER_LENGTH]
    //                 = (byte) (bcdNumber.length);
    //     }
    //     adnString[footerOffset + ADN_CAPABILITY_ID]
    //             = (byte) 0xFF; // Capability Id
    //     adnString[footerOffset + ADN_EXTENSION_ID]
    //             = (byte) 0xFF; // Extension Record Id
    //
    //     if (!TextUtils.isEmpty(mAlphaTag)) {
    //         byteTag = IccUtils.stringToAdnStringField(mAlphaTag);
    //         System.arraycopy(byteTag, 0, adnString, 0, byteTag.length);
    //     }
    //
    //     return adnString;
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecord::AppendExtRecord(
    /* [in] */ ArrayOf<Byte>* extRecord)
{
    // ==================before translated======================
    // try {
    //     if (extRecord.length != EXT_RECORD_LENGTH_BYTES) {
    //         return;
    //     }
    //
    //     if ((extRecord[0] & EXT_RECORD_TYPE_MASK)
    //             != EXT_RECORD_TYPE_ADDITIONAL_DATA) {
    //         return;
    //     }
    //
    //     if ((0xff & extRecord[1]) > MAX_EXT_CALLED_PARTY_LENGTH) {
    //         // invalid or empty record
    //         return;
    //     }
    //
    //     mNumber += PhoneNumberUtils.calledPartyBCDFragmentToString(
    //                                 extRecord, 2, 0xff & extRecord[1]);
    //
    //     // We don't support ext record chaining.
    //
    // } catch (RuntimeException ex) {
    //     Rlog.w(LOGTAG, "Error parsing AdnRecord ext record", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode AdnRecord::GetAnrNumbers(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getAdditionalNumbers();
    assert(0);
    return NOERROR;
}

Boolean AdnRecord::StringCompareNullEqualsEmpty(
    /* [in] */ const String& s1,
    /* [in] */ const String& s2)
{
    // ==================before translated======================
    // if (s1 == s2) {
    //     return true;
    // }
    // if (s1 == null) {
    //     s1 = "";
    // }
    // if (s2 == null) {
    //     s2 = "";
    // }
    // return (s1.equals(s2));
    assert(0);
    return FALSE;
}

Boolean AdnRecord::ArrayCompareNullEqualsEmpty(
    /* [in] */ ArrayOf<String>* s1,
    /* [in] */ ArrayOf<String>* s2)
{
    // ==================before translated======================
    // if (s1 == s2) {
    //     return true;
    // }
    //
    // if (s1 == null) {
    //     s1 = new String[1];
    //     s1[0] = "";
    // }
    //
    // if (s2 == null) {
    //     s2 = new String[1];
    //     s2[0] = "";
    // }
    //
    // for (String str:s1) {
    //     if (TextUtils.isEmpty(str)) {
    //         continue;
    //     } else {
    //         if (Arrays.asList(s2).contains(str)) {
    //             continue;
    //         } else {
    //             return false;
    //         }
    //     }
    // }
    //
    // for (String str:s2) {
    //     if (TextUtils.isEmpty(str)) {
    //         continue;
    //     } else {
    //         if (Arrays.asList(s1).contains(str)) {
    //             continue;
    //         } else {
    //             return false;
    //         }
    //     }
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

void AdnRecord::ParseRecord(
    /* [in] */ ArrayOf<Byte>* record)
{
    // ==================before translated======================
    // try {
    //     mAlphaTag = IccUtils.adnStringFieldToString(
    //                     record, 0, record.length - FOOTER_SIZE_BYTES);
    //
    //     int footerOffset = record.length - FOOTER_SIZE_BYTES;
    //
    //     int numberLength = 0xff & record[footerOffset];
    //
    //     if (numberLength > MAX_NUMBER_SIZE_BYTES) {
    //         // Invalid number length
    //         mNumber = "";
    //         return;
    //     }
    //
    //     // Please note 51.011 10.5.1:
    //     //
    //     // "If the Dialling Number/SSC String does not contain
    //     // a dialling number, e.g. a control string deactivating
    //     // a service, the TON/NPI byte shall be set to 'FF' by
    //     // the ME (see note 2)."
    //
    //     mNumber = PhoneNumberUtils.calledPartyBCDToString(
    //                     record, footerOffset + 1, numberLength);
    //
    //
    //     mExtRecord = 0xff & record[record.length - 1];
    //
    //     mEmails = null;
    //     mAdditionalNumbers = null;
    //
    // } catch (RuntimeException ex) {
    //     Rlog.w(LOGTAG, "Error parsing AdnRecord", ex);
    //     mNumber = "";
    //     mAlphaTag = "";
    //     mEmails = null;
    //     mAdditionalNumbers = null;
    // }
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
