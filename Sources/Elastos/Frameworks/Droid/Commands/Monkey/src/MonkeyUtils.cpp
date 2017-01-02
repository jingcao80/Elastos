//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
