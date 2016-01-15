#ifndef __ELASTOS_DROID_UTILITY_XML_H__
#define __ELASTOS_DROID_UTILITY_XML_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Org::Xml::Sax::IContentHandler;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParserFactory;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * XML utility methods.
 */
class ECO_PUBLIC Xml
{
public:
    /** Factory for xml serializers. Initialized on demand. */
    class XmlSerializerFactory {
    public:
        static const String TYPE;// = "org.kxml2.io.KXmlParser,org.kxml2.io.KXmlSerializer";

        static AutoPtr<IXmlPullParserFactory> GetInstance();

    private:
        static AutoPtr<IXmlPullParserFactory> mInstance;
    };

    /**
     * Supported character encodings.
     */
    class Encoding {
    public:
        static const Encoding US_ASCII;//("US-ASCII"),
        static const Encoding UTF_8;//("UTF-8"),
        static const Encoding UTF_16;//("UTF-16"),
        static const Encoding ISO_8859_1;//("ISO-8859-1");

        String mExpatName;

        Encoding(
            /* [in] */ const String& expatName)
            : mExpatName(expatName)
        {
        }
    };

public:

    /**
     * {@link org.xmlpull.v1.XmlPullParser} "relaxed" feature name.
     *
     * @see <a href="http://xmlpull.org/v1/doc/features.html#relaxed">
     *  specification</a>
     */
    static String FEATURE_RELAXED;// = "http://xmlpull.org/v1/doc/features.html#relaxed";

    /**
     * Parses the given xml string and fires events on the given SAX handler.
     */
    static CARAPI Parse(
        /* [in] */ const String& xml,
        /* [in] */ IContentHandler* contentHandler);

    /**
     * Parses xml from the given reader and fires events on the given SAX
     * handler.
     */
    static CARAPI Parse(
        /* [in] */ IReader* in,
        /* [in] */ IContentHandler* contentHandler);

    /**
     * Parses xml from the given input stream and fires events on the given SAX
     * handler.
     */
    static CARAPI Parse(
        /* [in] */ IInputStream* in,
        /* [in] */ const Encoding& encoding,
        /* [in] */ IContentHandler* contentHandler);

    /**
     * Returns a new pull parser with namespace support.
     */
    static CARAPI NewPullParser(
        /* [out] */ IXmlPullParser** parser);

    /**
     * Creates a new xml serializer.
     */
    static CARAPI NewSerializer(
        /* [out] */ IXmlSerializer** xs);

    /**
     * Finds an encoding by name. Returns UTF-8 if you pass {@code null}.
     */
    static Encoding FindEncodingByName(
        /* [in] */ const String& encodingName);

    /**
     * Return an AttributeSet interface for use with the given XmlPullParser.
     * If the given parser itself implements AttributeSet, that implementation
     * is simply returned.  Otherwise a wrapper class is
     * instantiated on top of the XmlPullParser, as a proxy for retrieving its
     * attributes, and returned to you.
     *
     * @param parser The existing parser for which you would like an
     *               AttributeSet.
     *
     * @return An AttributeSet you can use to retrieve the
     *         attribute values at each of the tags as the parser moves
     *         through its XML document.
     *
     * @see AttributeSet
     */
    static AutoPtr<IAttributeSet> AsAttributeSet(
        /* [in] */ IXmlPullParser* parser);

private:
    ECO_LOCAL Xml();
    ECO_LOCAL Xml(const Xml&);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_XML_H__
