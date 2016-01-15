
#include "org/apache/http/auth/CNTUserPrincipal.h"
#include "org/apache/http/utility/LangUtils.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Security::EIID_IPrincipal;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::LangUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAR_INTERFACE_IMPL_2(CNTUserPrincipal, Object, INTUserPrincipal, IPrincipal)

CAR_OBJECT_IMPL(CNTUserPrincipal)

ECode CNTUserPrincipal::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mNtname;
    return NOERROR;
}

ECode CNTUserPrincipal::GetDomain(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mDomain;
    return NOERROR;
}

ECode CNTUserPrincipal::GetUserName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mUserName;
    return NOERROR;
}

ECode CNTUserPrincipal::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = LangUtils::HASH_SEED;
    AutoPtr<ICharSequence> namecs, domaincs;
    CString::New(mUserName, (ICharSequence**)&namecs);
    CString::New(mDomain, (ICharSequence**)&domaincs);
    hash = LangUtils::HashCode(hash, IObject::Probe(namecs));
    hash = LangUtils::HashCode(hash, IObject::Probe(domaincs));
    *hashCode = hash;
    return NOERROR;
}

ECode CNTUserPrincipal::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(*equals)
    if (obj == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    if (this->Probe(EIID_IInterface) == obj) {
        *equals = TRUE;
        return NOERROR;
    }
    if (INTUserPrincipal::Probe(obj) != NULL) {
        AutoPtr<INTUserPrincipal> that = INTUserPrincipal::Probe(obj);
        String thatUsername;
        that->GetUserName(&thatUsername);
        AutoPtr<ICharSequence> namecs, thatNamecs;
        CString::New(mUserName, (ICharSequence**)&namecs);
        CString::New(thatUsername, (ICharSequence**)&thatNamecs);
        if (LangUtils::Equals(IObject::Probe(namecs), IObject::Probe(thatNamecs))) {
            String thatDomain;
            that->GetDomain(&thatDomain);
            AutoPtr<ICharSequence> domaincs, thatDomaincs;
            CString::New(mDomain, (ICharSequence**)&domaincs);
            CString::New(thatDomain, (ICharSequence**)&thatDomaincs);
            if (LangUtils::Equals(IObject::Probe(domaincs), IObject::Probe(thatDomaincs))) {
                *equals = TRUE;
                return NOERROR;
            }
        }
    }
    *equals = FALSE;
    return NOERROR;
}

ECode CNTUserPrincipal::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    *string = mNtname;
    return NOERROR;
}

ECode CNTUserPrincipal::constructor(
    /* [in] */ const String& domain,
    /* [in] */ const String& userName)
{
    if (userName.IsNull()) {
        Logger::E("CNTUserPrincipal", "User name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mUserName = userName;
    if (!domain.IsNull()) {
        mDomain = domain.ToUpperCase(/*ILocale::ENGLISH*/);
    }
    else {
        mDomain = String(NULL);
    }
    if (!mDomain.IsNull() && mDomain.GetLength() > 0) {
        StringBuilder buffer;
        buffer.Append(mDomain);
        buffer.AppendChar('/');
        buffer.Append(mUserName);
        mNtname = buffer.ToString();
    }
    else {
        mNtname = mUserName;
    }
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
