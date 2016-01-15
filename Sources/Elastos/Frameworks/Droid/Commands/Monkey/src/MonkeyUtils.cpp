#include "MonkeyUtils.h"

using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Text::CSimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

static AutoPtr<IDate> sDate = MonkeyUtils::InitStatics();
static AutoPtr<ISimpleDateFormat> sDATE_FORMATTER;
Mutex MonkeyUtils::mLockLock;

AutoPtr<IDate> MonkeyUtils::InitStatics()
{
    AutoPtr<IDate> date;
    CDate::New((IDate**)(&date));
    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss.SSS "),
        (ISimpleDateFormat**)(&sDATE_FORMATTER));
    return date;
}

MonkeyUtils::MonkeyUtils()
{}

void MonkeyUtils::ToCalendarTime(
    /* [in] */ Int64 time,
    /* [out] */ String *formatedTime)
{
    AutoLock lock(mLockLock);
    if(formatedTime == NULL)
        return;
    sDate->SetTime(time);
    if (sDATE_FORMATTER == NULL) {
        CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss.SSS "),
            (ISimpleDateFormat**)(&sDATE_FORMATTER));
    }
    assert(sDATE_FORMATTER != NULL);
    sDATE_FORMATTER->FormatDate(sDate, formatedTime);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
