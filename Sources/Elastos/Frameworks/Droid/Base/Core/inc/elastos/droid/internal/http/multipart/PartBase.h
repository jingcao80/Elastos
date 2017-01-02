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

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PARTBASE_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PARTBASE_H__

#include "elastos/droid/internal/http/multipart/Part.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

/**
 * Provides setters and getters for the basic Part properties.
 *
 * @author Michael Becke
 */
class PartBase : public Part
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructor.
     *
     * @param name The name of the part
     * @param contentType The content type, or <code>null</code>
     * @param charSet The character encoding, or <code>null</code>
     * @param transferEncoding The transfer encoding, or <code>null</code>
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charSet,
        /* [in] */ const String& transferEncoding);

    /**
     * Returns the name.
     * @return The name.
     * @see Part#getName()
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the content type of this part.
     * @return String The name.
     */
    CARAPI GetContentType(
        /* [out] */ String* contentType);

    /**
     * Return the character encoding of this part.
     * @return String The name.
     */
    CARAPI GetCharSet(
        /* [out] */ String* charSet);

    /**
     * Returns the transfer encoding of this part.
     * @return String The name.
     */
    CARAPI GetTransferEncoding(
        /* [out] */ String* transferEncoding);

    /**
     * Sets the character encoding.
     *
     * @param charSet the character encoding, or <code>null</code> to exclude the character
     * encoding header
     */
    CARAPI SetCharSet(
        /* [in] */ const String& charSet);

    /**
     * Sets the content type.
     *
     * @param contentType the content type, or <code>null</code> to exclude the content type header
     */
    CARAPI SetContentType(
        /* [in] */ const String& contentType);

    /**
     * Sets the part name.
     *
     * @param name
     */
    CARAPI SetName(
        /* [in] */ const String& name);

    /**
     * Sets the transfer encoding.
     *
     * @param transferEncoding the transfer encoding, or <code>null</code> to exclude the
     * transfer encoding header
     */
    CARAPI SetTransferEncoding(
        /* [in] */ const String& transferEncoding);

private:
    /** Name of the file part. */
    String mName;

    /** Content type of the file part. */
    String mContentType;

    /** Content encoding of the file part. */
    String mCharSet;

    /** The transfer encoding. */
    String mTransferEncoding;
};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PARTBASE_H__
