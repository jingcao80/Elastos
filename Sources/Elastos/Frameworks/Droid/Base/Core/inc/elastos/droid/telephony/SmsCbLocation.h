#ifndef __ELASTOS_DROID_TELEPHONY_SMSCBLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_SMSCBLOCATION_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * Represents the location and geographical scope of a cell broadcast message.
  * For GSM/UMTS, the Location Area and Cell ID are set when the broadcast
  * geographical scope is cell wide or Location Area wide. For CDMA, the
  * broadcast geographical scope is always PLMN wide.
  *
  * @hide
  */
class SmsCbLocation
    : public Object
    , public ISmsCbLocation
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<SmsCbLocation>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ SmsCbLocation* owner);
//
//        // @Override
//        CARAPI_(AutoPtr<SmsCbLocation>) CreateFromParcel(
//            /* [in] */ IParcel* in);
//
//        // @Override
//        CARAPI_(AutoPtr< ArrayOf< AutoPtr<SmsCbLocation> > >) NewArray(
//            /* [in] */ Int32 size);
//
//    private:
//        SmsCbLocation* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    /**
      * Construct an empty location object. This is used for some test cases, and for
      * cell broadcasts saved in older versions of the database without location info.
      */
    SmsCbLocation();

    /**
      * Construct a location object for the PLMN. This class is immutable, so
      * the same object can be reused for multiple broadcasts.
      */
    CARAPI constructor(
        /* [in] */ const String& plmn);

    /**
      * Construct a location object for the PLMN, LAC, and Cell ID. This class is immutable, so
      * the same object can be reused for multiple broadcasts.
      */
    CARAPI constructor(
        /* [in] */ const String& plmn,
        /* [in] */ Int32 lac,
        /* [in] */ Int32 cid);

    /**
      * Initialize the object from a Parcel.
      */
    CARAPI constructor(
        /* [in] */ IParcel* in);

    CARAPI constructor();

    /**
      * Returns the MCC/MNC of the network as a String.
      * @return the PLMN identifier (MCC+MNC) as a String
      */
    virtual CARAPI GetPlmn(
        /* [out] */ String* result);

    /**
      * Returns the GSM location area code, or UMTS service area code.
      * @return location area code, -1 if unknown, 0xffff max legal value
      */
    virtual CARAPI GetLac(
        /* [out] */ Int32* result);

    /**
      * Returns the GSM or UMTS cell ID.
      * @return gsm cell id, -1 if unknown, 0xffff max legal value
      */
    virtual CARAPI GetCid(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
      * Test whether this location is within the location area of the specified object.
      *
      * @param area the location area to compare with this location
      * @return true if this location is contained within the specified location area
      */
    virtual CARAPI IsInLocationArea(
        /* [in] */ ISmsCbLocation* area,
        /* [out] */ Boolean* result);

    /**
      * Test whether this location is within the location area of the CellLocation.
      *
      * @param plmn the PLMN to use for comparison
      * @param lac the Location Area (GSM) or Service Area (UMTS) to compare with
      * @param cid the Cell ID to compare with
      * @return true if this location is contained within the specified PLMN, LAC, and Cell ID
      */
    virtual CARAPI IsInLocationArea(
        /* [in*/ const String& plmn,
        /* [in] */ Int32 lac,
        /* [in] */ Int32 cid,
        /* [out] */ Boolean* result);

    /**
      * Flatten this object into a Parcel.
      *
      * @param dest  The Parcel in which the object should be written.
      * @param flags Additional flags about how the object should be written (ignored).
      */
    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Describe the kinds of special objects contained in the marshalled representation.
      * @return a bitmask indicating this Parcelable contains no special objects
      */
    // @Override
    //CARAPI_(Int32) DescribeContents();

private:
    /** The PLMN. Note that this field may be an empty string, but isn't allowed to be null. */
    /*const*/ String mPlmn;
    /*const*/ Int32 mLac;
    /*const*/ Int32 mCid;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SMSCBLOCATION_H__

