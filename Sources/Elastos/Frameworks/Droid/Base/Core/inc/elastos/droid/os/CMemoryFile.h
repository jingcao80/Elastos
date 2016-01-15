#ifndef __ELASTOS_DROID_OS_CMEMORYFILE_H__
#define __ELASTOS_DROID_OS_CMEMORYFILE_H__

#include "_Elastos_Droid_Os_CMemoryFile.h"
#include <elastos/core/Object.h>
#include <elastos/io/InputStream.h>
#include <elastos/io/OutputStream.h>

using Elastos::IO::InputStream;
using Elastos::IO::OutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * MemoryFile is a wrapper for the Linux ashmem driver.
 * MemoryFiles are backed by shared memory, which can be optionally
 * set to be purgeable.
 * Purgeable files may have their contents reclaimed by the kernel
 * in low memory conditions (only if allowPurging is set to true).
 * After a file is purged, attempts to read or write the file will
 * cause an IOException to be thrown.
 */
CarClass(CMemoryFile)
    , public Object
    , public IMemoryFile
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Allocates a new ashmem region. The region is initially not purgable.
     *
     * @param name optional name for the file (can be null).
     * @param length of the memory file in bytes, must be non-negative.
     * @throws IOException if the memory file could not be created.
     */
    CMemoryFile();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 length);

    /**
     * Closes the memory file. If there are no other open references to the memory
     * file, it will be deleted.
     */
    CARAPI Close();

    /**
     * Returns the length of the memory file.
     *
     * @return file length.
     */
    CARAPI GetLenth(
        /* [out] */ Int32* len);

    /**
     * Is memory file purging enabled?
     *
     * @return true if the file may be purged.
     */
    CARAPI IsPurgingAllowed(
        /* [out] */ Boolean* result);

    /**
     * Enables or disables purging of the memory file.
     *
     * @param allowPurging true if the operating system can purge the contents
     * of the file in low memory situations
     * @return previous value of allowPurging
     */
    CARAPI AllowPurging(
        /* [in] */ Boolean allowPurging,
        /* [out] */ Boolean* result);

    /**
     * Creates a new InputStream for reading from the memory file.
     *
     @return InputStream
     */
    CARAPI GetInputStream(
        /* [out] */ IInputStream** stream);

    /**
     * Creates a new OutputStream for writing to the memory file.
     *
     @return OutputStream
     */
    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** stream);

    /**
     * Reads bytes from the memory file.
     * Will throw an IOException if the file has been purged.
     *
     * @param buffer byte array to read bytes into.
     * @param srcOffset offset into the memory file to read from.
     * @param destOffset offset into the byte array buffer to read into.
     * @param count number of bytes to read.
     * @return number of bytes read.
     * @throws IOException if the memory file has been purged or deactivated.
     */
    CARAPI ReadBytes(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 destOffset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Write bytes to the memory file.
     * Will throw an IOException if the file has been purged.
     *
     * @param buffer byte array to write bytes from.
     * @param srcOffset offset into the byte array buffer to write from.
     * @param destOffset offset  into the memory file to write to.
     * @param count number of bytes to write.
     * @throws IOException if the memory file has been purged or deactivated.
     */
    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 destOffset,
        /* [in] */ Int32 count);

    /**
     * Unmaps the memory file from the process's memory space, but does not close it.
     * After this method has been called, read and write operations through this object
     * will fail, but {@link #getFileDescriptor()} will still return a valid file descriptor.
     *
     * @hide
     */
    CARAPI Deactivate();

    /**
     * Gets a FileDescriptor for the memory file.
     *
     * The returned file descriptor is not duplicated.
     *
     * @throws IOException If the memory file has been closed.
     *
     * @hide
     */
    AutoPtr<IFileDescriptor> GetFileDescriptor();

    /**
     * Returns the size of the memory file that the file descriptor refers to,
     * or -1 if the file descriptor does not refer to a memory file.
     *
     * @throws IOException If <code>fd</code> is not a valid file descriptor.
     *
     * @hide
     */
    static CARAPI GetSize(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* size);

private:
    /**
     * Checks whether the memory file has been deactivated.
     */
    CARAPI_(Boolean) IsDeactivated();

    /**
     * Checks whether the memory file has been closed.
     */
    CARAPI_(Boolean) IsClosed();


    CARAPI_(void) Finalize();

    static CARAPI native_open(
        /* [in] */ const String& name,
        /* [in] */ Int32 length,
        /* [out] */ IFileDescriptor** fd);

    // returns memory address for ashmem region
    static CARAPI native_mmap(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 length,
        /* [in] */ Int32 mode,
        /* [out] */ Handle64* addr);

    static CARAPI native_munmap(
        /* [in] */ Handle64 addr,
        /* [in] */ Int32 length);

    static CARAPI native_close(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI native_read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Handle64 address,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 destOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean isUnpinned,
        /* [out] */ Int32* result);

    static CARAPI native_write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Handle64 address,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 destOffset,
        /* [in] */ Int32 count,
        /* [in] */ Boolean isUnpinned);

    static CARAPI native_pin(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean pin);

    static CARAPI native_get_size(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Int32* result);

private:
    static const String TAG;// = "MemoryFile";

    // mmap(2) protection flags from <sys/mman.h>
    //static const Int32 PROT_READ;// = 0x1;
    //static const Int32 PROT_WRITE;// = 0x2;

    AutoPtr<IFileDescriptor> mFD;        // ashmem file descriptor
    Handle64 mAddress;   // address of ashmem memory
    Int32 mLength;    // total length of our ashmem region
    Boolean mAllowPurging;// = false;  // true if our ashmem region is unpinned

private:
    class MemoryInputStream
        : public InputStream
    {
    public:
        MemoryInputStream(
            /* [in] */ CMemoryFile* host);

        CARAPI Available(
            /* [out] */ Int32* number);

        CARAPI Mark(
            /* [in] */ Int32 readLimit);

        CARAPI IsMarkSupported(
            /* [out] */ Boolean* supported);

        CARAPI Read(
            /* [out] */ Int32* value);

        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 byteOffset,
            /* [in] */ Int32 byteCount,
            /* [out] */ Int32* number);

        CARAPI Reset();

        CARAPI Skip(
            /* [in] */ Int64 byteCount,
            /* [out] */ Int64* number);

    private:
        Int32 mMark;// = 0;
        Int32 mOffset;// = 0;
        AutoPtr<ArrayOf<Byte> > mSingleByte;
        CMemoryFile* mHost;

    };

    class MemoryOutputStream
        : public OutputStream
    {
    public:
        MemoryOutputStream(
            /* [in] */ CMemoryFile* host);

        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        CARAPI Write(
            /* [in] */ Int32 oneByte);

    private:
        Int32 mOffset;// = 0;
        AutoPtr<ArrayOf<Byte> > mSingleByte;
        CMemoryFile* mHost;
    };

    friend class MemoryInputStream;
    friend class MemoryOutputStream;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMEMORYFILE_H__
