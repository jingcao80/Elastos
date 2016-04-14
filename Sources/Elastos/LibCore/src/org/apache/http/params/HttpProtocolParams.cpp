
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

ECode HttpProtocolParams::GetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    *charset = String(NULL);
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> param;
    params->GetParameter(ICoreProtocolPNames::HTTP_ELEMENT_CHARSET, (IInterface**)&param);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
    if (cs == NULL) {
        *charset = IHTTP::DEFAULT_PROTOCOL_CHARSET;
        return NOERROR;
    }
    return cs->ToString(charset);
}

ECode HttpProtocolParams::SetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(charset, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> p;
    return params->SetParameter(ICoreProtocolPNames::HTTP_ELEMENT_CHARSET, cs, (IHttpParams**)&p);
}

ECode HttpProtocolParams::GetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    *charset = String(NULL);
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> param;
    params->GetParameter(ICoreProtocolPNames::HTTP_CONTENT_CHARSET, (IInterface**)&param);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
    if (cs == NULL) {
        *charset = IHTTP::DEFAULT_CONTENT_CHARSET;
        return NOERROR;
    }
    return cs->ToString(charset);
}

ECode HttpProtocolParams::SetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(charset, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> p;
    return params->SetParameter(ICoreProtocolPNames::HTTP_CONTENT_CHARSET, cs, (IHttpParams**)&p);
}

ECode HttpProtocolParams::GetVersion(
    /* [in] */ IHttpParams* params,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = NULL;
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> param;
    params->GetParameter(ICoreProtocolPNames::PROTOCOL_VERSION, (IInterface**)&param);
    AutoPtr<IProtocolVersion> pv = IProtocolVersion::Probe(param);
    if (pv == NULL) {
        pv = IProtocolVersion::Probe(CHttpVersion::HTTP_1_1);
    }

    *ver = pv;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode HttpProtocolParams::SetVersion(
    /* [in] */ IHttpParams* params,
    /* [in] */ IProtocolVersion* version)
{
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetParameter(ICoreProtocolPNames::PROTOCOL_VERSION, version, (IHttpParams**)&p);
}

ECode HttpProtocolParams::GetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* agent)
{
    VALIDATE_NOT_NULL(agent)
    *agent = String(NULL);
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> param;
    params->GetParameter(ICoreProtocolPNames::USER_AGENT, (IInterface**)&param);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
    if (cs == NULL) {
        return NOERROR;
    }
    return cs->ToString(agent);
}

ECode HttpProtocolParams::SetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& useragent)
{
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(useragent, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> p;
    return params->SetParameter(ICoreProtocolPNames::PROTOCOL_VERSION, cs, (IHttpParams**)&p);
}

ECode HttpProtocolParams::UseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetBooleanParameter(ICoreProtocolPNames::USE_EXPECT_CONTINUE, FALSE, result);
}

ECode HttpProtocolParams::SetUseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean b)
{
    if (params == NULL) {
        Logger::E("HttpProtocolParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetBooleanParameter(ICoreProtocolPNames::USE_EXPECT_CONTINUE, b, (IHttpParams**)&p);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
