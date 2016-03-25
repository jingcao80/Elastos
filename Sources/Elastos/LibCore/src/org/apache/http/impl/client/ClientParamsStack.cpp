
#include "org/apache/http/impl/client/ClientParamsStack.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

ClientParamsStack::ClientParamsStack(
    /* [in] */ IHttpParams* aparams,
    /* [in] */ IHttpParams* cparams,
    /* [in] */ IHttpParams* rparams,
    /* [in] */ IHttpParams* oparams)
{
    Init(aparams, cparams, rparams, oparams);
}

ClientParamsStack::ClientParamsStack(
    /* [in] */ ClientParamsStack* stack)
{
    AutoPtr<IHttpParams> ap = stack->GetApplicationParams();
    AutoPtr<IHttpParams> cp = stack->GetClientParams();
    AutoPtr<IHttpParams> rp = stack->GetRequestParams();
    AutoPtr<IHttpParams> op = stack->GetOverrideParams();
    Init(ap, cp, rp, op);
}

ClientParamsStack::ClientParamsStack(
    /* [in] */ ClientParamsStack* stack,
    /* [in] */ IHttpParams* aparams,
    /* [in] */ IHttpParams* cparams,
    /* [in] */ IHttpParams* rparams,
    /* [in] */ IHttpParams* oparams)
{
    AutoPtr<IHttpParams> ap = (aparams != NULL) ? aparams : stack->GetApplicationParams().Get();
    AutoPtr<IHttpParams> cp = (cparams != NULL) ? cparams : stack->GetClientParams().Get();
    AutoPtr<IHttpParams> rp = (rparams != NULL) ? rparams : stack->GetRequestParams().Get();
    AutoPtr<IHttpParams> op = (oparams != NULL) ? oparams : stack->GetOverrideParams().Get();
    Init(ap, cp, rp, op);
}

void ClientParamsStack::Init(
    /* [in] */ IHttpParams* aparams,
    /* [in] */ IHttpParams* cparams,
    /* [in] */ IHttpParams* rparams,
    /* [in] */ IHttpParams* oparams)
{
    mApplicationParams = aparams;
    mClientParams = cparams;
    mRequestParams = rparams;
    mOverrideParams = oparams;
}

AutoPtr<IHttpParams> ClientParamsStack::GetApplicationParams()
{
    return mApplicationParams;
}

AutoPtr<IHttpParams> ClientParamsStack::GetClientParams()
{
    return mClientParams;
}

AutoPtr<IHttpParams> ClientParamsStack::GetRequestParams()
{
    return mRequestParams;
}

AutoPtr<IHttpParams> ClientParamsStack::GetOverrideParams()
{
    return mOverrideParams;
}

ECode ClientParamsStack::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface** parameter)
{
    VALIDATE_NOT_NULL(parameter)
    *parameter = NULL;

    if (name.IsNull()) {
        Logger::E("ClientParamsStack", "Parameter name must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> result;

    if (mOverrideParams != NULL) {
        mOverrideParams->GetParameter(name, (IInterface**)&result);
    }
    if ((result == NULL) && (mRequestParams != NULL)) {
        mRequestParams->GetParameter(name, (IInterface**)&result);
    }
    if ((result == NULL) && (mClientParams != NULL)) {
        mClientParams->GetParameter(name, (IInterface**)&result);
    }
    if ((result == NULL) && (mApplicationParams != NULL)) {
        mApplicationParams->GetParameter(name, (IInterface**)&result);
    }
    // if (this.log.isDebugEnabled()) {
    //     this.log.debug("'" + name + "': " + result);
    // }
    *parameter = result;
    REFCOUNT_ADD(*parameter)
    return NOERROR;
}

ECode ClientParamsStack::SetParameter(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    *httpParams = NULL;
    Logger::E("ClientParamsStack", "Setting parameters in a stack is not supported.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ClientParamsStack::RemoveParameter(
    /* [in] */ const String& name,
    /* [out] */ Boolean* remove)
{
    VALIDATE_NOT_NULL(remove)
    *remove = FALSE;
    Logger::E("ClientParamsStack", "Removing parameters in a stack is not supported.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ClientParamsStack::Copy(
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    *httpParams = this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org