
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMErrorImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::W3c::Dom::IDOMLocator;
using Org::W3c::Dom::IDOMError;
using Org::W3c::Dom::INode;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMErrorImpl)
    , public Object
    , public IDOMError
{
private:
    class InnerDOMLocator
        : public Object
        , public IDOMLocator
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI GetLineNumber(
            /* [out] */ Int32* result);

        CARAPI GetColumnNumber(
            /* [out] */ Int32* result);

        CARAPI GetByteOffset(
            /* [out] */ Int32* result);

        CARAPI GetUtf16Offset(
            /* [out] */ Int32* result);

        CARAPI GetRelatedNode(
            /* [out] */ INode** result);

        CARAPI GetUri(
            /* [out] */ String* result);
    };

public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CARAPI GetSeverity(
        /* [out] */ Int16 * pValue);

    CARAPI GetMessage(
        /* [out] */ String * pStr);

    CARAPI GetType(
        /* [out] */ String * pStr);

    CARAPI GetRelatedException(
        /* [out] */ IObject ** ppObj);

    CARAPI GetRelatedData(
        /* [out] */ IObject ** ppObj);

    CARAPI GetLocation(
        /* [out] */ Org::W3c::Dom::IDOMLocator ** ppDomloc);

    CARAPI constructor(
        /* [in] */ Int32 severity,
        /* [in] */ const String& type);

private:
    static AutoPtr<IDOMLocator> NULL_DOM_LOCATOR;

    Int16 severity;
    String type;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
