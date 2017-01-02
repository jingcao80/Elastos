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

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/protocol/HttpDateGenerator.h"
#include "elastos/core/CSystem.h"
#include "elastos/core/AutoLock.h"
#include "elastos/text/CSimpleDateFormat.h"
#include "elastos/utility/CDate.h"
#include "elastos/utility/CTimeZoneHelper.h"
#include "elastos/utility/logging/Logger.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

static AutoPtr<ITimeZone> InitGMT()
{
    AutoPtr<CTimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingletonByFriend((CTimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetTimeZone(String("GMT"), (ITimeZone**)&tz);
    return tz;
}
const AutoPtr<ITimeZone> HttpDateGenerator::GMT = InitGMT();

CAR_INTERFACE_IMPL(HttpDateGenerator, Object, IHttpDateGenerator)

HttpDateGenerator::HttpDateGenerator()
    : mDateAsLong(0)
    , mDateAsText(String(NULL))
{}

ECode HttpDateGenerator::Init()
{
    AutoPtr<ISimpleDateFormat> sdf;
    AutoPtr<ILocale> l/*ILocale::US*/;
    CSimpleDateFormat::New(PATTERN_RFC1123, l, (ISimpleDateFormat**)&sdf);
    mDateformat = IDateFormat::Probe(sdf);
    mDateformat->SetTimeZone(GMT);
    return NOERROR;
}

ECode HttpDateGenerator::GetCurrentDate(
    /* [out] */ String* date)
{
    VALIDATE_NOT_NULL(date)
    {    AutoLock syncLock(this);
        Int64 now;
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&now);
        if (now - mDateAsLong > 1000) {
            // Generate new date string
            AutoPtr<IDate> d;
            CDate::New(now, (IDate**)&d);
            mDateformat->Format(d, &mDateAsText);
            mDateAsLong = now;
        }
        *date = mDateAsText;
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
