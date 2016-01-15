
#ifndef __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CSTRINGPART_H__
#define __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CSTRINGPART_H__

#include "_Elastos_Droid_Net_Internal_Http_Multipart_CStringPart.h"
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
 * Simple string parameter for a multipart post
 *
 * @author <a href="mailto:mattalbright@yahoo.com">Matthew Albright</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 */
CarClass(CStringPart), public PartBase
{
    friend class CStringPartHelper;

public:
    /**
     * Constructor.
     *
     * @param name The name of the part
     * @param value the string to post
     * @param charset the charset to be used to encode the string, if <code>null</code>
     * the {@link #DEFAULT_CHARSET default} is used
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ const String& charset);

    /**
     * Constructor.
     *
     * @param name The name of the part
     * @param value the string to post
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /* (non-Javadoc)
     * @see org.apache.commons.httpclient.methods.multipart.BasePart#setCharSet(java.lang.String)
     */
    CARAPI SetCharSet(
        /* [in] */ const String& charSet);

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

    CARAPI SetContentType(
        /* [in] */ const String& contentType);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI SetTransferEncoding(
        /* [in] */ const String& transferEncoding);

protected:
    /**
     * Writes the data to the given OutputStream.
     * @param out the OutputStream to write to
     * @throws IOException if there is a write error
     */
    CARAPI SendData(
        /* [in] */ IOutputStream* outStream);

    /**
     * Return the length of the data.
     * @return The length of the data.
     * @see Part#lengthOfData()
     */
    CARAPI LengthOfData(
        /* [out] */ Int64* length);

private:
    CARAPI_(void) InitStaticLOG();

    /**
     * Gets the content in bytes.  Bytes are lazily created to allow the charset to be changed
     * after the part is created.
     *
     * @return the content in bytes
     */
    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetContent();

private:
    /** Log object for this class. */
    static const AutoPtr<ILog> LOG;

    /** Contents of this StringPart. */
    AutoPtr<ArrayOf<Byte> > mContent;

    /** The String value of this part. */
    String mValue;
};

}
}
}
}
}
}

#endif // __ELASTOS_DROID_NET_INTERNAL_HTTP_MULTIPART_CSTRINGPART_H__
