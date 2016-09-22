
#ifndef __ELASTOS_SECURITY_CTIMESTAMP_H__
#define __ELASTOS_SECURITY_CTIMESTAMP_H__

#include "_Elastos_Security_CTimestamp.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::IO::ISerializable;
using Elastos::Security::Cert::ICertPath;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Security {

/**
 * {@code Timestamp} represents a signed time stamp. {@code Timestamp} is
 * immutable.
 */
CarClass(CTimestamp)
    , public Object
    , public ITimestamp
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code Timestamp} with the specified {@code
     * timestamp} and the given certificate path.
     *
     * @param timestamp
     *            date and time.
     * @param signerCertPath
     *            the certificate path.
     * @throws NullPointerException
     *             if {@code timestamp} is {@code NULL} or if {@code
     *             signerCertPath} is {@code NULL}.
     */
    CARAPI constructor(
        /* [in] */ IDate* timestamp,
        /* [in] */ ICertPath* signerCertPath);

    /**
     * Compares the specified object with this {@code Timestamp} for equality
     * and returns {@code true} if the specified object is equal, {@code false}
     * otherwise. The given object is equal to this {@code Timestamp}, if it is
     * an instance of {@code Timestamp}, the two timestamps have an equal date
     * and time and their certificate paths are equal.
     *
     * @param obj
     *            object to be compared for equality with this {@code
     *            Timestamp}.
     * @return {@code true} if the specified object is equal to this {@code
     *         Timestamp}, otherwise {@code false}.
     * @see #hashCode
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns the certificate path of this {@code Timestamp}.
     *
     * @return the certificate path of this {@code Timestamp}.
     */
    CARAPI GetSignerCertPath(
        /* [out] */ ICertPath** result);

    /**
     * Returns the date and time of this {@code Timestamp}.
     *
     * @return the date and time of this {@code Timestamp}.
     */
    CARAPI GetTimestamp(
        /* [out] */ IDate** result);

    /**
     * Returns the hash code value for this {@code Timestamp}. Returns the same
     * hash code for {@code Timestamp}s that are equal to each other as
     * required by the general contract of {@link Object#hashCode}.
     *
     * @return the hash code value for this {@code Timestamp}.
     * @see Object#equals(Object)
     * @see Timestamp#equals(Object)
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code Timestamp}.
     *
     * @return a printable representation for this {@code Timestamp}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    // private static final long serialVersionUID = -5502683707821851294L;

    AutoPtr<IDate> mTimestamp;

    AutoPtr<ICertPath> mSignerCertPath;

    // Cached hash
    /*private transient*/ Int32 mHash;
};

}
}

#endif // __ELASTOS_SECURITY_CTIMESTAMP_H__
