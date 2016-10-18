
#include "CAlgNameMapperHelper.h"
#include "AlgNameMapper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_SINGLETON_IMPL(CAlgNameMapperHelper)
CAR_INTERFACE_IMPL(CAlgNameMapperHelper, Singleton, IAlgNameMapperHelper)

ECode CAlgNameMapperHelper::Map2OID(
    /* [in] */ const String& algName,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Map2OID(algName);
    return NOERROR;
}

ECode CAlgNameMapperHelper::Map2AlgName(
    /* [in] */ const String& oid,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Map2AlgName(oid);
    return NOERROR;
}

ECode CAlgNameMapperHelper::GetStandardName(
    /* [in] */ const String& algName,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::GetStandardName(algName);
    return NOERROR;
}

ECode CAlgNameMapperHelper::IsOID(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::IsOID(alias);
    return NOERROR;
}

ECode CAlgNameMapperHelper::Normalize(
    /* [in] */ const String& oid,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = AlgNameMapper::Normalize(oid);
    return NOERROR;
}

}
}
}
}
}
