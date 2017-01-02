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

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CPARTHELPER_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CPARTHELPER_H__

#include "_Elastos_Droid_Internal_Http_Multipart_CPartHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CarClass(CPartHelper)
    , public Singleton
    , public IPartHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return the boundary string.
     * @return the boundary string
     * @deprecated uses a constant string. Rather use {@link #getPartBoundary}
     */
    CARAPI GetBoundary(
        /* [out] */ String* boundary);

    /**
     * Write all parts and the last boundary to the specified output stream.
     *
     * @param out The stream to write to.
     * @param parts The parts to write.
     *
     * @throws IOException If an I/O error occurs while writing the parts.
     */
    CARAPI SendParts(
        /* [in] */ IOutputStream* outStream,
        /* [in] */ ArrayOf<IPart*>* parts);

    /**
     * Write all parts and the last boundary to the specified output stream.
     *
     * @param out The stream to write to.
     * @param parts The parts to write.
     * @param partBoundary The ASCII bytes to use as the part boundary.
     *
     * @throws IOException If an I/O error occurs while writing the parts.
     *
     * @since 3.0
     */
    CARAPI SendParts(
        /* [in] */ IOutputStream* outStream,
        /* [in] */ ArrayOf<IPart*>* parts,
        /* [in] */ ArrayOf<Byte>* partBoundary);

    /**
     * Return the total sum of all parts and that of the last boundary
     *
     * @param parts The parts.
     * @return The total length
     *
     * @throws IOException If an I/O error occurs while writing the parts.
     */
    CARAPI GetLengthOfParts(
        /* [in] */ ArrayOf<IPart*>* parts,
        /* [out] */ Int64* length);

    /**
     * Gets the length of the multipart message including the given parts.
     *
     * @param parts The parts.
     * @param partBoundary The ASCII bytes to use as the part boundary.
     * @return The total length
     *
     * @throws IOException If an I/O error occurs while writing the parts.
     *
     * @since 3.0
     */
    CARAPI GetLengthOfParts(
        /* [in] */ ArrayOf<IPart*>* parts,
        /* [in] */ ArrayOf<Byte>* partBoundary,
        /* [out] */ Int64* length);
};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CPARTHELPER_H__
