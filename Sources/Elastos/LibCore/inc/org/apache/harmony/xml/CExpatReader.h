
#ifndef __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__
#define __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__

#include "_Org_Apache_Harmony_Xml_CExpatReader.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

CarClass(CExpatReader)
    , public Object
    , public IExpatReader
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pFeature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppValue);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface * pValue);

    CARAPI SetEntityResolver(
        /* [in] */ Org::Xml::Sax::IEntityResolver * pResolver);

    CARAPI GetEntityResolver(
        /* [out] */ Org::Xml::Sax::IEntityResolver ** ppResolver);

    CARAPI SetDTDHandler(
        /* [in] */ Org::Xml::Sax::IDTDHandler * pHandler);

    CARAPI GetDTDHandler(
        /* [out] */ Org::Xml::Sax::IDTDHandler ** ppHandler);

    CARAPI SetContentHandler(
        /* [in] */ Org::Xml::Sax::IContentHandler * pHandler);

    CARAPI GetContentHandler(
        /* [out] */ Org::Xml::Sax::IContentHandler ** ppHandler);

    CARAPI SetErrorHandler(
        /* [in] */ Org::Xml::Sax::IErrorHandler * pHandler);

    CARAPI GetErrorHandler(
        /* [out] */ Org::Xml::Sax::IErrorHandler ** ppHandler);

    CARAPI Parse(
        /* [in] */ Org::Xml::Sax::IInputSource * pInput);

    CARAPI ParseEx(
        /* [in] */ const String& systemId);

    CARAPI GetLexicalHandler(
        /* [out] */ Org::Xml::Sax::Ext::ILexicalHandler ** ppHandler);

    CARAPI SetLexicalHandler(
        /* [in] */ Org::Xml::Sax::Ext::ILexicalHandler * pLexicalHandler);

    CARAPI IsNamespaceProcessingEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI SetNamespaceProcessingEnabled(
        /* [in] */ Boolean processNamespaces);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__
