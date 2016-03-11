
#ifndef __ELASTOS_DROID_MEDIA_CMETADATA_H__
#define __ELASTOS_DROID_MEDIA_CMETADATA_H__

#include "_Elastos_Droid_Media_CMetadata.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::IDate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ISet;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMetadata)
    , public Object
    , public IMetadata
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMetadata();

    CARAPI constructor();

    /**
     * Check a parcel containing metadata is well formed. The header
     * is checked as well as the individual records format. However, the
     * data inside the record is not checked because we do lazy access
     * (we check/unmarshall only data the user asks for.)
     *
     * Format of a metadata parcel:
     <pre>
                         1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                     metadata total size                       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     'M'       |     'E'       |     'T'       |     'A'       |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                .... metadata records ....                     |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     </pre>
     *
     * @param parcel With the serialized data. Metadata keeps a
     *               reference on it to access it later on. The caller
     *               should not modify the parcel after this call (and
     *               not call recycle on it.)
     * @return false if an error occurred.
     */
    CARAPI Parse(
        /* [in] */ IParcel* parcel,
        /* [out] */ Boolean* result);

    /**
     * @return The set of metadata ID found.
     */
    CARAPI KeySet(
        /* [out] */ ISet** result);

    /**
     * @return true if a value is present for the given key.
     */
    CARAPI Has(
        /* [in] */ Int32 metadataId,
        /* [out] */ Boolean* result);

    // Accessors.
    // Caller must make sure the key is present using the {@code has}
    // method otherwise a RuntimeException will occur.
    CARAPI GetString(
        /* [in] */ Int32 key,
        /* [out] */ String* result);

    /**
     * {@hide}
     */
    CARAPI GetInt32(
        /* [in] */ Int32 key,
        /* [out] */ Int32* result);

    /**
     * Get the boolean value indicated by key
     */
    CARAPI GetBoolean(
        /* [in] */ Int32 key,
        /* [out] */ Boolean* result);

    /**
     * {@hide}
     */
    CARAPI GetInt64(
        /* [in] */ Int32 key,
        /* [out] */ Int64* result);

    /**
     * {@hide}
     */
    CARAPI GetDouble(
        /* [in] */ Int32 key,
        /* [out] */ Double* result);

    /**
     * {@hide}
     */
    CARAPI GetByteArray(
        /* [in] */ Int32 key,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * {@hide}
     */
    CARAPI GetDate(
        /* [in] */ Int32 key,
        /* [out] */ IDate** result);

     CARAPI RecycleParcel();

    /**
     * @return the last available system metadata id. Ids are
     *         1-indexed.
     * {@hide}
     */
    static CARAPI LastSytemId(
        /* [out] */ Int32* result);

    /**
     * @return the first available cutom metadata id.
     * {@hide}
     */
    static CARAPI FirstCustomId(
        /* [out] */ Int32* result);

    /**
     * @return the last value of known type. Types are 1-indexed.
     * {@hide}
     */
    static CARAPI LastType(
        /* [out] */ Int32* result);

private:
    /**
     * Go over all the records, collecting metadata keys and records'
     * type field offset in the Parcel. These are stored in
     * mKeyToPosMap for latter retrieval.
     * Format of a metadata record:
     <pre>
                         1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                     record size                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                     metadata key                              |  // TITLE
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                     metadata type                             |  // STRING_VAL
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                                                               |
      |                .... metadata payload ....                     |
      |                                                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     </pre>
     * @param parcel With the serialized records.
     * @param bytesLeft How many bytes in the parcel should be processed.
     * @return false if an error occurred during parsing.
     */
    CARAPI_(Boolean) ScanAllRecords(
        /* [in] */ IParcel* parcel,
        /* [in] */ Int32 bytesLeft);

    /**
     * Check val is either a system id or a custom one.
     * @param val Metadata key to test.
     * @return true if it is in a valid range.
     **/
    CARAPI_(Boolean) CheckMetadataId(
        /* [in] */ Int32 val);

    /**
     * Check the type of the data match what is expected.
     */
    CARAPI CheckType(
        /* [in] */ Int32 key,
        /* [in] */ Int32 expectedType);

public:
    // Shorthands to set the MediaPlayer's metadata filter.
    /**
     * {@hide}
     */
    //public static final Set<Integer> MATCH_NONE = Collections.EMPTY_SET;
    static AutoPtr<ISet> MATCH_NONE;

    /**
     * {@hide}
     */
    //public static final Set<Integer> MATCH_ALL = Collections.singleton(ANY);
    static AutoPtr<ISet> MATCH_ALL;

private:
    static const Int32 LAST_SYSTEM;// = 31;
    static const Int32 FIRST_CUSTOM;// = 8192;

    // FIXME: misses a type for shared heap is missing (MemoryFile).
    // FIXME: misses a type for bitmaps.
    static const Int32 LAST_TYPE;// = 7;

    static const String TAG; // = "media.Metadata";

    static const Int32 kInt32Size;// = 4;
    static const Int32 kMetaHeaderSize;// = 2 * kInt32Size; //  size + marker
    static const Int32 kRecordHeaderSize;// = 3 * kInt32Size; // size + id + type
    static const Int32 kMetaMarker;// = 0x4d455441;  // 'M' 'E' 'T' 'A'

    // After a successful parsing, set the parcel with the serialized metadata.
    AutoPtr<IParcel> mParcel;

    // Map to associate a Metadata key (e.g TITLE) with the offset of
    // the record's payload in the parcel.
    // Used to look up if a key was present too.
    // Key: Metadata ID
    // Value: Offset of the metadata type field in the record.
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IInteger32> > mKeyToPosMap; // = new HashMap<Integer, Integer>();
    AutoPtr<IHashMap> mKeyToPosMap;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMETADATA_H__
