
#ifndef __ELASTOS_DROID_OS_SYSTEMPROPERTIES_H
#define __ELASTOS_DROID_OS_SYSTEMPROPERTIES_H

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Gives access to the system properties store.  The system properties
 * store contains a list of string key-value pairs.
 *
 * {@hide}
 */
class SystemProperties
{
public:
    /**
     * Get the value for the given key.
     * @return an empty string if the key isn't found
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    static CARAPI Get(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    /**
     * Get the value for the given key.
     * @return if the key isn't found, return def if it isn't null, or an empty string otherwise
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    static CARAPI Get(
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
    static CARAPI GetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 def,
        /* [out] */ Int32* result);

    /**
     * Get the value for the given key, and return as a long.
     * @param key the key to lookup
     * @param def a default value to return
     * @return the key parsed as a long, or def if the key isn't found or
     *         cannot be parsed
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    static CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 def,
        /* [out] */ Int64* result);

    /**
     * Get the value for the given key, returned as a boolean.
     * Values 'n', 'no', '0', 'false' or 'off' are considered false.
     * Values 'y', 'yes', '1', 'true' or 'on' are considered true.
     * (case insensitive).
     * If the key does not exist, or has any other value, then the default
     * result is returned.
     * @param key the key to lookup
     * @param def a default value to return
     * @return the key parsed as a boolean, or def if the key isn't found or is
     *         not able to be parsed as a boolean.
     * @throws IllegalArgumentException if the key exceeds 32 characters
     */
    static CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean def,
        /* [out] */ Boolean* result);

    /**
     * Set the value for the given key.
     * @throws IllegalArgumentException if the key exceeds 32 characters
     * @throws IllegalArgumentException if the value exceeds 92 characters
     */
    static CARAPI Set(
        /* [in] */ const String& key,
        /* [in] */ const String& val);

    static CARAPI AddChangeCallback(
        /* [in] */ IRunnable* callback);

    static CARAPI_(void) CallChangeCallbacks();

private:
    static CARAPI_(String) NativeGet(
        /* [in] */ const String& key);

    static CARAPI_(String) NativeGet(
        /* [in] */ const String& key,
        /* [in] */ const String& def);

    static CARAPI_(Int32) NativeGetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 def);

    static CARAPI_(Int64) NativeGetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 def);

    static CARAPI_(Boolean) NativeGetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean def);

    static CARAPI_(void) NativeSet(
        /* [in] */ const String& key,
        /* [in] */ const String& val);

    static CARAPI_(void) NativeAddChangeCallback();

public:
    static const Int32 SYSTEM_PROPERTY_NAME_MAX;// = 31;
    static const Int32 SYSTEM_PROPERTY_VALUE_MAX;// = 91;

    static List< AutoPtr<IRunnable> > sChangeCallbacks;
    static Object sChangeCallbacksLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_SYSTEMPROPERTIES_H

