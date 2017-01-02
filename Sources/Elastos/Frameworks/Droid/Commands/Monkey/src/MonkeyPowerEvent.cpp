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

#include "MonkeyPowerEvent.h"
#include "elastos/droid/os/SystemClock.h"
#include <os/Build.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Os::Build;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::Core::IStringBuffer;
using Elastos::Core::EIID_IStringBuffer;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {
const Int64 MonkeyPowerEvent::USB_DELAY_TIME = 10000;
const String MonkeyPowerEvent::TAG("PowerTester");
const String MonkeyPowerEvent::LOG_FILE = String("/sdcard/autotester.log");
AutoPtr<List<AutoPtr<IContentValues> > > MonkeyPowerEvent::mLogEvents = \
    new List<AutoPtr<IContentValues> >();
const String MonkeyPowerEvent::TEST_SEQ_BEGIN = String("AUTOTEST_SEQUENCE_BEGIN");
const String MonkeyPowerEvent::TEST_STARTED = String("AUTOTEST_TEST_BEGIN");
const String MonkeyPowerEvent::TEST_DELAY_STARTED = String("AUTOTEST_TEST_BEGIN_DELAY");
const String MonkeyPowerEvent::TEST_ENDED = String("AUTOTEST_TEST_SUCCESS");
const String MonkeyPowerEvent::TEST_IDLE_ENDED = String("AUTOTEST_IDLE_SUCCESS");

Int64 MonkeyPowerEvent::mTestStartTime; /*get current time*/

MonkeyPowerEvent::MonkeyPowerEvent(
    /* [in] */ const String& powerLogTag,
    /* [in] */ const String& powerTestResult)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mPowerLogTag(powerLogTag)
    , mTestResult(powerTestResult)
{}

MonkeyPowerEvent::MonkeyPowerEvent(
    /* [in] */ const String& powerLogTag)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mPowerLogTag(powerLogTag)
    , mTestResult("")
{}

MonkeyPowerEvent::MonkeyPowerEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mPowerLogTag("")
    , mTestResult("")
{}

MonkeyPowerEvent::~MonkeyPowerEvent()
{
}

ECode MonkeyPowerEvent::Init(
    /* [in] */ const String& powerLogTag,
    /* [in] */ const String& powerTestResult)
{
    mPowerLogTag = powerLogTag;
    mTestResult = powerTestResult;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

ECode MonkeyPowerEvent::Init(
    /* [in] */ const String& powerLogTag)
{
    mPowerLogTag = powerLogTag;
    mTestResult = "";
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

ECode MonkeyPowerEvent::Init()
{
    mTestResult = "";
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

//Buffer an event to be logged later
ECode MonkeyPowerEvent::BufferLogEvent(
    /* [in] */ const String& tag,
    /* [in] */ const String& value)
{
    Int64 tagTime = SystemClock::GetUptimeMillis();
    //Record the test start time
    String tmp;
    if(0 == tag.Compare(TEST_STARTED)){
        mTestStartTime = tagTime;
    }
    else if(0 == tag.Compare(TEST_IDLE_ENDED)){
        Int64 lagTime = 0;
        StringUtils::ParseInt64(value, 10, &lagTime);
        tagTime = mTestStartTime + lagTime;
        tmp = TEST_ENDED;
    }
    else if(0 == tag.Compare(TEST_DELAY_STARTED)){
        mTestStartTime = tagTime + USB_DELAY_TIME;
        tagTime = mTestStartTime;
        tmp = TEST_STARTED;
    }

    AutoPtr<IContentValues> icv;
    CContentValues::New((IContentValues**)&icv);
    icv->PutInt64(String("date"), (IInteger64*)&tagTime);
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(tag, (ICharSequence**)&cs);
    icv->PutString(String("tag"), cs);

    if(!value.IsNull()){
        cs = NULL;
        CStringWrapper::New(value, (ICharSequence**)&cs);
        icv->PutString(String("value"), cs);
    }
    mLogEvents->PushBack(icv);
    return NOERROR;
}

ECode MonkeyPowerEvent::WriteLogEvents()
{
    AutoPtr< ArrayOf < IContentValues* > > events;

    ContentValuesIterator it = mLogEvents->Begin();
    for (Int32 i = 0; it != mLogEvents->End(); ++it, ++i) {
        events->Set(i, *it);
    }
    mLogEvents->Clear();

    StringBuffer sb;
    Int32 length = events->GetLength();
    for(Int32 i = 0; i < length; ++i){
        Int64 tmp = 0;
        String tmpString;
        (*events)[i]->GetAsInt64(String("date"), &tmp);
        //Todo
        //tmpString = MonkeyUtils.toCalendarTime(tmp);
        sb.Append(tmpString);
        (*events)[i]->GetAsString(String("tag"), &tmpString);
        sb.Append(tmpString);
        Boolean ret = FALSE;
        (*events)[i]->ContainsKey(String("value"), &ret);
        if(ret){
            (*events)[i]->GetAsString(String("value"), &tmpString);
            sb.Append(String(" "));
            sb.Append(tmpString.Replace('\n', '/'));
        }
        sb.Append(String("\n"));
    }

    AutoPtr<IFileWriter> ifw;
    CFileWriter::New(LOG_FILE, TRUE, (IFileWriter**)&ifw);
    String tmp;
    sb.Substring(0, &tmp);
    ifw->WriteString(tmp);
    return NOERROR;
}

Int32 MonkeyPowerEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    if (!mPowerLogTag.IsNull()){
        if(mPowerLogTag.Compare(TEST_SEQ_BEGIN) == 0) {
            BufferLogEvent(mPowerLogTag, Build::FINGERPRINT);
        }
        else if(!mTestResult.IsNull()) {
            BufferLogEvent(mPowerLogTag, mTestResult);
        }
    }
    else {
        WriteLogEvents();
    }

    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

