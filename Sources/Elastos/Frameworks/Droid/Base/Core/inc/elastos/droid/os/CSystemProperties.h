
#ifndef __ELASTOS_DROID_OS_CSYSTEMPROPERTIES_H__
#define __ELASTOS_DROID_OS_CSYSTEMPROPERTIES_H__

#include "_Elastos_Droid_Os_CSystemProperties.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CSystemProperties)
    , public Singleton
    , public ISystemProperties
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the value for the given key.
     * @return an empty string if the key isn't found
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    CARAPI Get(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    /**
     * Get the value for the given key.
     * @return if the key isn't found, return def if it isn't null, or an empty string otherwise
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    CARAPI Get(
        /* [in] */ const String& key,
        /* [in] */ const String& def,
        /* [out] */ String* value);

    /**
     * Get the value for the given key, and return as an integer.
     * @param key the key to lookup
     * @param def a default value to return
     * @return the key parsed as an integer, or def if the key isn't found or
     *         cannot be parsed
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    CARAPI GetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 def,
        /* [out] */ Int32* value);

    /**
     * Get the value for the given key, and return as a long.
     * @param key the key to lookup
     * @param def a default value to return
     * @return the key parsed as a long, or def if the key isn't found or
     *         cannot be parsed
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 def,
        /* [out] */ Int64* value);

    /**
     * Get the value for the given key, returned as a boolean.
     * Values 'n', 'no', '0', 'false' or 'off' are considered false.
     * Values 'y', 'yes', '1', 'true' or 'on' are considered true.
     * (case sensitive).
     * If the key does not exist, or has any other value, then the default
     * result is returned.
     * @param key the key to lookup
     * @param def a default value to return
     * @return the key parsed as a boolean, or def if the key isn't found or is
     *         not able to be parsed as a boolean.
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean def,
        /* [out] */ Boolean* value);

    /**
     * Set the value for the given key.
     * @throws IllegalArgumentException if the key exceeds 32 characters
     * @throws IllegalArgumentException if the value exceeds 92 characters
     */
    CARAPI Set(
        /* [in] */ const String& key,
        /* [in] */ const String& val);

    CARAPI AddChangeCallback(
        /* [in] */ IRunnable* cb);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSYSTEMPROPERTIES_H__
