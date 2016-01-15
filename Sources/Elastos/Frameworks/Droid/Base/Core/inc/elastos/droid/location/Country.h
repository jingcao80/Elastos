
#ifndef __ELASTOS_DROID_LOCATION_COUNTRY_H__
#define __ELASTOS_DROID_LOCATION_COUNTRY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * This class wraps the country information.
 *
 * @hide
 */
class Country
    : public Object
    , public ICountry
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Country();

    CARAPI constructor();

    /**
     * @param countryIso the ISO 3166-1 two letters country code.
     * @param source where the countryIso came from, could be one of below
     *        values
     *        <p>
     *        <ul>
     *        <li>{@link #COUNTRY_SOURCE_NETWORK}</li>
     *        <li>{@link #COUNTRY_SOURCE_LOCATION}</li>
     *        <li>{@link #COUNTRY_SOURCE_SIM}</li>
     *        <li>{@link #COUNTRY_SOURCE_LOCALE}</li>
     *        </ul>
     */
    CARAPI constructor(
        /* [in] */ const String& countryIso,
        /* [in] */ Int32 source);

    CARAPI constructor(
        /* [in] */ ICountry* country);

    CARAPI constructor(
        /* [in] */ const String& countryIso,
        /* [in] */ Int32 source,
        /* [in] */ Int64 timestamp);

    /**
     * @return the ISO 3166-1 two letters country code
     */
    virtual CARAPI GetCountryIso(
        /* [out] */ String* strOut);

    /**
     * @return where the country code came from, could be one of below values
     *         <p>
     *         <ul>
     *         <li>{@link #COUNTRY_SOURCE_NETWORK}</li>
     *         <li>{@link #COUNTRY_SOURCE_LOCATION}</li>
     *         <li>{@link #COUNTRY_SOURCE_SIM}</li>
     *         <li>{@link #COUNTRY_SOURCE_LOCALE}</li>
     *         </ul>
     */
    virtual CARAPI GetSource(
        /* [out] */ Int32* source);

    /**
     * Returns the time that this object was created (which we assume to be the time that the source
     * was consulted).
     */
    virtual CARAPI GetTimestamp(
        /* [out] */ Int64* timestamp);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    /**
     * Returns true if this {@link Country} is equivalent to the given object. This ignores
     * the timestamp value and just checks for equivalence of countryIso and source values.
     * Returns false otherwise.
     */
    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Compare the specified country to this country object ignoring the source
     * and timestamp fields, return true if the countryIso fields are equal
     *
     * @param country the country to compare
     * @return true if the specified country's countryIso field is equal to this
     *         country's, false otherwise.
     */
    virtual CARAPI EqualsIgnoreSource(
        /* [in] */ ICountry* country,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

private:
    /**
     * The ISO 3166-1 two letters country code.
     */
    String mCountryIso;

    /**
     * Where the country code came from.
     */
    Int32 mSource;

    Int32 mHashCode;

    /**
     * Time that this object was created (which we assume to be the time that the source was
     * consulted). This time is in milliseconds since boot up.
     */
    Int64 mTimestamp;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_COUNTRY_H__
