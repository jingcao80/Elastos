
#include "org/apache/http/params/AbstractHttpParams.h"
#include "elastos/core/CInteger64.h"
#include "elastos/core/CInteger32.h"
#include "elastos/core/CDouble.h"
#include "elastos/core/CBoolean.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL(AbstractHttpParams, Object, IHttpParams)

ECode AbstractHttpParams::GetInt64Parameter(
    /* [in] */ const String& name,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* int64Parameter)
{
    VALIDATE_NOT_NULL(int64Parameter)
    AutoPtr<IInterface> param;
    GetParameter(name, (IInterface**)&param);
    if (param == NULL) {
        *int64Parameter = defaultValue;
        return NOERROR;
    }
    return IInteger64::Probe(param)->GetValue(int64Parameter);
}

ECode AbstractHttpParams::SetInt64Parameter(
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IInteger64> i;
    CInteger64::New(value, (IInteger64**)&i);
    AutoPtr<IHttpParams> p;
    SetParameter(name, i, (IHttpParams**)&p);
    *httpParams = this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode AbstractHttpParams::GetInt32Parameter(
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* int32Parameter)
{
    VALIDATE_NOT_NULL(int32Parameter)
    AutoPtr<IInterface> param;
    GetParameter(name, (IInterface**)&param);
    if (param == NULL) {
        *int32Parameter = defaultValue;
        return NOERROR;
    }
    return IInteger32::Probe(param)->GetValue(int32Parameter);
}

ECode AbstractHttpParams::SetInt32Parameter(
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IInteger32> i;
    CInteger32::New(value, (IInteger32**)&i);
    AutoPtr<IHttpParams> p;
    SetParameter(name, i, (IHttpParams**)&p);
    *httpParams = this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode AbstractHttpParams::GetDoubleParameter(
    /* [in] */ const String& name,
    /* [in] */ Double defaultValue,
    /* [out] */ Double* doubleParameter)
{
    VALIDATE_NOT_NULL(doubleParameter)
    AutoPtr<IInterface> param;
    GetParameter(name, (IInterface**)&param);
    if (param == NULL) {
        *doubleParameter = defaultValue;
        return NOERROR;
    }
    return IDouble::Probe(param)->GetValue(doubleParameter);
}

ECode AbstractHttpParams::SetDoubleParameter(
    /* [in] */ const String& name,
    /* [in] */ Double value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IDouble> i;
    CDouble::New(value, (IDouble**)&i);
    AutoPtr<IHttpParams> p;
    SetParameter(name, i, (IHttpParams**)&p);
    *httpParams = this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode AbstractHttpParams::GetBooleanParameter(
    /* [in] */ const String& name,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* booleanParameter)
{
    VALIDATE_NOT_NULL(booleanParameter)
    AutoPtr<IInterface> param;
    GetParameter(name, (IInterface**)&param);
    if (param == NULL) {
        *booleanParameter = defaultValue;
        return NOERROR;
    }
    return IBoolean::Probe(param)->GetValue(booleanParameter);
}

ECode AbstractHttpParams::SetBooleanParameter(
    /* [in] */ const String& name,
    /* [in] */ Boolean value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IBoolean> i;
    CBoolean::New(value, (IBoolean**)&i);
    AutoPtr<IHttpParams> p;
    SetParameter(name, i, (IHttpParams**)&p);
    *httpParams = this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode AbstractHttpParams::IsParameterTrue(
    /* [in] */ const String& name,
    /* [out] */ Boolean* isParameterTrue)
{
    VALIDATE_NOT_NULL(isParameterTrue)
    return GetBooleanParameter(name, FALSE, isParameterTrue);
}

ECode AbstractHttpParams::IsParameterFalse(
    /* [in] */ const String& name,
    /* [out] */ Boolean* isParameterFalse)
{
    VALIDATE_NOT_NULL(isParameterFalse)
    Boolean result;
    GetBooleanParameter(name, FALSE, &result);
    *isParameterFalse = !result;
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
