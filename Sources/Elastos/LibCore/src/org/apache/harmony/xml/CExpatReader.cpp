
#include "CExpatReader.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

CAR_OBJECT_IMPL(CExpatReader)

CAR_INTERFACE_IMPL(CExpatReader, Object, IExpatReader)

ECode CExpatReader::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pFeature)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetEntityResolver(
    /* [in] */ Org::Xml::Sax::IEntityResolver * pResolver)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetEntityResolver(
    /* [out] */ Org::Xml::Sax::IEntityResolver ** ppResolver)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetDTDHandler(
    /* [in] */ Org::Xml::Sax::IDTDHandler * pHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetDTDHandler(
    /* [out] */ Org::Xml::Sax::IDTDHandler ** ppHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetContentHandler(
    /* [in] */ Org::Xml::Sax::IContentHandler * pHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetContentHandler(
    /* [out] */ Org::Xml::Sax::IContentHandler ** ppHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetErrorHandler(
    /* [in] */ Org::Xml::Sax::IErrorHandler * pHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetErrorHandler(
    /* [out] */ Org::Xml::Sax::IErrorHandler ** ppHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::Parse(
    /* [in] */ Org::Xml::Sax::IInputSource * pInput)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::ParseEx(
    /* [in] */ const String& systemId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::GetLexicalHandler(
    /* [out] */ Org::Xml::Sax::Ext::ILexicalHandler ** ppHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetLexicalHandler(
    /* [in] */ Org::Xml::Sax::Ext::ILexicalHandler * pLexicalHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::IsNamespaceProcessingEnabled(
    /* [out] */ Boolean * pEnabled)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExpatReader::SetNamespaceProcessingEnabled(
    /* [in] */ Boolean processNamespaces)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

