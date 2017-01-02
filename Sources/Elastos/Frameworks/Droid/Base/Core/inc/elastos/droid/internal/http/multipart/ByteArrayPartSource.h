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

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_BYTEARRAYPARTSOURCE_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_BYTEARRAYPARTSOURCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

/**
 * A PartSource that reads from a byte array.  This class should be used when
 * the data to post is already loaded into memory.
 *
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 *
 * @since 2.0
 */
class ByteArrayPartSource
    : public Object
    , public IByteArrayPartSource
    , public IPartSource
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor for ByteArrayPartSource.
     *
     * @param fileName the name of the file these bytes represent
     * @param bytes the content of this part
     */
    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ ArrayOf<Byte>* bytes);

    /**
     * @see PartSource#getLength()
     */
    CARAPI GetLength(
        /* [out] */ Int64* length);

    /**
     * @see PartSource#getFileName()
     */
    CARAPI GetFileName(
        /* [out] */ String* fileName);

    /**
     * @see PartSource#createInputStream()
     */
    CARAPI CreateInputStream(
        /* [out] */ Elastos::IO::IInputStream** stream);

private:
    /** Name of the source file. */
    String mFileName;

    /** Byte array of the source file. */
    AutoPtr<ArrayOf<Byte> > mBytes;
};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_BYTEARRAYPARTSOURCE_H__
