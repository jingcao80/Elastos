
#include "CPKIXBuilderParameters.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXBuilderParameters)
CPKIXBuilderParameters::CPKIXBuilderParameters()
    : mMaxPathLength(5)
{}

ECode CPKIXBuilderParameters::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("[\n");
    String base;
    PKIXParameters::ToString(&base);
    sb.Append(base);
    sb.Append(" Max Path Length: ");
    sb.Append(mMaxPathLength);
    sb.Append("\n]");
    return sb.ToString(str);

}

ECode CPKIXBuilderParameters::GetMaxPathLength(
    /* [out] */ Int32 *length)
{
    VALIDATE_NOT_NULL(length)
    *length = mMaxPathLength;
    return NOERROR;
}

ECode CPKIXBuilderParameters::SetMaxPathLength(
    /* [in] */ Int32 maxPathLength)
{
    if (mMaxPathLength < -1) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    mMaxPathLength = maxPathLength;
    return NOERROR;
}

ECode CPKIXBuilderParameters::constructor()
{
    return NOERROR;
}

ECode CPKIXBuilderParameters::constructor(
    /* [in] */ ISet *trustAnchors,
    /* [in] */ ICertSelector *targetConstraints)
{
    PKIXParameters::constructor(trustAnchors);
    return PKIXParameters::SetTargetCertConstraints(targetConstraints);
}

ECode CPKIXBuilderParameters::constructor(
    /* [in] */ IKeyStore *keyStore,
    /* [in] */ ICertSelector *targetConstraints)
{
    PKIXParameters::constructor(keyStore);
    return PKIXParameters::SetTargetCertConstraints(targetConstraints);
}

}
}
}
