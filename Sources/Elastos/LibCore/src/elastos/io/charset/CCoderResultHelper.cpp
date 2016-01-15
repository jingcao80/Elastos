
#include "CCoderResultHelper.h"
#include "CCoderResult.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCoderResultHelper, Singleton, ICoderResultHelper)

CAR_SINGLETON_IMPL(CCoderResultHelper)

ECode CCoderResultHelper::GetUNDERFLOW(
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::GetUNDERFLOW(result);
}

ECode CCoderResultHelper::GetOVERFLOW(
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::GetOVERFLOW(result);
}

ECode CCoderResultHelper::MalformedForLength(
    /* [in] */ Int32 length,
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::MalformedForLength(length, result);
}

ECode CCoderResultHelper::UnmappableForLength(
    /* [in] */ Int32 length,
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::UnmappableForLength(length, result);
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
