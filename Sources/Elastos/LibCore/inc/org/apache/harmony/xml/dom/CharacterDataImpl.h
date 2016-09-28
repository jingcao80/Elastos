#ifndef _ORG_APACHE_HARMONY_XML_DOM_CHARACTERDATAIMPL_H__
#define _ORG_APACHE_HARMONY_XML_DOM_CHARACTERDATAIMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/dom/LeafNodeImpl.h"
#include "elastos/core/StringBuffer.h"

using Org::W3c::Dom::ICharacterData;
using Org::W3c::Dom::IDocument;
using Elastos::Core::IStringBuilder;
using Elastos::Core::StringBuffer;
//import org.w3c.dom.DOMException;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

/**
 * Provides a straightforward implementation of the corresponding W3C DOM
 * interface. The class is used internally only, thus only notable members that
 * are not in the original interface are documented (the W3C docs are quite
 * extensive). Hope that's ok.
 * <p>
 * Some of the fields may have package visibility, so other classes belonging to
 * the DOM implementation can easily access them while maintaining the DOM tree
 * structure.
 */
class CharacterDataImpl
    : public LeafNodeImpl
    , public ICharacterData
    , public ICharacterDataImpl
{
public:
    CAR_INTERFACE_DECL();

    CharacterDataImpl();

    CARAPI constructor(
        /* [in] */ IDocument* document,
        /* [in] */ const String& data);

    CARAPI AppendData(
        /* [in] */ const String& arg);

    CARAPI DeleteData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI GetData(
        /* [out] */ String* result);

    /**
     * Appends this node's text content to the given builder.
     */
    CARAPI AppendDataTo(
        /* [in] */ IStringBuilder* stringBuilder);

    CARAPI GetLength(
        /* [out] */ Int32* result);

    CARAPI GetNodeValue(
        /* [out] */ String* result);

    CARAPI InsertData(
        /* [in] */ Int32 offset,
        /* [in] */ const String& arg);

    CARAPI ReplaceData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ const String& arg);

    CARAPI SetData(
        /* [in] */ const String& data);

    CARAPI SubstringData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ String* result);

protected:
    AutoPtr<StringBuffer> buffer;

    friend class CDOMConfigurationImpl;

};

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org
#endif // _ORG_APACHE_HARMONY_XML_DOM_CHARACTERDATAIMPL_H__
