#ifndef __ELASTOS_DROID_TELEPHONY_SMSCBCMASINFO_H__
#define __ELASTOS_DROID_TELEPHONY_SMSCBCMASINFO_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * Contains CMAS warning notification Type 1 elements for a {@link SmsCbMessage}.
  * Supported values for each element are defined in TIA-1149-0-1 (CMAS over CDMA) and
  * 3GPP TS 23.041 (for GSM/UMTS).
  *
  * {@hide}
  */
class SmsCbCmasInfo
    : public ISmsCbCmasInfo
    , public IParcelable
    , public Object
{
//private:
    //class InnerParcelableCreator
    //    : public Object
    //    , public Parcelable::Creator<SmsCbCmasInfo>
    //{
    //public:
    //    InnerParcelableCreator(
    //        /* [in] */ SmsCbCmasInfo* owner);

    //    // @Override
    //    CARAPI_(AutoPtr<SmsCbCmasInfo>) CreateFromParcel(
    //        /* [in] */ IParcel* in);

    //    // @Override
    //    CARAPI_(AutoPtr< ArrayOf< AutoPtr<SmsCbCmasInfo> > >) NewArray(
    //        /* [in] */ Int32 size);

    //private:
    //    SmsCbCmasInfo* mOwner;
    //};

public:
    CAR_INTERFACE_DECL();

    SmsCbCmasInfo();

    /** Create a new SmsCbCmasInfo object with the specified values. */
    SmsCbCmasInfo(
        /* [in] */ Int32 messageClass,
        /* [in] */ Int32 category,
        /* [in] */ Int32 responseType,
        /* [in] */ Int32 severity,
        /* [in] */ Int32 urgency,
        /* [in] */ Int32 certainty);

    /** Create a new SmsCbCmasInfo object from a Parcel. */
    SmsCbCmasInfo(
        /* [in] */ IParcel* in);

    CARAPI constructor();
    /**
      * Flatten this object into a Parcel.
      *
      * @param dest  The Parcel in which the object should be written.
      * @param flags Additional flags about how the object should be written (ignored).
      */
    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
      * Returns the CMAS message class, e.g. {@link #CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT}.
      * @return one of the {@code CMAS_CLASS} values
      */
    virtual CARAPI GetMessageClass(
        /* [out] */ Int32* result);

    /**
      * Returns the CMAS category, e.g. {@link #CMAS_CATEGORY_GEO}.
      * @return one of the {@code CMAS_CATEGORY} values
      */
    virtual CARAPI GetCategory(
        /* [out] */ Int32* result);

    /**
      * Returns the CMAS response type, e.g. {@link #CMAS_RESPONSE_TYPE_SHELTER}.
      * @return one of the {@code CMAS_RESPONSE_TYPE} values
      */
    virtual CARAPI GetResponseType(
        /* [out] */ Int32* result);

    /**
      * Returns the CMAS severity, e.g. {@link #CMAS_SEVERITY_EXTREME}.
      * @return one of the {@code CMAS_SEVERITY} values
      */
    virtual CARAPI GetSeverity(
        /* [out] */ Int32* result);

    /**
      * Returns the CMAS urgency, e.g. {@link #CMAS_URGENCY_IMMEDIATE}.
      * @return one of the {@code CMAS_URGENCY} values
      */
    virtual CARAPI GetUrgency(
        /* [out] */ Int32* result);

    /**
      * Returns the CMAS certainty, e.g. {@link #CMAS_CERTAINTY_OBSERVED}.
      * @return one of the {@code CMAS_CERTAINTY} values
      */
    virtual CARAPI GetCertainty(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
      * Describe the kinds of special objects contained in the marshalled representation.
      * @return a bitmask indicating this Parcelable contains no special objects
      */
    // @Override
    //CARAPI_(Int32) DescribeContents();

private:
    /** CMAS message class. */
    /*const*/ Int32 mMessageClass;
    /** CMAS category. */
    /*const*/ Int32 mCategory;
    /** CMAS response type. */
    /*const*/ Int32 mResponseType;
    /** CMAS severity. */
    /*const*/ Int32 mSeverity;
    /** CMAS urgency. */
    /*const*/ Int32 mUrgency;
    /** CMAS certainty. */
    /*const*/ Int32 mCertainty;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SMSCBCMASINFO_H__
