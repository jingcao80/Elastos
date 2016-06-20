#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CAR_INTERFACE_IMPL(CUiccControllerHelper, Singleton, IUiccControllerHelper)
CAR_SINGLETON_IMPL(CUiccControllerHelper)

ECode CUiccControllerHelper::Make(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ IUiccController** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUiccController> uc = UiccController::Make(c, ci);
    *result = uc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUiccControllerHelper::GetInstance(
    /* [out] */ IUiccController** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUiccController> uc = UiccController::GetInstance();
    *result = uc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUiccControllerHelper::GetFamilyFromRadioTechnology(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccController::GetFamilyFromRadioTechnology(radioTechnology);
    return NOERROR;
}

ECode CUiccControllerHelper::ParseOemSimRefresh(
    /* [in] */ IByteBuffer* payload,
    /* [out] */ IIccRefreshResponse** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIccRefreshResponse> rr = UiccController::ParseOemSimRefresh(payload);
    *result = rr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
