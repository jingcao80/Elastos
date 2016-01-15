
#include "elastos/droid/internal/app/CLocalePickerHelper.h"
#include "elastos/droid/internal/app/LocalePicker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(CLocalePickerHelper, Singleton, ILocalePickerHelper)
CAR_SINGLETON_IMPL(CLocalePickerHelper)

ECode CLocalePickerHelper::GetAllAssetLocales(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isInDeveloperMode,
    /* [out] */ IList** list)
{
    return LocalePicker::GetAllAssetLocales(context, isInDeveloperMode, list);
}

ECode CLocalePickerHelper::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [out] */ IArrayAdapter** adapter)
{
    return LocalePicker::ConstructAdapter(context, adapter);
}

ECode CLocalePickerHelper::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutId,
    /* [in] */ Int32 fieldId,
    /* [out] */ IArrayAdapter** adapter)
{
    return LocalePicker::ConstructAdapter(context, layoutId, fieldId, adapter);
}

ECode CLocalePickerHelper::UpdateLocale(
    /* [in] */ ILocale* locale)
{
    return LocalePicker::UpdateLocale(locale);
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
