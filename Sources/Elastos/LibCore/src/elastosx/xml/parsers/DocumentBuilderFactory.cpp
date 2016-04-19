#include "DocumentBuilderFactory.h"
#include "utility/logging/Slogger.h"
#include "Thread.h"
#include "CPathClassLoader.h"
// #include "org/apache/harmony/xml/parsers/CDocumentBuilderFactoryImpl.h"

// using Org::Apache::Harmony::Xml::Parsers::CDocumentBuilderFactoryImpl;
using Elastos::Core::Thread;
using Elastos::Core::CPathClassLoader;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_INTERFACE_IMPL(DocumentBuilderFactory, Object, IDocumentBuilderFactory)

DocumentBuilderFactory::DocumentBuilderFactory()
    : mValidating(FALSE)
    , mNamespaceAware(FALSE)
    , mWhitespace(FALSE)
    , mExpandEntityRef(TRUE)
    , mIgnoreComments(FALSE)
    , mCoalescing(FALSE)
{}

ECode DocumentBuilderFactory::SetNamespaceAware(
    /* [in] */ Boolean awareness)
{
    mNamespaceAware = awareness;
    return NOERROR;
}

ECode DocumentBuilderFactory::SetValidating(
    /* [in] */ Boolean validating)
{
    mValidating = validating;
    return NOERROR;
}

ECode DocumentBuilderFactory::SetIgnoringElementContentWhitespace(
    /* [in] */ Boolean whitespace)
{
    mWhitespace = whitespace;
    return NOERROR;
}

ECode DocumentBuilderFactory::SetExpandEntityReferences(
    /* [in] */ Boolean expandEntityRef)
{
    mExpandEntityRef = expandEntityRef;
    return NOERROR;
}

ECode DocumentBuilderFactory::SetIgnoringComments(
    /* [in] */ Boolean ignoreComments)
{
    mIgnoreComments = ignoreComments;
    return NOERROR;
}

ECode DocumentBuilderFactory::SetCoalescing(
    /* [in] */ Boolean coalescing)
{
    mCoalescing = coalescing;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsNamespaceAware(
    /* [out] */ Boolean* isAware)
{
    *isAware = mNamespaceAware;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsValidating(
    /* [out] */ Boolean* isValidating)
{
    *isValidating = mValidating;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsIgnoringElementContentWhitespace(
    /* [out] */ Boolean* isIgnoringElementContentWhiteSpace)
{
    *isIgnoringElementContentWhiteSpace = mWhitespace;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsExpandEntityReferences(
    /* [out] */ Boolean* isExpandEntityReferences)
{
    *isExpandEntityReferences = mExpandEntityRef;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsIgnoringComments(
    /* [out] */ Boolean* comments)
{
    *comments = mIgnoreComments;
    return NOERROR;
}

ECode DocumentBuilderFactory::IsCoalescing(
    /* [out] */ Boolean* isCoalescing)
{
    *isCoalescing = mCoalescing;
    return NOERROR;
}

ECode DocumentBuilderFactory::GetSchema(
    /* [out] */ ISchema** schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilderFactory::SetSchema(
    /* [in] */ ISchema* schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilderFactory::SetXIncludeAware(
    /* [in] */ Boolean state)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilderFactory::IsXIncludeAware(
    /* [out] */ Boolean* isXIncludeAware)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilderFactory::NewInstance(
    /* [out] */ IDocumentBuilderFactory** instance)
{
    // return CDocumentBuilderFactoryImpl::New(instance);
    return NOERROR;
}

ECode DocumentBuilderFactory::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ IDocumentBuilderFactory** instance)
{
    *instance = NULL;
    if (factoryClassName == NULL) {
        SLOGGERD("DocumentBuilderFactory", "factoryClassName == null");
        return E_FACTORY_CONFIGURATION_EXCEPTION;
    }

    AutoPtr<IClassLoader> pLoader = classLoader;
    if (pLoader == NULL) {
        FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&pLoader))
    }

    // try {
        if (pLoader == NULL) {
            String path = factoryClassName.Substring(0, factoryClassName.LastIndexOf('.'));
            FAIL_RETURN(CPathClassLoader::New(path, NULL, (IClassLoader**)&pLoader))
        }

        AutoPtr<IClassInfo> clsInfo;
        FAIL_RETURN(pLoader->LoadClass(factoryClassName, (IClassInfo**)&clsInfo))
        if (clsInfo == NULL) {
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
        ClassID id;
        clsInfo->GetId(&id);
        AutoPtr<IInterface> tmp;
        FAIL_RETURN(CObject::CreateInstance(id, RGM_SAME_DOMAIN, EIID_IInterface, (IInterface**)&tmp))
        *instance = IDocumentBuilderFactory::Probe(tmp);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    //     Class<?> type = classLoader != null
    //             ? classLoader.loadClass(factoryClassName)
    //             : Class.forName(factoryClassName);
    //     return (DocumentBuilderFactory) type.newInstance();
    // } catch (ClassNotFoundException e) {
    //     throw new FactoryConfigurationError(e);
    // } catch (InstantiationException e) {
    //     throw new FactoryConfigurationError(e);
    // } catch (IllegalAccessException e) {
    //     throw new FactoryConfigurationError(e);
    // }
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx