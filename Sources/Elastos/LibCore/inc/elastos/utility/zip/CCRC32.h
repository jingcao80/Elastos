
#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCRC32_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCRC32_H__

#include "_Elastos_Utility_Zip_CCRC32.h"
#include "Object.h"


namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CCRC32)
    , public Object
    , public ICRC32
    , public IChecksum
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCRC32();

    /**
     * Returns the CRC32 checksum for all input received.
     *
     * @return The checksum for this instance.
     */
    CARAPI GetValue(
        /* [out] */ Int64* value);

    /**
     * Resets the CRC32 checksum to it initial state.
     */
    CARAPI Reset();

    /**
     * Updates this checksum with the byte value provided as integer.
     *
     * @param val
     *            represents the byte to update the checksum.
     */
    CARAPI Update(
        /* [in] */ Int32 val);

    /**
     * Updates this checksum with the bytes contained in buffer {@code buf}.
     *
     * @param buf
     *            the buffer holding the data to update the checksum with.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Update this {@code CRC32} checksum with the contents of {@code buf},
     * starting from {@code offset} and reading {@code byteCount} bytes of data.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

private:
    CARAPI_(Int64) UpdateImpl(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 crc);

    CARAPI_(Int64) UpdateByteImpl(
        /* [in] */ Byte val,
        /* [in] */ Int64 crc);

public:
    Int64 mTbytes;

private:
    Int64 mCrc;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CCRC32_H__
