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

#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Utility::CPair;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String HdmiLogger::TAG("HDMI");
const Int64 HdmiLogger::ERROR_LOG_DURATTION_MILLIS = 20 * 1000;
const Boolean HdmiLogger::DEBUG = InitDEBUG();

pthread_key_t HdmiLogger::sKey;
pthread_once_t HdmiLogger::sKeyOnce = PTHREAD_ONCE_INIT;

static void HdmiLoggerDestructor(void* st)
{
    HdmiLogger* obj = static_cast<HdmiLogger*>(st);
    if (obj) {
        delete obj;
    }
}

static void MakeKey()
{
    ASSERT_TRUE(pthread_key_create(&HdmiLogger::sKey, HdmiLoggerDestructor) == 0);
}

Boolean HdmiLogger::InitDEBUG()
{
    pthread_once(&sKeyOnce, MakeKey);
    pthread_setspecific(sKey, NULL);

    return FALSE;
}

HdmiLogger::HdmiLogger()
{
    CHashMap::New((IHashMap**)&mWarningTimingCache);
    CHashMap::New((IHashMap**)&mErrorTimingCache);
}

ECode HdmiLogger::Warning(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    GetLogger()->WarningInternal(ToLogString(fmt, ap));
    va_end(ap);
    return NOERROR;
}

ECode HdmiLogger::WarningInternal(
    /* [in] */ const String& logMessage)
{
    String log = UpdateLog(mWarningTimingCache, logMessage);
    if (!log.IsEmpty()) {
        Slogger::W(TAG, log);
    }
    return NOERROR;
}

ECode HdmiLogger::Error(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    GetLogger()->ErrorInternal(ToLogString(fmt, ap));
    va_end(ap);
    return NOERROR;
}

ECode HdmiLogger::ErrorInternal(
    /* [in] */ const String& logMessage)
{
    String log = UpdateLog(mErrorTimingCache, logMessage);
    if (!log.IsEmpty()) {
        Slogger::E(TAG, log);
    }
    return NOERROR;
}

ECode HdmiLogger::Debug(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    GetLogger()->DebugInternal(ToLogString(fmt, ap));
    va_end(ap);
    return NOERROR;
}

ECode HdmiLogger::DebugInternal(
    /* [in] */ const String& logMessage)
{
    if (!DEBUG) {
        return NOERROR;
    }
    Slogger::D(TAG, logMessage);
    return NOERROR;
}

String HdmiLogger::ToLogString(
    /* [in] */ const char* fmt, ...)
{
    String rev;
    va_list ap;
    va_start(ap, fmt);
    rev.AppendFormat(fmt, ap);
    va_end(ap);
    return rev;
}

AutoPtr<HdmiLogger> HdmiLogger::GetLogger()
{
    AutoPtr<HdmiLogger> logger = (HdmiLogger*) pthread_getspecific(sKey);
    if (logger == NULL) {
        logger = new HdmiLogger();
        pthread_setspecific(sKey, logger.Get());
    }
    return logger;
}

String HdmiLogger::UpdateLog(
    /* [in] */ IHashMap* cache,
    /* [in] */ const String& logMessage)
{
    Int64 curTime = SystemClock::GetUptimeMillis();
    AutoPtr<IInterface> obj;
    cache->Get(StringUtils::ParseCharSequence(logMessage), (IInterface**)&obj);
    AutoPtr<IPair> timing = IPair::Probe(obj);
    if (ShouldLogNow(timing, curTime)) {
        String log = BuildMessage(logMessage, timing);
        AutoPtr<IInteger64> i64CurTime;
        CInteger64::New(curTime, (IInteger64**)&i64CurTime);
        AutoPtr<IInteger32> i32;
        CInteger32::New(1, (IInteger32**)&i32);
        AutoPtr<IPair> pair;
        CPair::New(i64CurTime, i32, (IPair**)&pair);
        cache->Put(StringUtils::ParseCharSequence(logMessage), pair);
        return log;
    } else {
        IncreaseLogCount(cache, logMessage);
    }
    return String("");
}

String HdmiLogger::BuildMessage(
    /* [in] */ const String& message,
    /* [in] */ IPair* timing)
{
    StringBuilder sb;
    sb.Append("[");
    AutoPtr<IInterface> second;
    timing->GetSecond((IInterface**)&second);
    Int32 iSecond;
    IInteger32::Probe(second)->GetValue(&iSecond);
    sb.Append(timing == NULL ? 1 : iSecond);
    sb.Append("]:");
    sb.Append(message);
    return sb.ToString();
}

ECode HdmiLogger::IncreaseLogCount(
    /* [in] */ IHashMap* cache,
    /* [in] */ const String& message)
{
    AutoPtr<IInterface> obj;
    cache->Get(StringUtils::ParseCharSequence(message), (IInterface**)&obj);
    AutoPtr<IPair> timing = IPair::Probe(obj);
    if (timing != NULL) {
        AutoPtr<IPair> pair;
        AutoPtr<IInterface> first;
        timing->GetFirst((IInterface**)&first);
        AutoPtr<IInterface> second;
        timing->GetSecond((IInterface**)&second);
        Int32 i32;
        IInteger32::Probe(second)->GetValue(&i32);
        i32 += 1;
        AutoPtr<IInteger32> i32Second;
        CInteger32::New(i32, (IInteger32**)&i32Second);
        CPair::New(first, i32Second, (IPair**)&pair);
        cache->Put(StringUtils::ParseCharSequence(message), pair);
    }
    return NOERROR;
}

Boolean HdmiLogger::ShouldLogNow(
    /* [in] */ IPair* timing,
    /* [in] */ Int64 curTime)
{
    AutoPtr<IInterface> iFirst;
    timing->GetFirst((IInterface**)&iFirst);
    Int64 first;
    IInteger64::Probe(iFirst)->GetValue(&first);
    return timing == NULL || curTime - first > ERROR_LOG_DURATTION_MILLIS;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
