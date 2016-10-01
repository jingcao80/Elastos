
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/contacts/common/util/CommonDateUtils.h"

using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

static AutoPtr<ISimpleDateFormat> InitFormat(
    /* [in] */ const String& tem)
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> us;
    helper->GetUS((ILocale**)&us);
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(tem, us, (ISimpleDateFormat**)&format);
    return format;
}

const AutoPtr<ISimpleDateFormat> CommonDateUtils::NO_YEAR_DATE_FORMAT =
        InitFormat(String("--MM-dd"));
const AutoPtr<ISimpleDateFormat> CommonDateUtils::FULL_DATE_FORMAT =
        InitFormat(String("yyyy-MM-dd"));
const AutoPtr<ISimpleDateFormat> CommonDateUtils::DATE_AND_TIME_FORMAT =
        InitFormat(String("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"));
const AutoPtr<ISimpleDateFormat> CommonDateUtils::NO_YEAR_DATE_AND_TIME_FORMAT =
        InitFormat(String("--MM-dd'T'HH:mm:ss.SSS'Z'"));

const Int32 CommonDateUtils::DEFAULT_HOUR;

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
