#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORD_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.uicc;
// import android.os.Parcel;
// import android.os.Parcelable;
// import android.telephony.PhoneNumberUtils;
// import android.text.TextUtils;
// import android.telephony.Rlog;
// import java.util.Arrays;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  *
  * Used to load or store ADNs (Abbreviated Dialing Numbers).
  *
  * {@hide}
  *
  */
class AdnRecord
    : public Object
    , public IAdnRecord
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<AdnRecord>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ AdnRecord* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* source,
//            /* [out] */ AdnRecord** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ AdnRecord[]** result);
//
//    private:
//        AdnRecord* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    AdnRecord();

    CARAPI constructor();

    //***** Constructor
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* record);

    CARAPI constructor(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 recordNumber,
        /* [in] */ ArrayOf<Byte>* record);

    CARAPI constructor(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number);

    CARAPI constructor(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ ArrayOf<String>* emails);

    CARAPI constructor(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ ArrayOf<String>* emails,
        /* [in] */ ArrayOf<String>* additionalNumbers);

    CARAPI constructor(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 recordNumber,
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ ArrayOf<String>* emails);

    CARAPI constructor(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 recordNumber,
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ ArrayOf<String>* emails,
        /* [in] */ ArrayOf<String>* additionalNumbers);

    CARAPI constructor(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 recordNumber,
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number);

    //***** Instance Methods
    virtual CARAPI GetAlphaTag(
        /* [out] */ String* result);

    virtual CARAPI GetNumber(
        /* [out] */ String* result);

    virtual CARAPI GetEmails(
        /* [out] */ ArrayOf<String>** result);

    virtual CARAPI SetEmails(
        /* [in] */ ArrayOf<String>* emails);

    virtual CARAPI GetAdditionalNumbers(
        /* [out] */ ArrayOf<String>** result);

    virtual CARAPI SetAdditionalNumbers(
        /* [in] */ ArrayOf<String>* additionalNumbers);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    virtual CARAPI HasExtendedRecord(
        /* [out] */ Boolean* result);

    virtual CARAPI IsEqual(
        /* [in] */ IAdnRecord* adn,
        /* [out] */ Boolean* result);

    virtual CARAPI UpdateAnrEmailArrayHelper(
        /* [in] */ ArrayOf<String>* dest,
        /* [in] */ ArrayOf<String>* src,
        /* [in] */ Int32 fileCount,
        /* [out] */ ArrayOf<String>** result);

    virtual CARAPI UpdateAnrEmailArray(
        /* [in] */ IAdnRecord* adn,
        /* [in] */ Int32 emailFileNum,
        /* [in] */ Int32 anrFileNum);

    //***** Parcelable Implementation
    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Build adn hex byte array based on record size
      * The format of byte array is defined in 51.011 10.5.1
      *
      * @param recordSize is the size X of EF record
      * @return hex byte[recordSize] to be written to EF record
      *          return null for wrong format of dialing number or tag
      */
    virtual CARAPI BuildAdnString(
        /* [in] */ Int32 recordSize,
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * See TS 51.011 10.5.10
      */
    virtual CARAPI AppendExtRecord(
        /* [in] */ ArrayOf<Byte>* extRecord);

    virtual CARAPI GetAnrNumbers(
        /* [out] */ ArrayOf<String>** result);

private:
    /** Helper function for {@link #isEqual}. */
    static CARAPI_(Boolean) StringCompareNullEqualsEmpty(
        /* [in] */ const String& s1,
        /* [in] */ const String& s2);

    /** Help function for ANR/EMAIL array compare. */
    static CARAPI_(Boolean) ArrayCompareNullEqualsEmpty(
        /* [in] */ ArrayOf<String>* s1,
        /* [in] */ ArrayOf<String>* s2);

    //***** Private Methods
    /**
      * alphaTag and number are set to null on invalid format
      */
    CARAPI_(void) ParseRecord(
        /* [in] */ ArrayOf<Byte>* record);

public:
    static const String LOGTAG;
    //***** Instance Variables
    String mAlphaTag;
    String mNumber;
    AutoPtr<ArrayOf<String> > mEmails;
    AutoPtr<ArrayOf<String> > mAdditionalNumbers;
    Int32 mExtRecord;
    Int32 mEfid;
    // or 0 if none
    Int32 mRecordNumber;
    // or 0 if none
    //***** Constants
    // In an ADN record, everything but the alpha identifier
    // is in a footer that's 14 bytes
    static const Int32 FOOTER_SIZE_BYTES = 14;
    // Maximum size of the un-extended number field
    static const Int32 MAX_NUMBER_SIZE_BYTES = 11;
    static const Int32 EXT_RECORD_LENGTH_BYTES = 13;
    static const Int32 EXT_RECORD_TYPE_ADDITIONAL_DATA = 2;
    static const Int32 EXT_RECORD_TYPE_MASK = 3;
    static const Int32 MAX_EXT_CALLED_PARTY_LENGTH = 0xa;
    // ADN offset
    static const Int32 ADN_BCD_NUMBER_LENGTH = 0;
    static const Int32 ADN_TON_AND_NPI = 1;
    static const Int32 ADN_DIALING_NUMBER_START = 2;
    static const Int32 ADN_DIALING_NUMBER_END = 11;
    static const Int32 ADN_CAPABILITY_ID = 12;
    static const Int32 ADN_EXTENSION_ID = 13;
    //***** Static Methods
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<AdnRecord> > CREATOR;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORD_H__

