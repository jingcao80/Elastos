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

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PART_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PART_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::IO::IOutputStream;
// using Org::Apache::Commons::Logging::ILog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

/**
 * Abstract class for one Part of a multipart post object.
 *
 * @author <a href="mailto:mattalbright@yahoo.com">Matthew Albright</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 */
class Part
    : public Object
    , public IPart
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI SetPartBoundary(
        /* [in] */ ArrayOf<Byte>* boundaryBytes);

    /**
     * Tests if this part can be sent more than once.
     * @return <code>true</code> if {@link #sendData(OutputStream)} can be successfully called
     * more than once.
     * @since 3.0
     */
    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    /**
     * Write all the data to the output stream.
     * If you override this method make sure to override
     * #length() as well
     *
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    CARAPI Send(
        /* [in] */ IOutputStream* outStream);

    /**
     * Return the full length of all the data.
     * If you override this method make sure to override
     * #send(OutputStream) as well
     *
     * @return long The length.
     * @throws IOException If an IO problem occurs
     */
    CARAPI Length(
        /* [out] */ Int64* length);

    /**
     * Return a string representation of this object.
     * @return A string representation of this object.
     * @see java.lang.Object#toString()
     */
    CARAPI ToString(
         /* [out] */ String* str);

public:
    /**
     * Return the boundary string.
     * @return the boundary string
     * @deprecated uses a constant string. Rather use {@link #getPartBoundary}
     */
    static CARAPI GetBoundary(
        /* [out] */ String* boundary);

    /**
     * Write all parts and the last boundary to the specified output stream.
     *
     * @param out The stream to write to.
     * @param parts The parts to write.
     *
     * @throws IOException If an I/O error occurs while writing the parts.
     */
    static CARAPI SendParts(
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
    static CARAPI SendParts(
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
    static CARAPI GetLengthOfParts(
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
    static CARAPI GetLengthOfParts(
        /* [in] */ ArrayOf<IPart*>* parts,
        /* [in] */ ArrayOf<Byte>* partBoundary,
        /* [out] */ Int64* length);

protected:
    /**
     * Gets the part boundary to be used.
     * @return the part boundary as an array of bytes.
     *
     * @since 3.0
     */
    virtual CARAPI_(AutoPtr<ArrayOf<Byte> >) GetPartBoundary();

    /**
     * Write the start to the specified output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendStart(
        /* [in] */ IOutputStream* outStream);

    /**
     * Write the content disposition header to the specified output stream
     *
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendDispositionHeader(
        /* [in] */ IOutputStream* outStream);

    /**
     * Write the content type header to the specified output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendContentTypeHeader(
        /* [in] */ IOutputStream* outStream);

    /**
     * Write the content transfer encoding header to the specified
     * output stream
     *
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendTransferEncodingHeader(
        /* [in] */ IOutputStream* outStream);

    /**
     * Write the end of the header to the output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendEndOfHeader(
        /* [in] */ IOutputStream* outStream);

    /**
     * Write the data to the specified output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendData(
        /* [in] */ IOutputStream* outStream) = 0;

    /**
     * Return the length of the main content
     *
     * @return long The length.
     * @throws IOException If an IO problem occurs
     */
    virtual CARAPI LengthOfData(
        /* [out] */ Int64* length) = 0;

    /**
     * Write the end data to the output stream.
     * @param out The output stream
     * @throws IOException If an IO problem occurs.
     */
    virtual CARAPI SendEnd(
        /* [in] */ IOutputStream* outStream);

protected:
    /**
     * The boundary
     * @deprecated use {@link org.apache.http.client.methods.multipart#MULTIPART_BOUNDARY}
     */
    static const String BOUNDARY;

    /**
     * The boundary as a byte array.
     * @deprecated
     */
    static const AutoPtr<ArrayOf<Byte> > BOUNDARY_BYTES;

    /** Carriage return/linefeed */
    static const String CRLF;

    /** Carriage return/linefeed as a byte array */
    static const AutoPtr<ArrayOf<Byte> > CRLF_BYTES;

    /** Content dispostion characters */
    static const String QUOTE;

    /** Content dispostion as a byte array */
    static const AutoPtr<ArrayOf<Byte> > QUOTE_BYTES;

    /** Extra characters */
    static const String EXTRA;

    /** Extra characters as a byte array */
    static const AutoPtr<ArrayOf<Byte> > EXTRA_BYTES;

    /** Content dispostion characters */
    static const String CONTENT_DISPOSITION;

    /** Content dispostion as a byte array */
    static const AutoPtr<ArrayOf<Byte> > CONTENT_DISPOSITION_BYTES;

    /** Content type header */
    static const String CONTENT_TYPE;

    /** Content type header as a byte array */
    static const AutoPtr<ArrayOf<Byte> > CONTENT_TYPE_BYTES;

    /** Content charset */
    static const String CHARSET;

    /** Content charset as a byte array */
    static const AutoPtr<ArrayOf<Byte> > CHARSET_BYTES;

    /** Content type header */
    static const String CONTENT_TRANSFER_ENCODING;

    /** Content type header as a byte array */
    static const AutoPtr<ArrayOf<Byte> > CONTENT_TRANSFER_ENCODING_BYTES;

private:
    /** Log object for this class. */
    // static const AutoPtr<ILog> LOG;

    /**
     * The default boundary to be used if {@link #setPartBoundary(byte[])} has not
     * been called.
     */
    static const AutoPtr<ArrayOf<Byte> > DEFAULT_BOUNDARY_BYTES;

    /**
     * The ASCII bytes to use as the multipart boundary.
     */
    AutoPtr<ArrayOf<Byte> > mBoundaryBytes;

};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_PART_H__
