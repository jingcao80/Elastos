
#include "org/apache/http/auth/CNTCredentials.h"
#include "org/apache/http/auth/CNTUserPrincipal.h"
#include "org/apache/http/utility/LangUtils.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::LangUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAR_INTERFACE_IMPL_2(CNTCredentials, Object, INTCredentials, ICredentials)

CAR_OBJECT_IMPL(CNTCredentials)

ECode CNTCredentials::GetUserPrincipal(
    /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)
    *principal = IPrincipal::Probe(mPrincipal);
    REFCOUNT_ADD(*principal)
    return NOERROR;
}

ECode CNTCredentials::GetPassword(
    /* [out] */ String* password)
{
    VALIDATE_NOT_NULL(password)
    *password = mPassword;
    return NOERROR;
}

ECode CNTCredentials::GetUserName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return mPrincipal->GetUserName(name);
}

ECode CNTCredentials::GetDomain(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return mPrincipal->GetDomain(name);
}

ECode CNTCredentials::GetWorkstation(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mWorkstation;
    return NOERROR;
}

ECode CNTCredentials::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = LangUtils::HASH_SEED;
    hash = LangUtils::HashCode(hash, IObject::Probe(mPrincipal));
    AutoPtr<ICharSequence> cs;
    CString::New(mWorkstation, (ICharSequence**)&cs);
    hash = LangUtils::HashCode(hash, IObject::Probe(cs));
    *hashCode = hash;
    return NOERROR;
}

ECode CNTCredentials::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)
    if (obj == NULL) {
        *equals = FALSE;
        return NOERROR;
    }
    if (obj == TO_IINTERFACE(this)) {
        *equals = TRUE;
        return NOERROR;
    }
    if (INTCredentials::Probe(obj) != NULL) {
        AutoPtr<INTCredentials> that = INTCredentials::Probe(obj);
        AutoPtr<IPrincipal> thatPrincipal;
        ICredentials::Probe(that)->GetUserPrincipal((IPrincipal**)&thatPrincipal);
        if (LangUtils::Equals(IObject::Probe(mPrincipal), IObject::Probe(thatPrincipal))) {
            String thatWorkstation;
            that->GetWorkstation(&thatWorkstation);
            AutoPtr<ICharSequence> cs, thatCS;
            CString::New(mWorkstation, (ICharSequence**)&cs);
            CString::New(thatWorkstation, (ICharSequence**)&thatCS);
            if (LangUtils::Equals(IObject::Probe(cs), IObject::Probe(thatCS))) {
                *equals = TRUE;
                return NOERROR;
            }
        }
    }
    *equals = FALSE;
    return NOERROR;
}

ECode CNTCredentials::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    StringBuilder buffer;
    buffer.Append("[principal: ");
    buffer.Append(mPrincipal);
    buffer.Append("][workstation: ");
    buffer.Append(mWorkstation);
    buffer.Append("]");
    *string = buffer.ToString();
    return NOERROR;
}

ECode CNTCredentials::constructor(
    /* [in] */ const String& usernamePassword)
{
    if (usernamePassword.IsNull()) {
        Logger::E("CNTCredentials", "Username:password string may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String username;
    Int32 atColon = usernamePassword.IndexOf(':');
    if (atColon >= 0) {
        username = usernamePassword.Substring(0, atColon);
        mPassword = usernamePassword.Substring(atColon + 1);
    }
    else {
        username = usernamePassword;
        mPassword = String(NULL);
    }
    Int32 atSlash = username.IndexOf('/');
    mPrincipal = NULL;
    if (atSlash >= 0) {
        CNTUserPrincipal::New(username.Substring(0, atSlash).ToUpperCase(/*ILocale::ENGLISH*/),
                username.Substring(atSlash + 1), (INTUserPrincipal**)&mPrincipal);
    }
    else {
        CNTUserPrincipal::New(String(NULL), username.Substring(atSlash + 1), (INTUserPrincipal**)&mPrincipal);
    }
    mWorkstation = String(NULL);
    return NOERROR;
}

ECode CNTCredentials::constructor(
    /* [in] */ const String& userName,
    /* [in] */ const String& password,
    /* [in] */ const String& workstation,
    /* [in] */ const String& domain)
{
    if (userName.IsNull()) {
        Logger::E("CNTCredentials", "User name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPrincipal = NULL;
    CNTUserPrincipal::New(domain, userName, (INTUserPrincipal**)&mPrincipal);
    mPassword = password;
    if (!workstation.IsNull()) {
        mWorkstation = workstation.ToUpperCase(/*ILocale::ENGLISH*/);
    }
    else {
        mWorkstation = String(NULL);
    }
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
