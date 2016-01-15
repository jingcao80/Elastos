
#include "org/apache/http/impl/client/AbstractAuthenticationHandler.h"
#include "org/apache/http/protocol/HTTP.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::ILocale;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::EIID_IList;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IFormattedHeader;
using Org::Apache::Http::Auth::IAuthSchemeRegistry;
using Org::Apache::Http::Client::EIID_IAuthenticationHandler;
using Org::Apache::Http::Client::Protocol::IClientContext;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::HTTP;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

static AutoPtr<IList> InitDefaultSchemePriority()
{
    AutoPtr<CArrayList> list;
    CArrayList::NewByFriend((CArrayList**)&list);
    AutoPtr<CString> ntlm, digest, basic;
    CString::NewByFriend(String("ntlm"), (CString**)&ntlm);
    CString::NewByFriend(String("digest"), (CString**)&digest);
    CString::NewByFriend(String("basic"), (CString**)&basic);
    list->Add(ntlm->Probe(EIID_IInterface));
    list->Add(digest->Probe(EIID_IInterface));
    list->Add(basic->Probe(EIID_IInterface));
    return (IList*)list->Probe(EIID_IList);
}
const AutoPtr<IList> AbstractAuthenticationHandler::DEFAULT_SCHEME_PRIORITY = InitDefaultSchemePriority();

CAR_INTERFACE_IMPL(AbstractAuthenticationHandler, Object, IAuthenticationHandler)

ECode AbstractAuthenticationHandler::ParseChallenges(
    /* [in] */ ArrayOf<IHeader*>* headers,
    /* [out] */ IMap** challenges)
{
    VALIDATE_NOT_NULL(challenges)
    *challenges = NULL;

    AutoPtr<IHashMap> hm;
    CHashMap::New(headers->GetLength(), (IHashMap**)&hm);
    AutoPtr<IMap> map = IMap::Probe(hm);
    for (Int32 i = 0; i < headers->GetLength(); ++i) {
        AutoPtr<IHeader> header = (*headers)[i];
        AutoPtr<ICharArrayBuffer> buffer;
        Int32 pos;
        AutoPtr<IFormattedHeader> formattedHeader = IFormattedHeader::Probe(header);
        if (formattedHeader != NULL) {
            formattedHeader->GetBuffer((ICharArrayBuffer**)&buffer);
            formattedHeader->GetValuePos(&pos);
        }
        else {
            String s;
            header->GetValue(&s);
            if (s.IsNull()) {
                Logger::E("AbstractAuthenticationHandler", "Header value is null");
                return E_MALFORMED_CHALLENGE_EXCEPTION;
            }
            CCharArrayBuffer::New(s.GetLength(), (ICharArrayBuffer**)&buffer);
            buffer->Append(s);
            pos = 0;
        }
        Int32 len;
        Char32 c;
        while ((buffer->GetLength(&len), pos < len) && (buffer->CharAt(pos, &c), HTTP::IsWhitespace(c))) {
            pos++;
        }
        Int32 beginIndex = pos;
        while ((buffer->GetLength(&len), pos < len) && (buffer->CharAt(pos, &c), !HTTP::IsWhitespace(c))) {
            pos++;
        }
        Int32 endIndex = pos;
        String s;
        buffer->Substring(beginIndex, endIndex, &s);
        AutoPtr<ICharSequence> cs;
        CString::New(s.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        map->Put(cs, header);
    }
    *challenges = map;
    REFCOUNT_ADD(*challenges)
    return NOERROR;
}

AutoPtr<IList> AbstractAuthenticationHandler::GetAuthPreferences()
{
    return DEFAULT_SCHEME_PRIORITY;
}

ECode AbstractAuthenticationHandler::SelectScheme(
    /* [in] */ IMap* challenges,
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    *scheme = NULL;

    AutoPtr<IInterface> o;
    context->GetAttribute(IClientContext::AUTHSCHEME_REGISTRY, (IInterface**)&o);
    AutoPtr<IAuthSchemeRegistry> registry = IAuthSchemeRegistry::Probe(o);
    if (registry == NULL) {
        Logger::E("AbstractAuthenticationHandler", "AuthScheme registry not set in HTTP context");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    o = NULL;
    context->GetAttribute(IClientContext::AUTH_SCHEME_PREF, (IInterface**)&o);
    AutoPtr<IList> authPrefs = IList::Probe(o);
    if (authPrefs == NULL) {
        authPrefs = GetAuthPreferences();
    }

    // if (this.log.isDebugEnabled()) {
    //     this.log.debug("Authentication schemes in the order of preference: "
    //         + authPrefs);
    // }

    AutoPtr<IAuthScheme> authScheme;
    Int32 size;
    authPrefs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        authPrefs->Get(i, (IInterface**)&value);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(value);
        String id;
        cs->ToString(&id);
        AutoPtr<ICharSequence> idcs;
        CString::New(id.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&idcs);
        value = NULL;
        challenges->Get(idcs, (IInterface**)&value);
        AutoPtr<IHeader> challenge = IHeader::Probe(value);

        if (challenge != NULL) {
            // if (this.log.isDebugEnabled()) {
            //     this.log.debug(id + " authentication scheme selected");
            // }
            // try {
            AutoPtr<IHttpParams> params;
            IHttpMessage::Probe(response)->GetParams((IHttpParams**)&params);
            authScheme = NULL;
            ECode ec = registry->GetAuthScheme(id, params, (IAuthScheme**)&authScheme);
            if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
                Logger::E("AbstractAuthenticationHandler", "Authentication scheme %s not supported", id.string());
                continue;
            }
            break;
            // } catch (IllegalStateException e) {
            //     if (this.log.isWarnEnabled()) {
            //         this.log.warn("Authentication scheme " + id + " not supported");
            //         // Try again
            //     }
            // }
        }
        else {
            // if (this.log.isDebugEnabled()) {
            //     this.log.debug("Challenge for " + id + " authentication scheme not available");
            //     // Try again
            // }
        }
    }
    if (authScheme == NULL) {
        // If none selected, something is wrong
        Logger::E("AbstractAuthenticationHandler", "Unable to respond to any of these challenges: %p", challenges);
        return E_AUTHENTICATION_EXCEPTION;
    }
    *scheme = authScheme;
    REFCOUNT_ADD(*scheme)
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org