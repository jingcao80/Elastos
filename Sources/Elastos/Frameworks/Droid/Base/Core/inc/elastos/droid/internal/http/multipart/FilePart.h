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

#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_FILEPART_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_FILEPART_H__

#include "elastos/droid/internal/http/multipart/PartBase.h"

using Elastos::IO::IFile;
// using Org::Apache::Commons::Logging::ILog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

class FilePart
    : public PartBase
    , public IFilePart
{
public:
    CAR_INTERFACE_DECL()

    /**
     * FilePart Constructor.
     *
     * @param name the name for this part
     * @param partSource the source for this part
     * @param contentType the content type for this part, if <code>null</code> the
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the
     * {@link #DEFAULT_CHARSET default} is used
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPartSource* partSource,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charset);

    /**
     * FilePart Constructor.
     *
     * @param name the name for this part
     * @param partSource the source for this part
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPartSource* partSource);

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param file the file to post
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IFile* file);

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param file the file to post
     * @param contentType the content type for this part, if <code>null</code> the
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the
     * {@link #DEFAULT_CHARSET default} is used
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IFile* file,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charset);

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param fileName the file name
     * @param file the file to post
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& fileName,
        /* [in] */ IFile* file);

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param fileName the file name
     * @param file the file to post
     * @param contentType the content type for this part, if <code>null</code> the
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the
     * {@link #DEFAULT_CHARSET default} is used
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& fileName,
        /* [in] */ IFile* file,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charset);

protected:
    /**
     * Write the disposition header to the output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs
     * @see Part#sendDispositionHeader(OutputStream)
     */
    // @Override
    CARAPI SendDispositionHeader(
        /* [in] */ IOutputStream* out);

    /**
     * Write the data in "source" to the specified stream.
     * @param out The output stream.
     * @throws IOException if an IO problem occurs.
     * @see Part#sendData(OutputStream)
     */
    CARAPI SendData(
        /* [in] */ IOutputStream* outStream);

    /**
     * Returns the source of the file part.
     *
     * @return The source.
     */
    CARAPI GetSource(
        /* [out] */ IPartSource** partSource);

    /**
     * Return the length of the data.
     * @return The length.
     * @see Part#lengthOfData()
     */
    CARAPI LengthOfData(
        /* [out] */ Int64* length);

protected:
    /** Attachment's file name */
     static const String FILE_NAME;

private:
    /** Log object for this class. */
    // static const AutoPtr<ILog> LOG;

    /** Attachment's file name as a byte array */
    static const AutoPtr<ArrayOf<Byte> > FILE_NAME_BYTES;

    /** Source of the file part. */
    AutoPtr<IPartSource> mSource;

};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_FILEPART_H__
