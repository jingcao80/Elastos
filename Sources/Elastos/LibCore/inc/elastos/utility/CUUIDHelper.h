#ifndef __ELASTOS_UTILITY_CUUIDHELPER_H__
#define __ELASTOS_UTILITY_CUUIDHELPER_H__

#include "_Elastos_Utility_CUUIDHelper.h"
#include "Singleton.h"


namespace Elastos {
namespace Utility {

CarClass(CUUIDHelper)
    , public Singleton
    , public IUUIDHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * <p>
     * Generates a variant 2, version 4 (randomly generated number) UUID as per
     * <a href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @return an UUID instance.
     */
    CARAPI RandomUUID(
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Generates a variant 2, version 3 (name-based, MD5-hashed) UUID as per <a
     * href="http://www.ietf.org/rfc/rfc4122.txt">RFC 4122</a>.
     *
     * @param name
     *            the name used as byte array to create an UUID.
     * @return an UUID instance.
     */
    CARAPI NameUUIDFromBytes(
        /* [in] */ ArrayOf<Byte>* name,
        /* [out] */ IUUID** uuid);

    /**
     * <p>
     * Parses a UUID string with the format defined by {@link #toString()}.
     *
     * @param uuid
     *            the UUID string to parse.
     * @return an UUID instance.
     * @throws NullPointerException
     *             if {@code uuid} is {@code null}.
     * @throws IllegalArgumentException
     *             if {@code uuid} is not formatted correctly.
     */
    CARAPI FromString(
        /* [in] */ const String& name,
        /* [out] */ IUUID** uuid);
};

} // namespace Utility
} // namespace Elastos

#endif //__CUUIDHELPER_H__
