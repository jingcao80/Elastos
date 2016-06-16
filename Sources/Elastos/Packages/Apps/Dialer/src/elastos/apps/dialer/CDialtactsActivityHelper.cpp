#include "CDialtactsActivityHelper.h"
#include "DialtactsActivity.h"

namespace Elastos {
namespace Apps {
namespace Dialer {

CAR_INTERFACE_IMPL(CDialtactsActivityHelper, Singleton, IDialtactsActivityHelper);

CAR_SINGLETON_IMPL(CDialtactsActivityHelper);

ECode CDialtactsActivityHelper::GetCallSettingsIntent(
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    AutoPtr<IIntent> obj = DialtactsActivity::GetCallSettingsIntent();
    *intent = obj;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CDialtactsActivityHelper::GetAddNumberToContactIntent(
    /* [in] */ICharSequence* text,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    AutoPtr<IIntent> obj = DialtactsActivity::GetAddNumberToContactIntent(text);
    *intent = obj;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

} // Dialer
} // Apps
} // Elastos
