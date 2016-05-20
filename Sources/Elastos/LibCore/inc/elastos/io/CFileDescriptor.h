#ifndef __ELASTOS_IO_CFILEDESCRIPTOR_H__
#define __ELASTOS_IO_CFILEDESCRIPTOR_H__

#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_IO_CFileDescriptor.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace IO {

CarClass(CFileDescriptor)
    , public Object
    , public IFileDescriptor
{
    friend class CRandomAccessFile;

public:
    /**
     * Constructs a new FileDescriptor containing an invalid handle. The
     * contained handle is usually modified by native code at a later point.
     */
    CFileDescriptor();

    CAR_INTERFACE_DECL()

    CARAPI constructor();
    /**
     * Ensures that data which is buffered within the underlying implementation
     * is written out to the appropriate device before returning.
     *
     * @throws SyncFailedException
     *             when the operation fails.
     */
    CARAPI Sync();

    /**
     * Indicates whether this FileDescriptor is valid.
     *
     * @return {@code true} if this FileDescriptor is valid, {@code false}
     *         otherwise.
     */
    CARAPI Valid(
        /* [out] */ Boolean* isValid);

    /**
     * Returns the int fd. It's highly unlikely you should be calling this. Please discuss
     * your needs with a libcore maintainer before using this method.
     * @hide internal use only
     */
    CARAPI GetInt(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetDescriptor(
        /* [out] */ Int32* fd);

    CARAPI SetDescriptor(
        /* [in] */ Int32 fd);

    /**
     * @hide internal use only
     */
    CARAPI IsSocket(
        /* [out] */ Boolean* isSocket);

public:
    /**
     * The FileDescriptor representing standard input.
     */
    static const AutoPtr<IFileDescriptor> IN;

    /**
     * FileDescriptor representing standard out.
     */
    static const AutoPtr<IFileDescriptor> OUT;

    /**
     * FileDescriptor representing standard error.
     */
    static const AutoPtr<IFileDescriptor> ERR;

private:
    Int32 mDescriptor;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILEDESCRIPTOR_H__
