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

#include "elastos/droid/systemui/statusbar/phone/DozeParameters.h"
#include "../R.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include <elastos/droid/text/TextUtils.h>
#include "elastos/droid/utility/MathUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String DozeParameters::TAG("DozeParameters");
const Boolean DozeParameters::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Int32 DozeParameters::MAX_DURATION = 10 * 1000;

AutoPtr<IPattern> DozeParameters::PulseSchedule::PATTERN = DozeParameters::PulseSchedule::InitStatic();
AutoPtr<DozeParameters::PulseSchedule> DozeParameters::PulseSchedule::Parse(
    /* [in] */ const String& spec)
{
    if (TextUtils::IsEmpty(spec)) return NULL;
    do {
        AutoPtr<PulseSchedule> rt = new PulseSchedule();
        rt->mSpec = spec;
        AutoPtr<ArrayOf<String> > tokens = TextUtils::Split(spec, String(","));
        rt->mSchedule = ArrayOf<Int32>::Alloc(tokens->GetLength());
        for (Int32 i = 0; i < tokens->GetLength(); i++) {
            AutoPtr<IMatcher> m;
            PATTERN->Matcher((*tokens)[i], (IMatcher**)&m);
            Boolean matched = FALSE;
            if (m->Matches(&matched), !matched) {
                // throw new IllegalArgumentException("Bad token: " + tokens[i]);
                break;
            }
            String value;
            IMatchResult::Probe(m)->Group(1, &value);
            (*rt->mSchedule)[i] = StringUtils::ParseInt32(value);
        }
        String str;
        rt->ToString(&str);
        if (DEBUG) Logger::D(TAG, "Parsed spec [%s] as: %s", spec.string(), str.string());
        return rt;
    } while (0);

    Logger::W(TAG, "Error parsing spec: %s", spec.string());
    return NULL;
}

ECode DozeParameters::PulseSchedule::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = Arrays::ToString(mSchedule);
    return NOERROR;
}

Int64 DozeParameters::PulseSchedule::GetNextTime(
    /* [in] */ Int64 now,
    /* [in] */ Int64 notificationTime)
{
    for (Int32 i = 0; i < mSchedule->GetLength(); i++) {
        const Int64 time = notificationTime + (*mSchedule)[i] * 1000;
        if (time > now) return time;
    }
    return 0;
}

AutoPtr<IPattern> DozeParameters::PulseSchedule::InitStatic()
{
    AutoPtr<IPattern> p;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(String("(\\d+?)s"), 0, (IPattern**)&p);
    return p;
}


AutoPtr<DozeParameters::PulseSchedule> DozeParameters::sPulseSchedule;
DozeParameters::DozeParameters(
    /* [in] */ IContext* context)
{
    mContext = context;
}

ECode DozeParameters::Dump(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("  DozeParameters:"));
    pw->Print(String("    getDisplayStateSupported(): "));
    pw->Println(GetDisplayStateSupported());
    pw->Print(String("    getPulseDuration(): "));
    pw->Println(GetPulseDuration());
    pw->Print(String("    getPulseInDuration(): "));
    pw->Println(GetPulseInDuration());
    pw->Print(String("    getPulseInVisibleDuration(): "));
    pw->Println(GetPulseVisibleDuration());
    pw->Print(String("    getPulseOutDuration(): "));
    pw->Println(GetPulseOutDuration());
    pw->Print(String("    getPulseOnSigMotion(): "));
    pw->Println(GetPulseOnSigMotion());
    pw->Print(String("    getVibrateOnSigMotion(): "));
    pw->Println(GetVibrateOnSigMotion());
    pw->Print(String("    getPulseOnPickup(): "));
    pw->Println(GetPulseOnPickup());
    pw->Print(String("    getVibrateOnPickup(): "));
    pw->Println(GetVibrateOnPickup());
    pw->Print(String("    getPulseOnNotifications(): "));
    pw->Println(GetPulseOnNotifications());
    pw->Print(String("    getPulseSchedule(): "));
    pw->Println(GetPulseSchedule()->Probe(EIID_IInterface));
    pw->Print(String("    getPulseScheduleResets(): "));
    pw->Println(GetPulseScheduleResets());
    pw->Print(String("    getPickupVibrationThreshold(): "));
    pw->Println(GetPickupVibrationThreshold());
    return NOERROR;
}

Boolean DozeParameters::GetDisplayStateSupported()
{
    return GetBoolean(String("doze.display.supported"), R::bool_::doze_display_state_supported);
}

Int32 DozeParameters::GetPulseDuration()
{
    return GetPulseInDuration() + GetPulseVisibleDuration() + GetPulseOutDuration();
}

Int32 DozeParameters::GetPulseInDuration()
{
    return GetInt32(String("doze.pulse.duration.in"), R::integer::doze_pulse_duration_in);
}

Int32 DozeParameters::GetPulseVisibleDuration()
{
    return GetInt32(String("doze.pulse.duration.visible"), R::integer::doze_pulse_duration_visible);
}

Int32 DozeParameters::GetPulseOutDuration()
{
    return GetInt32(String("doze.pulse.duration.out"), R::integer::doze_pulse_duration_out);
}

Boolean DozeParameters::GetPulseOnSigMotion()
{
    return GetBoolean(String("doze.pulse.sigmotion"), R::bool_::doze_pulse_on_significant_motion);
}

Boolean DozeParameters::GetVibrateOnSigMotion()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean value = FALSE;
    sp->GetBoolean(String("doze.vibrate.sigmotion"), FALSE, &value);
    return value;
}

Boolean DozeParameters::GetPulseOnPickup()
{
    return GetBoolean(String("doze.pulse.pickup"), R::bool_::doze_pulse_on_pick_up);
}

Boolean DozeParameters::GetVibrateOnPickup()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean value = FALSE;
    sp->GetBoolean(String("doze.vibrate.pickup"), FALSE, &value);
    return value;
}

Boolean DozeParameters::GetPulseOnNotifications()
{
    return GetBoolean(String("doze.pulse.notifications"), R::bool_::doze_pulse_on_notifications);
}

AutoPtr<DozeParameters::PulseSchedule> DozeParameters::GetPulseSchedule()
{
    String spec = GetString(String("doze.pulse.schedule"), R::string::doze_pulse_schedule);
    if (sPulseSchedule == NULL || !sPulseSchedule->mSpec.Equals(spec)) {
        sPulseSchedule = PulseSchedule::Parse(spec);
    }
    return sPulseSchedule;
}

Int32 DozeParameters::GetPulseScheduleResets()
{
    return GetInt32(String("doze.pulse.schedule.resets"), R::integer::doze_pulse_schedule_resets);
}

Int32 DozeParameters::GetPickupVibrationThreshold()
{
    return GetInt32(String("doze.pickup.vibration.threshold"), R::integer::doze_pickup_vibration_threshold);
}

Boolean DozeParameters::GetBoolean(
    /* [in] */ const String& propName,
    /* [in] */ Int32 resId)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean value = FALSE, tmp = FALSE;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(resId, &tmp);
    sp->GetBoolean(propName, tmp, &value);
    return value;
}

Int32 DozeParameters::GetInt32(
    /* [in] */ const String& propName,
    /* [in] */ Int32 resId)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Int32 value = 0, tmp = 0;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(resId, &tmp);
    sp->GetInt32(propName, tmp, &value);
    return MathUtils::Constrain(value, 0, MAX_DURATION);
}

String DozeParameters::GetString(
    /* [in] */ const String& propName,
    /* [in] */ Int32 resId)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String s1, value;
    mContext->GetString(resId, &s1);
    sp->Get(propName, s1, &value);
    return value;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
