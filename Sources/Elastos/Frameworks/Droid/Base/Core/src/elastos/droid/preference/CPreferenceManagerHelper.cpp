
#include "elastos/droid/preference/CPreferenceManagerHelper.h"
#include "elastos/droid/preference/PreferenceManager.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(CPreferenceManagerHelper, Singleton, IPreferenceManagerHelper)

CAR_SINGLETON_IMPL(CPreferenceManagerHelper)


ECode CPreferenceManagerHelper::GetDefaultSharedPreferences(
    /* [in] */ IContext* context,
    /* [out] */ ISharedPreferences** result)
{
    VALIDATE_NOT_NULL(result)
    return PreferenceManager::GetDefaultSharedPreferences(context, result);
}

ECode CPreferenceManagerHelper::SetDefaultValues(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Boolean readAgain)
{
    return PreferenceManager::SetDefaultValues(context, resId, readAgain);
}

ECode CPreferenceManagerHelper::SetDefaultValues(
    /* [in] */ IContext* context,
    /* [in] */ const String& sharedPreferencesName,
    /* [in] */ Int32 sharedPreferencesMode,
    /* [in] */ Int32 resId,
    /* [in] */ Boolean readAgain)
{
    return PreferenceManager::SetDefaultValues(context, sharedPreferencesName, sharedPreferencesMode, resId, readAgain);
}

} // namespace Preference
} // namespace Droid
} // namespace Elastos