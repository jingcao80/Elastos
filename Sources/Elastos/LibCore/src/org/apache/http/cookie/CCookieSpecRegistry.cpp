
#include "org/apache/http/cookie/CCookieSpecRegistry.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CString.h"
#include "elastos/core/AutoLock.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CLinkedHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;
using Elastos::Utility::ISet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::CLinkedHashMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

CAR_INTERFACE_IMPL(CCookieSpecRegistry, Object, ICookieSpecRegistry)

CAR_OBJECT_IMPL(CCookieSpecRegistry)

ECode CCookieSpecRegistry::Register(
    /* [in] */ const String& name,
    /* [in] */ ICookieSpecFactory* factory)
{
    synchronized(this) {
        if (name.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Name may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (factory == NULL) {
            Logger::E("CCookieSpecRegistry", "Cookie spec factory may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        mRegisteredSpecs->Put(cs, factory);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::Unregister(
    /* [in] */ const String& id)
{
    synchronized(this) {
        if (id.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Id may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(id.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        mRegisteredSpecs->Remove(cs);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetCookieSpec(
    /* [in] */ const String& name,
    /* [in] */ IHttpParams* params,
    /* [out] */ ICookieSpec** spec)
{
    synchronized(this) {
        VALIDATE_NOT_NULL(spec)
        *spec = NULL;

        if (name.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Name may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        AutoPtr<IInterface> temp;
        mRegisteredSpecs->Get(cs, (IInterface**)&temp);
        AutoPtr<ICookieSpecFactory> factory = ICookieSpecFactory::Probe(temp);
        if (factory != NULL) {
            return factory->NewInstance(params, spec);
        }
        else {
            Logger::E("CCookieSpecRegistry", "Unsupported cookie spec: %s", name.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetCookieSpec(
    /* [in] */ const String& name,
    /* [out] */ ICookieSpec** spec)
{
    synchronized(this) {
        VALIDATE_NOT_NULL(spec)
        GetCookieSpec(name, NULL, spec);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetSpecNames(
    /* [out] */ IList** names)
{
    VALIDATE_NOT_NULL(names)
    synchronized(this) {
        AutoPtr<ISet> keySet;
        mRegisteredSpecs->GetKeySet((ISet**)&keySet);
        AutoPtr<ICollection> col = ICollection::Probe(keySet);
        AutoPtr<IArrayList> al;
        CArrayList::New(col, (IArrayList**)&al);
        *names = IList::Probe(al);
        REFCOUNT_ADD(*names)
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::SetItems(
    /* [in] */ IMap* map)
{
    synchronized(this) {
        if (map == NULL) {
            return NOERROR;
        }
        mRegisteredSpecs->Clear();
        mRegisteredSpecs->PutAll(map);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::constructor()
{
    CLinkedHashMap::New((IMap**)&mRegisteredSpecs);
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
