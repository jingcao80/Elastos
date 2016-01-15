#ifndef __ELASTOS_DROID_OS_CPARCEL_H__
#define __ELASTOS_DROID_OS_CPARCEL_H__

#include "_Elastos_Droid_Os_CParcel.h"
#include <elastos/core/Object.h>

using Elastos::Core::IClassLoader;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CParcel)
    , public Object
    , public IParcel
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Marshall(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI Unmarshall(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI AppendFrom(
        /* [in] */ IParcel* parcel,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI HasFileDescriptors(
        /* [out] */ Boolean* result);

    CARAPI ReadByte(
        /* [out] */ Byte* value);

    CARAPI WriteByte(
        /* [in] */ Byte value);

    CARAPI ReadBoolean(
        /* [out] */ Boolean* value);

    CARAPI WriteBoolean(
        /* [in] */ Boolean value);

    CARAPI ReadChar(
        /* [out] */ Char32* value);

    CARAPI WriteChar(
        /* [in] */ Char32 value);

    CARAPI ReadInt16(
        /* [out] */ Int16* value);

    CARAPI WriteInt16(
        /* [in] */ Int16 value);

    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI ReadInt64(
        /* [out] */ Int64* value);

    CARAPI WriteInt64(
        /* [in] */ Int64 value);

    CARAPI ReadFloat(
        /* [out] */ Float* value);

    CARAPI WriteFloat(
        /* [in] */ Float value);

    CARAPI ReadDouble(
        /* [out] */ Double* value);

    CARAPI WriteDouble(
        /* [in] */ Double value);

    CARAPI ReadString(
        /* [out] */ String* str);

    CARAPI WriteString(
        /* [in] */ const String& str);

    CARAPI ReadStruct(
        /* [out] */ Handle32* address);

    CARAPI WriteStruct(
        /* [in] */ Handle32 value,
        /* [in] */ Int32 size);

    CARAPI ReadEMuid(
        /* [out] */ EMuid* id);

    CARAPI WriteEMuid(
        /* [in] */ const EMuid& id);

    CARAPI ReadEGuid(
        /* [out] */ EGuid* id);

    CARAPI WriteEGuid(
        /* [in] */ const EGuid& id);

    CARAPI ReadArrayOf(
        /* [out] */ Handle32* array);

    CARAPI WriteArrayOf(
        /* [in] */ Handle32 array);

    CARAPI ReadArrayOfString(
        /* [out, callee] */ ArrayOf<String>** array);

    CARAPI WriteArrayOfString(
        /* [in] */ ArrayOf<String>* array);

    CARAPI ReadInterfacePtr(
        /* [out] */ Handle32* itfPtr);

    CARAPI WriteInterfacePtr(
        /* [in] */ IInterface* value);

    // Place a file descriptor into the parcel.  The given fd must remain
    // valid for the lifetime of the parcel.
    CARAPI WriteFileDescriptor(
        /* [in] */ Int32 fd);

    // Place a file descriptor into the parcel.  A dup of the fd is made, which
    // will be closed once the parcel is destroyed.
    CARAPI WriteDupFileDescriptor(
        /* [in] */ Int32 fd);

    // Retrieve a file descriptor from the parcel.  This returns the raw fd
    // in the parcel, which you do not own -- use dup() to get your own copy.
    CARAPI ReadFileDescriptor(
        /* [out] */ Int32* fd);

    CARAPI Clone(
        /* [in] */ IParcel* srcParcel);

    CARAPI GetDataPosition(
        /* [out] */ Int32* position);

    CARAPI SetDataPosition(
        /* [in] */ Int32 position);

    CARAPI GetElementSize(
        /* [out] */ Int32* size);

    CARAPI GetElementPayload(
        /* [out] */ Handle32* buffer);

public:
    static CARAPI OpenFileDescriptor(
        /* [in] */ const String& file,
        /* [in] */ Int32 mode,
        /* [out] */ IFileDescriptor** desc);

    static CARAPI DupFileDescriptor(
        /* [in] */ IFileDescriptor* orig,
        /* [out] */ IFileDescriptor** desc);

    static CARAPI CloseFileDescriptor(
        /* [in] */ IFileDescriptor* desc);

    static CARAPI ClearFileDescriptor(
        /* [in] */ IFileDescriptor* desc);

    static AutoPtr<IFileDescriptor> CreateFileDescriptor(
        /* [in] */ Int32 fd);

    static Int32 GetFDFromFileDescriptor(
        /* [in] */ IFileDescriptor* fileDescriptor);

    static void SetFileDescriptorOfFD(
        /* [in] */ IFileDescriptor* fileDescriptor,
        /* [in] */ Int32 value);

    static AutoPtr<IArrayList> CreateStringArrayList(
        /* [in] */ IParcel* source);

    /**
     * Read and return a new Bundle object from the parcel at the current
     * dataPosition().  Returns null if the previously written Bundle object was
     * null.
     */
     static AutoPtr<IBundle> ReadBundle(
        /* [in] */ IParcel* source);

    /**
     * Read and return a new Bundle object from the parcel at the current
     * dataPosition(), using the given class loader to initialize the class
     * loader of the Bundle for later retrieval of Parcelable objects.
     * Returns null if the previously written Bundle object was null.
     */
    static AutoPtr<IBundle> ReadBundle(
        /* [in] */ IParcel* source,
        /* [in] */ IClassLoader* loader);

    /**
     * Flatten a Bundle into the parcel at the current dataPosition(),
     * growing dataCapacity() if needed.
     */
    static CARAPI WriteBundle(
        /* [in] */ IParcel* dest,
        /* [in] */ IBundle* val);

    /**
     * Flatten a List containing a particular object type into the parcel, at
     * the current dataPosition() and growing dataCapacity() if needed.  The
     * type of the objects in the list must be one that implements Parcelable.
     * Unlike the generic writeList() method, however, only the raw data of the
     * objects is written and not their type, so you must use the corresponding
     * readTypedList() to unmarshall them.
     *
     * @param val The list of objects to be written.
     *
     * @see #createTypedArrayList
     * @see #readTypedList
     * @see Parcelable
     */
    static CARAPI WriteTypedList(
        /* [in] */ IParcel* dest,
        /* [in] */ IList* val);

    /**
     * Flatten a List containing String objects into the parcel, at
     * the current dataPosition() and growing dataCapacity() if needed.  They
     * can later be retrieved with {@link #createStringArrayList} or
     * {@link #readStringList}.
     *
     * @param val The list of strings to be written.
     *
     * @see #createStringArrayList
     * @see #readStringList
     */
    static CARAPI WriteStringList(
        /* [in] */ IParcel* dest,
        /* [in] */ IList* val);

    /**
     * Read into the given List items containing a particular object type
     * that were written with {@link #writeTypedList} at the
     * current dataPosition().  The list <em>must</em> have
     * previously been written via {@link #writeTypedList} with the same object
     * type.
     *
     * @return A newly created ArrayList containing objects with the same data
     *         as those that were previously written.
     *
     * @see #writeTypedList
     */
    static CARAPI ReadTypedList(
        /* [in] */ IParcel* source,
        /* [in] */ IList* val);

    static CARAPI WriteCharSequenceArray(
        /* [in] */ IParcel* source,
        /* [in] */ ArrayOf<ICharSequence*>* array);

    /**
     * Read and return a CharSequence[] object from the parcel.
     * {@hide}
     */
    static AutoPtr<ArrayOf<ICharSequence*> > ReadCharSequenceArray(
        /* [in] */ IParcel* source);

private:
    CARAPI ReadValue(
        /* [in] */ PVoid value,
        /* [in] */ Int32 type);

    CARAPI WriteValue(
        /* [in] */ PVoid value,
        /* [in] */ Int32 type,
        /* [in] */ Int32 size);

private:
    AutoPtr<IParcel> mData;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CPARCEL_H__
