
#include "org/apache/http/protocol/BasicHttpContext.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(BasicHttpContext, Object, IHttpContext)

ECode BasicHttpContext::Init()
{
    return Init(NULL);
}

ECode BasicHttpContext::Init(
    /* [in] */ IHttpContext* parentContext)
{
    mParentContext = parentContext;
    return NOERROR;
}

ECode BasicHttpContext::GetAttribute(
    /* [in] */ const String& id,
    /* [out] */ IInterface** attr)
{
    VALIDATE_NOT_NULL(attr)
    if (id.IsNull()) {
        Logger::E("BasicHttpContext", "Id may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> obj;
    if (mMap != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(id, (ICharSequence**)&cs);
        mMap->Get(cs, (IInterface**)&obj);
    }
    if (obj == NULL && mParentContext != NULL) {
        mParentContext->GetAttribute(id, (IInterface**)&obj);
    }
    *attr = obj;
    REFCOUNT_ADD(*attr)
    return NOERROR;
}

ECode BasicHttpContext::SetAttribute(
    /* [in] */ const String& id,
    /* [in] */ IInterface* obj)
{
    if (id.IsNull()) {
        Logger::E("BasicHttpContext", "Id may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mMap == NULL) {
        AutoPtr<IHashMap> hm;
        CHashMap::New((IHashMap**)&hm);
        mMap = IMap::Probe(hm);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(id, (ICharSequence**)&cs);
    mMap->Put(cs, obj);
    return NOERROR;
}

ECode BasicHttpContext::RemoveAttribute(
    /* [in] */ const String& id,
    /* [out] */ IInterface** attrs)
{
    VALIDATE_NOT_NULL(attrs)
    *attrs = NULL;

    if (id.IsNull()) {
        Logger::E("BasicHttpContext", "Id may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mMap != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(id, (ICharSequence**)&cs);
        return mMap->Remove(cs, attrs);
    }
    else {
        return NOERROR;
    }
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
