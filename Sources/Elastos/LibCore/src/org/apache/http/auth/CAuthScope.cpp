
#include "org/apache/http/auth/CAuthScope.h"
#include "org/apache/http/utility/LangUtils.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::LangUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

static AutoPtr<IAuthScope> InitANY()
{
    AutoPtr<CAuthScope> scope;
    CAuthScope::NewByFriend(String(NULL)/*IAuthScope::ANY_HOST*/, IAuthScope::ANY_PORT,
            String(NULL)/*IAuthScope::ANY_REALM*/, String(NULL)/*IAuthScope::*/, (CAuthScope**)&scope);
    return (IAuthScope*)scope.Get();
}
const AutoPtr<IAuthScope> CAuthScope::ANY = InitANY();

CAR_INTERFACE_IMPL(CAuthScope, Object, IAuthScope)

CAR_OBJECT_IMPL(CAuthScope)

ECode CAuthScope::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host)
    *host = mHost;
    return NOERROR;
}

ECode CAuthScope::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = mPort;
    return NOERROR;
}

ECode CAuthScope::GetRealm(
    /* [out] */ String* realmName)
{
    VALIDATE_NOT_NULL(realmName)
    *realmName = mRealm;
    return NOERROR;
}

ECode CAuthScope::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme)
    *scheme = mScheme;
    return NOERROR;
}

ECode CAuthScope::Match(
    /* [in] */ IAuthScope* that,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 factor = 0;

    String thatScheme;
    that->GetScheme(&thatScheme);
    AutoPtr<ICharSequence> schemeCS, thatSchemeCS;
    CString::New(mScheme, (ICharSequence**)&schemeCS);
    CString::New(thatScheme, (ICharSequence**)&thatSchemeCS);
    if (LangUtils::Equals(IObject::Probe(schemeCS), IObject::Probe(thatSchemeCS))) {
        factor += 1;
    }
    else {
        if (mScheme != String(NULL)/*ANY_SCHEME*/ && thatScheme != String(NULL)/*ANY_SCHEME*/) {
            *value = -1;
            return NOERROR;
        }
    }

    String thatRealm;
    that->GetRealm(&thatRealm);
    AutoPtr<ICharSequence> realmCS, thatRealmCS;
    CString::New(mRealm, (ICharSequence**)&realmCS);
    CString::New(thatRealm, (ICharSequence**)&thatRealmCS);
    if (LangUtils::Equals(IObject::Probe(realmCS), IObject::Probe(thatRealmCS))) {
        factor += 2;
    }
    else {
        if (mRealm != String(NULL)/*ANY_REALM*/ && thatRealm != String(NULL)/*ANY_REALM*/) {
            *value = -1;
            return NOERROR;
        }
    }

    Int32 thatPort;
    that->GetPort(&thatPort);
    if (mPort == thatPort) {
        factor += 4;
    }
    else {
        if (mPort != ANY_PORT && thatPort != ANY_PORT) {
            *value = -1;
            return NOERROR;
        }
    }

    String thatHost;
    that->GetHost(&thatHost);
    AutoPtr<ICharSequence> hostCS, thatHostCS;
    CString::New(mHost, (ICharSequence**)&hostCS);
    CString::New(thatHost, (ICharSequence**)&thatHostCS);
    if (LangUtils::Equals(IObject::Probe(hostCS), IObject::Probe(thatHostCS))) {
        factor += 8;
    }
    else {
        if (mHost != String(NULL)/*ANY_HOST*/ && thatHost != String(NULL)/*ANY_HOST*/) {
            *value = -1;
            return NOERROR;
        }
    }

    *value = factor;
    return NOERROR;
}

