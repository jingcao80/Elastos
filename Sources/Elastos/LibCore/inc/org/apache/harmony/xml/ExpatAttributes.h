#ifndef __ORG_APACHE_HARMONY_XML_EXPATATTRIBUTES_H__
#define __ORG_APACHE_HARMONY_XML_EXPATATTRIBUTES_H__

#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.External.h"

using Org::Xml::Sax::IAttributes;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
/**
 * Wraps native attribute array.
 */
class ExpatAttributes
    : public Object
    , public IExpatAttributes
    , public IAttributes
{
public:
    CAR_INTERFACE_DECL();

    CARAPI GetURI(
        /* [in] */ Int32 index,
        /* [out] */ String* URI);

    CARAPI GetLocalName(
        /* [in] */ Int32 index,
        /* [out] */ String* localName);

    CARAPI GetQName(
        /* [in] */ Int32 index,
        /* [out] */ String* qName);

    CARAPI GetType(
        /* [in] */ Int32 index,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */  Int32 index,
        /* [out] */ String* value);

    CARAPI GetIndex(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ Int32* index);

    CARAPI GetIndex(
        /* [in] */ const String& qName,
        /* [out] */ Int32* index);

    CARAPI GetType(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* type);

    CARAPI GetType(
        /* [in] */ const String& qName,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* value);

    CARAPI GetValue(
        /* [in] */ const String& qName,
        /* [out] */ String* value);

private:
    //private static native String getURI(long pointer, long attributePointer, int index);
    static CARAPI_(String) GetURI(
        /* [in] */ Int64 pointer,
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 index);
    //private static native String getLocalName(long pointer, long attributePointer, int index);
    static CARAPI_(String) GetLocalName(
        /* [in] */ Int64 pointer,
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 index);
    //private static native String getQName(long pointer, long attributePointer, int index);
    static CARAPI_(String) GetQName(
        /* [in] */ Int64 pointer,
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 index);
    //private static native String getValueByIndex(long attributePointer, int index);
    static CARAPI_(String) GetValueByIndex(
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 index);
    //private static native int getIndex(long attributePointer, String uri, String localName);
    static CARAPI_(Int32) GetIndex(
        /* [in] */ Int64 attributePointer,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName);
    //private static native int getIndexForQName(long attributePointer, String qName);
    static CARAPI_(Int32) GetIndexForQName(
        /* [in] */ Int64 attributePointer,
        /* [in] */ const String& qName);
    //private static native String getValue(long attributePointer, String uri, String localName);
    static CARAPI_(String) GetValue(
        /* [in] */ Int64 attributePointer,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName);
    //private static native String getValueForQName(long attributePointer, String qName);
    static CARAPI_(String) GetValueForQName(
        /* [in] */ Int64 attributePointer,
        /* [in] */ const String& qName);

protected:
    //protected native void freeAttributes(long pointer);
    CARAPI_(void) FreeAttributes(
        /* [in] */ Int64 pointer);

private:
    /**
     * Since we don't do validation, pretty much everything is CDATA type.
     */
    static const String CDATA;// = "CDATA";
};

} // namespace Xml
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_XML_EXPATATTRIBUTES_H__
