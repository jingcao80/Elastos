
#include "org/apache/http/auth/CAuthSchemeRegistry.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CLinkedHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAuthSchemeRegistry::CAuthSchemeRegistry()
    : Object()
{}

CAR_INTERFACE_IMPL(CAuthSchemeRegistry, Object, IAuthSchemeRegistry)

CAR_OBJECT_IMPL(CAuthSchemeRegistry)

ECode CAuthSchemeRegistry::Register(
    /* [in] */ const String& name,
    /* [in] */ IAuthSchemeFactory* factory)
{
    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (factory == NULL) {
        Logger::E("CAuthSchemeRegistry", "Authentication scheme factory may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> oldValue;
    return mRegisteredSchemes->Put(cs, factory, (IInterface**)&oldValue);
}

ECode CAuthSchemeRegistry::Unregister(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    return mRegisteredSchemes->Remove(cs, (IInterface**)&value);
}

ECode CAuthSchemeRegistry::GetAuthScheme(
    /* [in] */ const String& name,
    /* [in] */ IHttpParams* params,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    *scheme = NULL;

    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> factory;
    mRegisteredSchemes->Get(cs, (IInterface**)&factory);
    if (factory != NULL) {
        return IAuthSchemeFactory::Probe(factory)->NewInstance(params, scheme);
    }
    else {
        Logger::E("CAuthSchemeRegistry", "Unsupported authentication scheme: %s", name.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CAuthSchemeRegistry::GetSchemeNames(
    /* [out] */ IList** names)
{
    VALIDATE_NOT_NULL(names)

    AutoPtr<ISet> set;
    mRegisteredSchemes->GetKeySet((ISet**)&set);
    AutoPtr<ICollection> col = ICollection::Probe(set);
    AutoPtr<IArrayList> aList;
    CArrayList::New(col, (IArrayList**)&aList);
    *names = IList::Probe(aList);
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode CAuthSchemeRegistry::SetItems(
    /* [in] */ IMap* map) // will exit?
{
    if (map == NULL) {
        return NOERROR;
    }
    mRegisteredSchemes->Clear();
    mRegisteredSchemes->PutAll(map);
    return NOERROR;
}

ECode CAuthSchemeRegistry::constructor()
{
    mRegisteredSchemes = NULL;
    CLinkedHashMap::New((IMap**)&mRegisteredSchemes);
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
