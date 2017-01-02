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

#include "elastos/droid/service/notification/CZenModeConfigHelper.h"
#include "elastos/droid/service/notification/ZenModeConfig.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CAR_INTERFACE_IMPL(CZenModeConfigHelper, Singleton, IZenModeConfigHelper)

CAR_SINGLETON_IMPL(CZenModeConfigHelper)

ECode CZenModeConfigHelper::GetALL_DAYS(
    /* [out, callee]*/ ArrayOf<Int32>** days)
{
    VALIDATE_NOT_NULL(days)
    *days = ZenModeConfig::ALL_DAYS;
    REFCOUNT_ADD(*days)
    return NOERROR;
}

ECode CZenModeConfigHelper::GetWEEKNIGHT_DAYS(
    /* [out, callee]*/ ArrayOf<Int32>** days)
{
    VALIDATE_NOT_NULL(days)
    *days = ZenModeConfig::WEEKNIGHT_DAYS;
    REFCOUNT_ADD(*days)
    return NOERROR;
}

ECode CZenModeConfigHelper::GetMINUTE_BUCKETS(
    /* [out, callee]*/ ArrayOf<Int32>** buckets)
{
    VALIDATE_NOT_NULL(buckets)
    *buckets = ZenModeConfig::MINUTE_BUCKETS;
    REFCOUNT_ADD(*buckets)
    return NOERROR;
}

ECode CZenModeConfigHelper::SourceToString(
    /* [in] */ Int32 source,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ZenModeConfig::SourceToString(source);
    return NOERROR;
}

ECode CZenModeConfigHelper::IsValidSleepMode(
    /* [in] */ const String& sleepMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ZenModeConfig::IsValidSleepMode(sleepMode);
    return NOERROR;
}

ECode CZenModeConfigHelper::TryParseDays(
    /* [in] */ const String& sleepMode,
    /* [out, callee] */ ArrayOf<Int32>** results)
{
    return ZenModeConfig::TryParseDays(sleepMode, results);
}

ECode CZenModeConfigHelper::ReadXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IZenModeConfig** result)
{
    return ZenModeConfig::ReadXml(parser, result);
}

ECode CZenModeConfigHelper::ReadConditionXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ ICondition** result)
{
    return ZenModeConfig::ReadConditionXml(parser, result);
}

ECode CZenModeConfigHelper::WriteConditionXml(
    /* [in] */ ICondition* c,
    /* [in] */ IXmlSerializer* out)
{
    return ZenModeConfig::WriteConditionXml(c, out);
}

ECode CZenModeConfigHelper::IsValidHour(
    /* [in] */ Int32 val,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ZenModeConfig::IsValidHour(val);
    return NOERROR;
}

ECode CZenModeConfigHelper::IsValidMinute(
    /* [in] */ Int32 val,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ZenModeConfig::IsValidMinute(val);
    return NOERROR;
}

ECode CZenModeConfigHelper::ToTimeCondition(
    /* [in] */ Int32 minutesFromNow,
    /* [out] */ ICondition** result)
{
    return ZenModeConfig::ToTimeCondition(minutesFromNow, result);
}

ECode CZenModeConfigHelper::ToTimeCondition(
    /* [in] */ Int64 time,
    /* [in] */ Int32 minutes,
    /* [out] */ ICondition** result)
{
    return ZenModeConfig::ToTimeCondition(time, minutes, result);
}

ECode CZenModeConfigHelper::ToCountdownConditionId(
    /* [in] */ Int64 time,
    /* [out] */ IUri** result)
{
    return ZenModeConfig::ToCountdownConditionId(time, result);
}

ECode CZenModeConfigHelper::TryParseCountdownConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ Int64* result)
{
    return ZenModeConfig::TryParseCountdownConditionId(conditionId, result);
}

ECode CZenModeConfigHelper::IsValidCountdownConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ Boolean* result)
{
    return ZenModeConfig::IsValidCountdownConditionId(conditionId, result);
}

ECode CZenModeConfigHelper::ToDowntimeConditionId(
    /* [in] */ IZenModeConfigDowntimeInfo* downtime,
    /* [out] */ IUri** result)
{
    return ZenModeConfig::ToDowntimeConditionId(downtime, result);
}

ECode CZenModeConfigHelper::TryParseDowntimeConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ IZenModeConfigDowntimeInfo** result)
{
    return ZenModeConfig::TryParseDowntimeConditionId(conditionId, result);
}

ECode CZenModeConfigHelper::IsValidDowntimeConditionId(
    /* [in] */ IUri* conditionId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ZenModeConfig::IsValidDowntimeConditionId(conditionId);
    return NOERROR;
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

