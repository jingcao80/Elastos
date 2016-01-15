#ifndef __ELASTOS_IO_CNIOACCESS_H__
#define __ELASTOS_IO_CNIOACCESS_H__

#include "_Elastos_IO_CNIOAccess.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace IO {

/**
 * This class is used via JNI by code in frameworks/base/.
 */
CarClass(CNIOAccess)
    , public Singleton
    , public INIOAccess
{

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the underlying native pointer to the data of the given
     * Buffer starting at the Buffer's current position, or 0 if the
     * Buffer is not backed by native heap storage.
     */
    CARAPI GetBasePointer(
        /* [in] */ IBuffer* b,
        /* [out] */ Int64* result);

    /**
     * Returns the underlying Java array containing the data of the
     * given Buffer, or null if the Buffer is not backed by a Java array.
     */
    CARAPI GetBaseArray(
        /* [in] */ IBuffer* b,
        /* [out] */ IInterface** result);

    /**
     * Returns the offset in bytes from the start of the underlying
     * Java array object containing the data of the given Buffer to
     * the actual start of the data. The start of the data takes into
     * account the Buffer's current position. This method is only
     * meaningful if getBaseArray() returns non-null.
     */
    CARAPI GetBaseArrayOffset(
        /* [in] */ IBuffer* b,
        /* [out] */ Int32* result);

};

} // namespace IO
} // namespace Elastos

#endif