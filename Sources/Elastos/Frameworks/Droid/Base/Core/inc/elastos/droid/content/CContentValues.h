#ifndef __ELASTOS_DROID_CONTENT_CCONTENTVALUES_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTVALUES_H__

#include "_Elastos_Droid_Content_CContentValues.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IByte;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Utility::ISet;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * This class is used to store a set of values that the {@link ContentResolver}
 * can process.
 */
CarClass(CContentValues)
    , public Object
    , public IContentValues
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentValues();

    virtual ~CContentValues();

    /**
     * Creates an empty set of values using the default initial size
     */
    CARAPI constructor();

    /**
     * Creates an empty set of values using the given initial size
     *
     * @param size the initial size of the set of values
     */
    CARAPI constructor(
        /* [in] */ Int32 size);

    /**
     * Creates a set of values copied from the given set
     *
     * @param from the values to copy
     */
    CARAPI constructor(
        /* [in] */ IContentValues* from);

public:
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashcode);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ ICharSequence* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Adds all values from the passed in ContentValues.
     *
     * @param other the ContentValues from which to copy
     */
    CARAPI PutAll(
        /* [in] */ IContentValues* contentvalues);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IByte* value);

    CARAPI PutByte(
        /* [in] */ const String& key,
        /* [in] */ Byte value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IInteger16* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Int16 value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IInteger32* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IInteger64* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Int64 value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IFloat* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Float value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IDouble* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Double value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IBoolean* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put
     * @param value the data for the value to put
     */
    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ IArrayOf* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value);

    /**
     * Adds a null value to the set.
     *
     * @param key the name of the value to make null
     */
    CARAPI PutNull(
        /* [in] */ const String& key);

    /**
     * Returns the number of values.
     *
     * @return the number of values
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Remove a single value.
     *
     * @param key the name of the value to remove
     */
    CARAPI Remove(
        /* [in] */ const String& key);

    /**
     * Removes all values.
     */
    CARAPI Clear();

    /**
     * Returns true if this object has the named value.
     *
     * @param key the value to check for
     * @return {@code true} if the value is present, {@code false} otherwise
     */
    CARAPI ContainsKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    /**
     * Gets a value. Valid value types are {@link String}, {@link Boolean}, and
     * {@link Number} implementations.
     *
     * @param key the value to get
     * @return the data for the value
     */
    CARAPI Get(
        /* [in] */ const String& key,
        /* [out] */ IInterface** value);

    /**
     * Gets a value and converts it to a String.
     *
     * @param key the value to get
     * @return the String for the value
     */
    CARAPI GetAsString(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    /**
     * Gets a value and converts it to a Long.
     *
     * @param key the value to get
     * @return the Long value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsInteger64(
        /* [in] */ const String& key,
        /* [out] */ IInteger64** value);

    /**
     * Gets a value and converts it to an Integer.
     *
     * @param key the value to get
     * @return the Integer value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsInteger32(
        /* [in] */ const String& key,
        /* [out] */ IInteger32** value);

    /**
     * Gets a value and converts it to a Short.
     *
     * @param key the value to get
     * @return the Short value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsInteger16(
        /* [in] */ const String& key,
        /* [out] */ IInteger16** value);

    /**
     * Gets a value and converts it to a Byte.
     *
     * @param key the value to get
     * @return the Byte value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsByte(
        /* [in] */ const String& key,
        /* [out] */ IByte** value);

    /**
     * Gets a value and converts it to a Double.
     *
     * @param key the value to get
     * @return the Double value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsDouble(
        /* [in] */ const String& key,
        /* [out] */ IDouble** value);

    /**
     * Gets a value and converts it to a Float.
     *
     * @param key the value to get
     * @return the Float value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsFloat(
        /* [in] */ const String& key,
        /* [out] */ IFloat** value);

    /**
     * Gets a value and converts it to a Boolean.
     *
     * @param key the value to get
     * @return the Boolean value, or null if the value is missing or cannot be converted
     */
    CARAPI GetAsBoolean(
        /* [in] */ const String& key,
        /* [out] */ IBoolean** value);

    /**
     * Gets a value that is a byte array. Note that this method will not convert
     * any other types to byte arrays.
     *
     * @param key the value to get
     * @return the byte[] value, or null is the value is missing or not a byte[]
     */
    CARAPI GetAsByteArray(
        /* [in] */ const String& key,
        /* [out, callee] */ ArrayOf<Byte>** value);

    /**
     * Returns a set of all of the keys and values
     *
     * @return a set of all of the keys and values
     */
    CARAPI GetValueSet(
        /* [out] */ ISet** values);

    /**
     * Returns a set of all of the keys
     *
     * @return a set of all of the keys
     */
    CARAPI GetKeySet(
        /* [out] */ ISet** values);

    /**
     * Unsupported, here until we get proper bulk insert APIs.
     * {@hide}
     */
    CARAPI PutStringArrayList(
        /* [in] */ const String& key,
        /* [in] */ IArrayList* value);

    /**
     * Unsupported, here until we get proper bulk insert APIs.
     * {@hide}
     */
    //@SuppressWarnings("unchecked")
    //@Deprecated
    CARAPI GetStringArrayList(
        /* [in] */ const String& key,
        /* [out] */ IArrayList** value);

    /**
     * Returns a string containing a concise, human-readable description of this object.
     * @return a printable representation of this object.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI WriteValue(
        /* [in] */ IParcel* dest,
        /* [in] */ IInterface* obj);

    CARAPI_(AutoPtr<IInterface>) ReadValue(
        /* [in] */ IParcel* source);

private:
    static const Int32 VAL_NULL = -1;
    static const Int32 VAL_STRING = 0;
    static const Int32 VAL_BYTE = 1;
    static const Int32 VAL_INTEGER16 = 2;
    static const Int32 VAL_INTEGER32 = 3;
    static const Int32 VAL_INTEGER64 = 4;
    static const Int32 VAL_FLOAT = 5;
    static const Int32 VAL_DOUBLE = 6;
    static const Int32 VAL_BOOLEAN = 7;
    static const Int32 VAL_ARRAYOF = 8;
    static const Int32 VAL_ARRAYLIST = 9;

public:
    static const String TAG;

private:
    /** Holds the actual values */
    //AutoPtr< HashMap<String, AutoPtr<IInterface> > > mValues;
    AutoPtr<IHashMap> mValues;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTVALUES_H__
