//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_IO_NIOAccess_H__
#define __ELASTOS_IO_NIOAccess_H__

#include "elastos/core/Object.h"

namespace Elastos {
namespace IO {

/**
 * This class is used via JNI by code in frameworks/base/.
 */
class NIOAccess
{
public:
    /**
     * Returns the underlying native pointer to the data of the given
     * Buffer starting at the Buffer's current position, or 0 if the
     * Buffer is not backed by native heap storage.
     */
    static CARAPI GetBasePointer(
        /* [in] */ IBuffer* b,
        /* [out] */ Int64* result);

    /**
     * Returns the underlying Java array containing the data of the
     * given Buffer, or null if the Buffer is not backed by a Java array.
     */
    static CARAPI GetBaseArray(
        /* [in] */ IBuffer* b,
        /* [out] */ IInterface** result);

    static CARAPI GetRemainingBytes(
        /* [in] */ IBuffer* b,
        /* [out] */ Int32* remaining);

    /**
     * Returns the offset in bytes from the start of the underlying
     * Java array object containing the data of the given Buffer to
     * the actual start of the data. The start of the data takes into
     * account the Buffer's current position. This method is only
     * meaningful if getBaseArray() returns non-null.
     */
    static CARAPI GetBaseArrayOffset(
        /* [in] */ IBuffer* b,
        /* [out] */ Int32* result);

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_NIOAccess_H__
