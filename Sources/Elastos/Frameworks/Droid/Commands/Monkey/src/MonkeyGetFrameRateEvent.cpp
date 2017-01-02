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

#include "MonkeyGetFrameRateEvent.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

Int64 MonkeyGetFrameRateEvent::sStartTime;
Int64 MonkeyGetFrameRateEvent::sEndTime;
Float MonkeyGetFrameRateEvent::sDuration;
String MonkeyGetFrameRateEvent::sTestCaseName = String("");
Int32 MonkeyGetFrameRateEvent::sStartFrameNo = 0;
Int32 MonkeyGetFrameRateEvent::sEndFrameNo = 0;
const String MonkeyGetFrameRateEvent:: TAG = String("MonkeyGetFrameRateEvent");
volatile const String MonkeyGetFrameRateEvent::LOG_FILE;
AutoPtr<IPattern> MonkeyGetFrameRateEvent::NO_OF_FRAMES_PATTERN;

MonkeyGetFrameRateEvent::MonkeyGetFrameRateEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , GET_FRAMERATE_CMD("service call SurfaceFlinger 1013")
{
    Initializer();
}

MonkeyGetFrameRateEvent::MonkeyGetFrameRateEvent(
    /* [in] */ const String& status,
    /* [in] */ const String& testCaseName)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mStatus(status)
{
    sTestCaseName = testCaseName;
}

MonkeyGetFrameRateEvent::MonkeyGetFrameRateEvent(
    /* [in] */ const String& status)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mStatus(status)
{
}

ECode MonkeyGetFrameRateEvent::Init(
    /* [in] */ const String& status,
    /* [in] */ const String& testCaseName)
{
    mStatus = status;
    sTestCaseName = testCaseName;
    Initializer();
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

ECode MonkeyGetFrameRateEvent::Init(
    /* [in] */ const String& status)
{
    mStatus = status;
    Initializer();
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

Int32 MonkeyGetFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    FILE* pFile = popen(GET_FRAMERATE_CMD, "r");
    if (pFile) {
        char oneLine[1000];
        fgets(oneLine, sizeof(oneLine), pFile);
        String output(oneLine);

        if (!output.IsNull()) {
            if (mStatus.Equals("start")) {
                sStartFrameNo = StringUtils::ParseInt32(GetNumberOfFrames(output), 16);
                sStartTime = SystemClock::GetUptimeMillis();
            }
            else if(mStatus.Equals("end")) {
                sEndFrameNo = StringUtils::ParseInt32(GetNumberOfFrames(output), 16);
                sEndTime = SystemClock::GetUptimeMillis();
                Int64 diff = sEndTime - sStartTime;
                sDuration = (Float) (diff / 1000.0f);
                WriteAverageFrameRate();
            }
        }
        pclose(pFile);
    }
    return IMonkeyEvent::INJECT_SUCCESS;
}

Float MonkeyGetFrameRateEvent::GetAverageFrameRate(
    /* [in] */ Int32 totalNumberOfFrame,
    /* [in] */ Float duration)
{
    Float avgFrameRate = 0;
    if (sDuration > 0.0) {
        avgFrameRate = (totalNumberOfFrame / duration);
    }

    return avgFrameRate;
}

ECode MonkeyGetFrameRateEvent::WriteAverageFrameRate()
{
    Float avgFrameRate = 0.0f;
    Int32 totalNumberOfFrame = 0;
    AutoPtr<IFileWriter> fw;
    char buf[100] = {0};
    CFileWriter::New(*const_cast<String*>(&LOG_FILE), TRUE, (IFileWriter**)&fw);
    totalNumberOfFrame = sEndFrameNo - sStartFrameNo;
    avgFrameRate = GetAverageFrameRate(totalNumberOfFrame, sDuration);
    sprintf(buf, "%s:%.2f\n", sTestCaseName.string(), avgFrameRate);
    VALIDATE_NOT_NULL(fw)
    fw->WriteString(String(buf));
    return NOERROR;
}

String MonkeyGetFrameRateEvent::GetNumberOfFrames(const String& input)
{
    String noOfFrames("0");
    AutoPtr<IMatcher> m;
    NO_OF_FRAMES_PATTERN->Matcher(input, (IMatcher**)&m);
    Boolean ret = FALSE;
    m->Matches(&ret);
    if (ret) {
        AutoPtr<IMatchResult> mr;
        m->ToMatchResult((IMatchResult**)&mr);
        mr->Group(1, &noOfFrames);
    }
    return noOfFrames;
}

CARAPI MonkeyGetFrameRateEvent::Initializer()
{
    sStartFrameNo = 0;

    String path;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> pDir, pFile;
    env->GetExternalStorageDirectory((IFile**)&pDir);
    CFile::New(pDir, String("avgFrameRateOut.txt"), (IFile**)&pFile);
    pFile->GetAbsolutePath(&path);
    String* tmpPath = const_cast<String*>(&LOG_FILE);
    *tmpPath = path;

    const String str(".*\\(([a-f[A-F][0-9]].*?)\\s.*\\)");
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(str, (IPattern**)&NO_OF_FRAMES_PATTERN);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

