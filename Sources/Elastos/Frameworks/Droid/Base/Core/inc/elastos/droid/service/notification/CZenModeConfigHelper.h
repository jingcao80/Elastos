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

#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIGHELPER_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIGHELPER_H__

#include "_Elastos_Droid_Service_Notification_CZenModeConfigHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CZenModeConfigHelper)
    , public Singleton
    , public IZenModeConfigHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetALL_DAYS(
        /* [out, callee]*/ ArrayOf<Int32>** days);

    CARAPI GetWEEKNIGHT_DAYS(
        /* [out, callee]*/ ArrayOf<Int32>** days);

    CARAPI GetMINUTE_BUCKETS(
        /* [out, callee]*/ ArrayOf<Int32>** buckets);

    CARAPI SourceToString(
        /* [in] */ Int32 source,
        /* [out] */ String* str);

    CARAPI IsValidSleepMode(
        /* [in] */ const String& sleepMode,
        /* [out] */ Boolean* result);

    CARAPI TryParseDays(
        /* [in] */ const String& sleepMode,
        /* [out, callee] */ ArrayOf<Int32>** results);

    CARAPI ReadXml(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IZenModeConfig** result);

    CARAPI ReadConditionXml(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ ICondition** result);

    CARAPI WriteConditionXml(
        /* [in] */ ICondition* c,
        /* [in] */ IXmlSerializer* out);

    CARAPI IsValidHour(
        /* [in] */ Int32 val,
        /* [out] */ Boolean* result);

    CARAPI IsValidMinute(
        /* [in] */ Int32 val,
        /* [out] */ Boolean* result);

    CARAPI ToTimeCondition(
        /* [in] */ Int32 minutesFromNow,
        /* [out] */ ICondition** result);

    CARAPI ToTimeCondition(
        /* [in] */ Int64 time,
        /* [in] */ Int32 minutes,
        /* [out] */ ICondition** result);

    CARAPI ToCountdownConditionId(
        /* [in] */ Int64 time,
        /* [out] */ IUri** result);

   CARAPI TryParseCountdownConditionId(
        /* [in] */ IUri* conditionId,
        /* [out] */ Int64* result);

    CARAPI IsValidCountdownConditionId(
        /* [in] */ IUri* conditionId,
        /* [out] */ Boolean* result);

    CARAPI ToDowntimeConditionId(
        /* [in] */ IZenModeConfigDowntimeInfo* downtime,
        /* [out] */ IUri** result);

    CARAPI TryParseDowntimeConditionId(
        /* [in] */ IUri* conditionId,
        /* [out] */ IZenModeConfigDowntimeInfo** result);

    CARAPI IsValidDowntimeConditionId(
        /* [in] */ IUri* conditionId,
        /* [out] */ Boolean* result);
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIGHELPER_H__