ECode CAuthScope::Equals(
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
    if (IAuthScope::Probe(obj) == NULL) {
        return Object::Equals(obj, equals);
    }
    AutoPtr<IAuthScope> that = IAuthScope::Probe(obj);
    String thatHost, thatRealm, thatScheme;
    that->GetHost(&thatHost);
    that->GetRealm(&thatRealm);
    that->GetScheme(&thatScheme);
    Int32 thatPort;
    that->GetPort(&thatPort);
    AutoPtr<ICharSequence> hostCS, thatHostCS;
    CString::New(mHost, (ICharSequence**)&hostCS);
    CString::New(thatHost, (ICharSequence**)&thatHostCS);
    if (LangUtils::Equals(IObject::Probe(hostCS), IObject::Probe(thatHostCS)) && mPort == thatPort) {
        AutoPtr<ICharSequence> realmCS, thatRealmCS;
        CString::New(mRealm, (ICharSequence**)&realmCS);
        CString::New(thatRealm, (ICharSequence**)&thatRealmCS);
        if (LangUtils::Equals(IObject::Probe(realmCS), IObject::Probe(thatRealmCS))) {
            AutoPtr<ICharSequence> schemeCS, thatSchemeCS;
            CString::New(mScheme, (ICharSequence**)&schemeCS);
            CString::New(thatScheme, (ICharSequence**)&thatSchemeCS);
            *equals = LangUtils::Equals(IObject::Probe(schemeCS), IObject::Probe(thatSchemeCS));
        }
        else {
            *equals = FALSE;
        }
    }
    else {
        *equals = FALSE;
    }
    return NOERROR;
}

ECode CAuthScope::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    StringBuilder buffer;
    if (!mScheme.IsNull()) {
        buffer.Append(mScheme.ToUpperCase(/*ILocale::ENGLISH*/));
        buffer.AppendChar(' ');
    }
    if (!mRealm.IsNull()) {
        buffer.AppendChar('\'');
        buffer.Append(mRealm);
        buffer.AppendChar('\'');
    }
    else {
        buffer.Append("<any realm>");
    }
    if (!mHost.IsNull()) {
        buffer.AppendChar('@');
        buffer.Append(mHost);
        if (mPort >= 0) {
            buffer.AppendChar(':');
            buffer.Append(mPort);
        }
    }
    *string = buffer.ToString();
    return NOERROR;
}

ECode CAuthScope::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = LangUtils::HASH_SEED;
    AutoPtr<ICharSequence> hostCS, realmCS, schemeCS;
    CString::New(mHost, (ICharSequence**)&hostCS);
    CString::New(mRealm, (ICharSequence**)&realmCS);
    CString::New(mScheme, (ICharSequence**)&schemeCS);
    hash = LangUtils::HashCode(hash, IObject::Probe(hostCS));
    hash = LangUtils::HashCode(hash, mPort);
    hash = LangUtils::HashCode(hash, IObject::Probe(realmCS));
    hash = LangUtils::HashCode(hash, IObject::Probe(schemeCS));
    *hashCode = hash;
    return NOERROR;
}

ECode CAuthScope::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& realm,
    /* [in] */ const String& scheme)
{
    mHost = host.IsNull() ? String(NULL)/*ANY_HOST*/: host.ToLowerCase(/*ILocale::ENGLISH*/);
    mPort = port < 0 ? ANY_PORT: port;
    mRealm = realm.IsNull() ? String(NULL)/*ANY_REALM*/: realm;
    mScheme = scheme.IsNull() ? String(NULL)/*ANY_SCHEME*/: scheme.ToUpperCase(/*ILocale::ENGLISH*/);
    return NOERROR;
}

ECode CAuthScope::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& realm)
{
    return constructor(host, port, realm, String(NULL)/*ANY_SCHEME*/);
}

ECode CAuthScope::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    return constructor(host, port, String(NULL)/*ANY_REALM*/, String(NULL)/*ANY_SCHEME*/);
}

ECode CAuthScope::constructor(
    /* [in] */ IAuthScope* authscope)
{
    if (authscope == NULL) {
        Logger::E("CAuthScope", "Scope may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    authscope->GetHost(&mHost);
    authscope->GetPort(&mPort);
    authscope->GetRealm(&mRealm);
    authscope->GetScheme(&mScheme);
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
