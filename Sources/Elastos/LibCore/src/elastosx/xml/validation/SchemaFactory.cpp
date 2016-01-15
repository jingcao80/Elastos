
#include "Elastos.CoreLibrary.Net.h"
#include "SchemaFactory.h"
#include "stream/CStreamSource.h"
#include "utility/logging/Slogger.h"
#include "Thread.h"
#include "CPathClassLoader.h"
#include "SchemaFactoryFinder.h"

using Elastos::Core::Thread;
using Elastos::Core::CPathClassLoader;
using Elastosx::Xml::Transform::Stream::IStreamSource;
using Elastosx::Xml::Transform::Stream::CStreamSource;

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_INTERFACE_IMPL(SchemaFactory, Object, ISchemaFactory)

ECode SchemaFactory::GetFeature(
    /* [in] */ String name,
    /* [out] */ Boolean* feature)
{
    *feature = FALSE;
    if (name == NULL) {
        SLOGGERD("SchemaFactory", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode SchemaFactory::SetFeature(
    /* [in] */ String name,
    /* [in] */ Boolean value)
{
    if (name == NULL) {
        SLOGGERD("SchemaFactory", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode SchemaFactory::SetProperty(
    /* [in] */ String name,
    /* [in] */ IInterface* object)
{
    if (name == NULL) {
        SLOGGERD("SchemaFactory", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode SchemaFactory::GetProperty(
    /* [in] */ String name,
    /* [out] */ IInterface** property)
{
    *property = NULL;
    if (name == NULL) {
        SLOGGERD("SchemaFactory", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode SchemaFactory::NewSchema(
    /* [in] */ ISource* schema,
    /* [out] */ ISchema** instance)
{
    AutoPtr<ArrayOf<ISource*> > pArr = ArrayOf<ISource*>::Alloc(1);
    pArr->Set(0, schema);
    return NewSchema(pArr, instance);
}

ECode SchemaFactory::NewSchema(
    /* [in] */ IFile* schema,
    /* [out] */ ISchema** instance)
{
    AutoPtr<ISource> ss;
    CStreamSource::New(schema, (ISource**)&ss);
    return NewSchema(ss, instance);
}

ECode SchemaFactory::NewSchema(
    /* [in] */ IURL* schema,
    /* [out] */ ISchema** instance)
{
    String str;
    schema->ToExternalForm(&str);
    AutoPtr<ISource> ss;
    CStreamSource::New(str, (ISource**)&ss);
    return NewSchema(ss, instance);
}

ECode SchemaFactory::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [out] */ ISchemaFactory** factory)
{
    AutoPtr<IClassLoader> cl;
    if (cl == NULL) {
        FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&cl))
    }

    AutoPtr<ISchemaFactoryFinder> finder = new SchemaFactoryFinder(cl);
    finder->NewFactory(schemaLanguage, factory);
    if (*factory == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SchemaFactory::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ ISchemaFactory** factory)
{
    if (schemaLanguage == NULL) {
        SLOGGERD("SchemaFactory", "schemaLanguage == null")
        return E_NULL_POINTER_EXCEPTION;
    } else if (factoryClassName == NULL) {
        SLOGGERD("SchemaFactory", "factoryClassName == null")
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IClassLoader> pLoader = classLoader;
    if (pLoader == NULL) {
        FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&pLoader))
    }
        if (pLoader == NULL) {
            String path = factoryClassName.Substring(0, factoryClassName.LastIndexOf('.'));
            FAIL_RETURN(CPathClassLoader::New(path, (IClassLoader**)&pLoader))
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
        *factory = ISchemaFactory::Probe(tmp);
        if (*factory == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Boolean spted;
        (*factory)->IsSchemaLanguageSupported(schemaLanguage, &spted);
        if (spted) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        REFCOUNT_ADD(*factory)
        return NOERROR;
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx