
#ifndef __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTOR_H__
#define __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTOR_H__

#include "_Elastos_Droid_Content_Res_CAssetFileDescriptor.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;

using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * File descriptor of an entry in the AssetManager.  This provides your own
 * opened FileDescriptor that can be used to read the data, as well as the
 * offset and length of that entry's data in the file.
 */
CarClass(CAssetFileDescriptor)
    , public Object
    , public IAssetFileDescriptor
    , public ICloseable
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAssetFileDescriptor();

    virtual ~CAssetFileDescriptor();

    /**
     * Create an empty surface, which will later be filled in by
     * readFromParcel().
     */
    CARAPI constructor();

    /**
     * Create a new AssetFileDescriptor from the given values.
     *
     * @param fd The underlying file descriptor.
     * @param startOffset The location within the file that the asset starts.
     *            This must be 0 if length is UNKNOWN_LENGTH.
     * @param length The number of bytes of the asset, or
     *            {@link #UNKNOWN_LENGTH} if it extends to the end of the file.
     */
    CARAPI constructor(
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int64 startOffset,
        /* [in] */ Int64 length);

    /**
     * Create a new AssetFileDescriptor from the given values.
     *
     * @param fd The underlying file descriptor.
     * @param startOffset The location within the file that the asset starts.
     *            This must be 0 if length is UNKNOWN_LENGTH.
     * @param length The number of bytes of the asset, or
     *            {@link #UNKNOWN_LENGTH} if it extends to the end of the file.
     * @param extras additional details that can be used to interpret the
     *            underlying file descriptor. May be null.
     */
    CARAPI constructor(
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int64 startOffset,
        /* [in] */ Int64 length,
        /* [in] */ IBundle* extras);

    /**
     * The AssetFileDescriptor contains its own ParcelFileDescriptor, which
     * in addition to the normal FileDescriptor object also allows you to close
     * the descriptor when you are done with it.
     */
    CARAPI GetParcelFileDescriptor(
        /* [out] */ IParcelFileDescriptor** fd);

    /**
     * Returns the FileDescriptor that can be used to read the data in the
     * file.
     */
    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fd);

    /**
     * Returns the byte offset where this asset entry's data starts.
     */
    CARAPI GetStartOffset(
        /* [out] */ Int64* startOffset);

    /**
     * Returns any additional details that can be used to interpret the
     * underlying file descriptor. May be null.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * Returns the total number of bytes of this asset entry's data.  May be
     * {@link #UNKNOWN_LENGTH} if the asset extends to the end of the file.
     * If the AssetFileDescriptor was constructed with {@link #UNKNOWN_LENGTH},
     * this will use {@link ParcelFileDescriptor#getStatSize()
     * ParcelFileDescriptor.getStatSize()} to find the total size of the file,
     * returning that number if found or {@link #UNKNOWN_LENGTH} if it could
     * not be determined.
     *
     * @see #getDeclaredLength()
     */
    CARAPI GetLength(
        /* [out] */ Int64* length);

    /**
     * Return the actual number of bytes that were declared when the
     * AssetFileDescriptor was constructed.  Will be
     * {@link #UNKNOWN_LENGTH} if the length was not declared, meaning data
     * should be read to the end of the file.
     *
     * @see #getDeclaredLength()
     */
    CARAPI GetDeclaredLength(
        /* [out] */ Int64* declaredLength);

    /**
     * Convenience for calling <code>getParcelFileDescriptor().close()</code>.
     */
    CARAPI Close();

    /**
     * Create and return a new auto-close input stream for this asset.  This
     * will either return a full asset {@link AutoCloseInputStream}, or
     * an underlying {@link ParcelFileDescriptor.AutoCloseInputStream
     * ParcelFileDescriptor.AutoCloseInputStream} depending on whether the
     * the object represents a complete file or sub-section of a file.  You
     * should only call this once for a particular asset.
     */
    CARAPI CreateInputStream(
        /* [out] */  IFileInputStream** stream);

    /**
     * Create and return a new auto-close output stream for this asset.  This
     * will either return a full asset {@link AutoCloseOutputStream}, or
     * an underlying {@link ParcelFileDescriptor.AutoCloseOutputStream
     * ParcelFileDescriptor.AutoCloseOutputStream} depending on whether the
     * the object represents a complete file or sub-section of a file.  You
     * should only call this once for a particular asset.
     */
    CARAPI CreateOutputStream(
        /* [out] */ IFileOutputStream** stream);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IParcelFileDescriptor> mFd;
    Int64 mStartOffset;
    Int64 mLength;
    AutoPtr<IBundle> mExtras;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTOR_H__
