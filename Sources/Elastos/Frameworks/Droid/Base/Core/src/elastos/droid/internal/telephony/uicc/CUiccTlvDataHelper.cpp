#include "elastos/droid/internal/telephony/uicc/CUiccTlvDataHelper.h"
#include "elastos/droid/internal/telephony/uicc/UiccTlvData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CAR_INTERFACE_IMPL(CUiccTlvDataHelper, Singleton, IUiccTlvDataHelper)

CAR_SINGLETON_IMPL(CUiccTlvDataHelper)

ECode CUiccTlvDataHelper::IsUiccTlvData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccTlvData::IsUiccTlvData(data);
    return NOERROR;
}

ECode CUiccTlvDataHelper::Parse(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IUiccTlvData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUiccTlvData> utd = UiccTlvData::Parse(data);
    *result = utd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
