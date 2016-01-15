
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/conn/routing/CBasicRouteDirector.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpHost;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {

CAR_INTERFACE_IMPL_2(CBasicRouteDirector, Object, IBasicRouteDirector, IHttpRouteDirector)

CAR_OBJECT_IMPL(CBasicRouteDirector)

ECode CBasicRouteDirector::NextStep(
    /* [in] */ IRouteInfo* plan,
    /* [in] */ IRouteInfo* fact,
    /* [out] */ Int32* step)
{
    VALIDATE_NOT_NULL(step)
    *step = UNREACHABLE;
    if (plan == NULL) {
        Logger::E("CBasicRouteDirector", "Planned route may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 count;
    if ((fact == NULL) || (fact->GetHopCount(&count), count < 1)) {
        return FirstStep(plan, step);
    }
    else if (plan->GetHopCount(&count), count > 1) {
        return ProxiedStep(plan, fact, step);
    }
    else {
        return DirectStep(plan, fact, step);
    }
}

ECode CBasicRouteDirector::FirstStep(
    /* [in] */ IRouteInfo* plan,
    /* [out] */ Int32* step)
{
    VALIDATE_NOT_NULL(step)
    Int32 count;
    if (plan->GetHopCount(&count), count > 1) {
        *step = CONNECT_PROXY;
    }
    else {
        *step = CONNECT_TARGET;
    }
    return NOERROR;
}

ECode CBasicRouteDirector::DirectStep(
    /* [in] */ IRouteInfo* plan,
    /* [in] */ IRouteInfo* fact,
    /* [out] */ Int32* step)
{
    VALIDATE_NOT_NULL(step)
    Int32 count;
    if (fact->GetHopCount(&count), count > 1) {
        *step = UNREACHABLE;
        return NOERROR;
    }
    AutoPtr<IHttpHost> planHost, factHost;
    plan->GetTargetHost((IHttpHost**)&planHost);
    plan->GetTargetHost((IHttpHost**)&factHost);
    Boolean isEquals;
    if (IObject::Probe(planHost)->Equals(factHost, &isEquals), !isEquals) {
        *step = UNREACHABLE;
        return NOERROR;
    }
    // If the security is too low, we could now suggest to layer
    // a secure protocol on the direct connection. Layering on direct
    // connections has not been supported in HttpClient 3.x, we don't
    // consider it here until there is a real-life use case for it.

    // Should we tolerate if security is better than planned?
    // (plan.isSecure() && !fact.isSecure())
    Boolean isSecurePlan, isSecureFact;
    plan->IsSecure(&isSecurePlan);
    fact->IsSecure(&isSecureFact);
    if (isSecurePlan != isSecureFact) {
        *step = UNREACHABLE;
        return NOERROR;
    }

    // Local address has to match only if the plan specifies one.
    AutoPtr<IInetAddress> planAddr;
    plan->GetLocalAddress((IInetAddress**)&planAddr);
    if (planAddr != NULL) {
        AutoPtr<IInetAddress> factAddr;
        fact->GetLocalAddress((IInetAddress**)&factAddr);
        if (IObject::Probe(planAddr)->Equals(factAddr, &isEquals), !isEquals) {
            *step = UNREACHABLE;
            return NOERROR;
        }
    }

    *step = COMPLETE;
    return NOERROR;
}

ECode CBasicRouteDirector::ProxiedStep(
    /* [in] */ IRouteInfo* plan,
    /* [in] */ IRouteInfo* fact,
    /* [out] */ Int32* step)
{
    VALIDATE_NOT_NULL(step)
    Int32 count;
    if (fact->GetHopCount(&count), count <= 1) {
        *step = UNREACHABLE;
        return NOERROR;
    }
    AutoPtr<IHttpHost> planHost, factHost;
    plan->GetTargetHost((IHttpHost**)&planHost);
    fact->GetTargetHost((IHttpHost**)&factHost);
    Boolean isEquals;
    if (IObject::Probe(planHost)->Equals(factHost, &isEquals), !isEquals) {
        *step = UNREACHABLE;
        return NOERROR;
    }
    Int32 phc, fhc;
    plan->GetHopCount(&phc);
    fact->GetHopCount(&fhc);
    if (phc < fhc) {
        *step = UNREACHABLE;
        return NOERROR;
    }

    for (Int32 i = 0; i < fhc - 1; i++) {
        AutoPtr<IHttpHost> pht, fht;
        plan->GetHopTarget(i, (IHttpHost**)&pht);
        fact->GetHopTarget(i, (IHttpHost**)&fht);
        if (IObject::Probe(pht)->Equals(fht, &isEquals), !isEquals) {
            *step = UNREACHABLE;
            return NOERROR;
        }
    }
    // now we know that the target matches and proxies so far are the same
    if (phc > fhc) {
        *step = TUNNEL_PROXY;// need to extend the proxy chain
        return NOERROR;
    }

    // proxy chain and target are the same, check tunnelling and layering
    Boolean fIsTunnelled, pIsTunnelled, fIslayered, pIsLayered;
    if (((fact->IsTunnelled(&fIsTunnelled), fIsTunnelled) && (plan->IsTunnelled(&pIsTunnelled), !pIsTunnelled)) ||
        ((fact->IsLayered(&fIslayered), fIslayered) && (plan->IsLayered(&pIsLayered), !pIsLayered))) {
        *step = UNREACHABLE;
        return NOERROR;
    }

    if ((plan->IsTunnelled(&pIsTunnelled), pIsTunnelled) && (fact->IsTunnelled(&fIsTunnelled), !fIsTunnelled)) {
        *step = TUNNEL_TARGET;
        return NOERROR;
    }

    if ((plan->IsTunnelled(&pIsTunnelled), pIsTunnelled) && (fact->IsLayered(&fIslayered), !fIslayered)) {
        *step = LAYER_PROTOCOL;
        return NOERROR;
    }

    // tunnel and layering are the same, remains to check the security
    // Should we tolerate if security is better than planned?
    // (plan.isSecure() && !fact.isSecure())
    Boolean pIsSecure, fIsSecure;
    plan->IsSecure(&pIsSecure);
    fact->IsSecure(&fIsSecure);
    if (pIsSecure != fIsSecure) {
        *step = LAYER_PROTOCOL;
        return NOERROR;
    }

    *step = COMPLETE;
    return NOERROR;
}

} // namespace Routing
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
