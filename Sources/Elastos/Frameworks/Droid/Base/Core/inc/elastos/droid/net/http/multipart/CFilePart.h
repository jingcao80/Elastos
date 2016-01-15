
#ifndef __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CFILEPART_H__
#define __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CFILEPART_H__

#include "_Elastos_Droid_Net_Internal_Http_Multipart_CFilePart.h"
#include "PartBase.h"
#include "elautoptr.h"

using Org::Apache::Commons::Logging::ILog;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Internal {
namespace Http {
namespace Multipart {

/**
 * This class implements a part of a Multipart post object that
 * consists of a file.
 *
 * @author <a href="mailto:mattalbright@yahoo.com">Matthew Albright</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 * @author <a href="mailto:mdiggory@latte.harvard.edu">Mark Diggory</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 *
 */

CarClass(CFilePart), public PartBase
{
    friend class CFilePartHelper;

public:
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
        /* [in] */ Elastos::IO::IFile* file);

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
        /* [in] */ Elastos::IO::IFile* file,
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
        /* [in] */ Elastos::IO::IFile* file);

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
        /* [in] */ Elastos::IO::IFile* file,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charset);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetContentType(
        /* [out] */ String* type);

    CARAPI GetCharSet(
        /* [out] */ String* charset);

    CARAPI GetTransferEncoding(
        /* [out] */ String* transferEncoding);

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI Send(
        /* [in] */ Elastos::IO::IOutputStream* outStream);

    CARAPI Length(
        /* [out] */ Int64* length);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetCharSet(
        /* [in] */ const String& charSet);

    CARAPI SetContentType(
        /* [in] */ const String& contentType);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI SetTransferEncoding(
        /* [in] */ const String& transferEncoding);

protected:
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

private:
    CARAPI_(void) InitStaticLOG();

    CARAPI Init(
        /* [in] */ const String& name,
        /* [in] */ IPartSource* partSource,
        /* [in] */ const String& contentType,
        /* [in] */ const String& charset);

protected:
    /** Attachment's file name */
     static const String FILE_NAME;

private:
    /** Log object for this class. */
    static const AutoPtr<ILog> LOG;

    /** Attachment's file name as a byte array */
    static const AutoPtr<ArrayOf<Byte> > FILE_NAME_BYTES;

    /** Source of the file part. */
    AutoPtr<IPartSource> mSource;
};

}
}
}
}
}
}

#endif // __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CFILEPART_H__
