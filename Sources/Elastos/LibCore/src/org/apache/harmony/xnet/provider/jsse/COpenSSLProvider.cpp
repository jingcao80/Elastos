
#include "COpenSSLProvider.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLProvider)

CAR_INTERFACE_IMPL(COpenSSLProvider, Object, IOpenSSLProvider)

ECode COpenSSLProvider::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetPropertyEx(
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::List(
    /* [in] */ Elastos::IO::IPrintStream * pOutstream)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::ListEx(
    /* [in] */ Elastos::IO::IPrintWriter * pOutwriter)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::Load(
    /* [in] */ Elastos::IO::IInputStream * pInstream)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::LoadEx(
    /* [in] */ Elastos::IO::IReader * pInreader)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::PropertyNames(
    /* [out] */ IInterface ** ppInter)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::StringPropertyNames(
    /* [out] */ Elastos::Utility::IObjectStringMap ** ppStrmap)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::Save(
    /* [in] */ Elastos::IO::IOutputStream * pOutstream,
    /* [in] */ const String& comment)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ IObject ** ppObj)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::Store(
    /* [in] */ Elastos::IO::IOutputStream * pOutstream,
    /* [in] */ const String& comment)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::StoreEx(
    /* [in] */ Elastos::IO::IWriter * pWriter,
    /* [in] */ const String& comment)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::LoadFromXML(
    /* [in] */ Elastos::IO::IInputStream * pInstream)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::StoreToXML(
    /* [in] */ Elastos::IO::IOutputStream * pOs,
    /* [in] */ const String& comment)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::StoreToXMLEx(
    /* [in] */ Elastos::IO::IOutputStream * pOs,
    /* [in] */ const String& comment,
    /* [in] */ const String& encoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetName(
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetVersion(
    /* [out] */ Double * pVersionNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetInfo(
    /* [out] */ String * pInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetService(
    /* [in] */ const String& type,
    /* [in] */ const String& algorithm,
    /* [out] */ Elastos::Security::IProviderService ** ppService)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::GetServices(
    /* [out] */ Elastos::Utility::ISet ** ppServices)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLProvider::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

