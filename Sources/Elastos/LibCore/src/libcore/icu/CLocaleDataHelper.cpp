
#include "Elastos.CoreLibrary.Utility.h"
#include "CLocaleDataHelper.h"
#include "LocaleData.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CLocaleDataHelper)

CAR_INTERFACE_IMPL(CLocaleDataHelper, Singleton, ILocaleDataHelper)

ECode CLocaleDataHelper::MapInvalidAndNullLocales(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocale** rst)
{
    VALIDATE_NOT_NULL(rst);
    AutoPtr<ILocale> localeObj = LocaleData::MapInvalidAndNullLocales(locale);
    *rst = localeObj;
    REFCOUNT_ADD(*rst);

    return NOERROR;
}

ECode CLocaleDataHelper::Get(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocaleData** localeData)
{
    VALIDATE_NOT_NULL(localeData);
    if (NULL == locale)
        return E_NULL_POINTER_EXCEPTION;

    AutoPtr<ILocaleData> temp = LocaleData::Get(locale);
    *localeData = temp;
    REFCOUNT_ADD(*localeData);

    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
