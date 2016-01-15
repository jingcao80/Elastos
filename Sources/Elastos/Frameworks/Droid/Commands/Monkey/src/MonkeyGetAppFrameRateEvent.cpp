#include "MonkeyGetAppFrameRateEvent.h"
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

Int64 MonkeyGetAppFrameRateEvent::sStartTime;
Int64 MonkeyGetAppFrameRateEvent::sEndTime;
Float MonkeyGetAppFrameRateEvent::sDuration;
String MonkeyGetAppFrameRateEvent::sActivityName = String("");
String MonkeyGetAppFrameRateEvent::sTestCaseName = String("");
Int32 MonkeyGetAppFrameRateEvent::sStartFrameNo = 0;
Int32 MonkeyGetAppFrameRateEvent::sEndFrameNo = 0;
const String MonkeyGetAppFrameRateEvent:: TAG = String("MonkeyGetAppFrameRateEvent");
volatile const String MonkeyGetAppFrameRateEvent::LOG_FILE;
AutoPtr<IPattern> MonkeyGetAppFrameRateEvent::NO_OF_FRAMES_PATTERN;

MonkeyGetAppFrameRateEvent::MonkeyGetAppFrameRateEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , GET_APP_FRAMERATE_TMPL("dumpsys gfxinfo %s")
{
    Initializer();
}

MonkeyGetAppFrameRateEvent::MonkeyGetAppFrameRateEvent(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName,
    /* [in] */ const String& testCaseName)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mStatus(status)
{
    sActivityName = activityName;
    sTestCaseName = testCaseName;
}

MonkeyGetAppFrameRateEvent::MonkeyGetAppFrameRateEvent(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mStatus(status)
{
    sActivityName = activityName;
}

MonkeyGetAppFrameRateEvent::MonkeyGetAppFrameRateEvent(
    /* [in] */ const String& status)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mStatus(status)
{}

ECode MonkeyGetAppFrameRateEvent::Init(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName,
    /* [in] */ const String& testCaseName)
{
    mStatus = status;
    sActivityName = activityName;
    sTestCaseName = testCaseName;
    Initializer();
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

ECode MonkeyGetAppFrameRateEvent::Init(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName)
{
    mStatus = status;
    sActivityName = activityName;
    Initializer();
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

ECode MonkeyGetAppFrameRateEvent::Init(
    /* [in] */ const String& status)
{
    mStatus = status;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

Int32 MonkeyGetAppFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    char buf[500];
    sprintf(buf, GET_APP_FRAMERATE_TMPL.string(), sActivityName.string());
    String cmd(buf);
    FILE* pFile = popen(cmd.string(), "r");
    if (pFile) {
        String output = GetNumberOfFrames(pFile);
        if(!output.IsNull()) {
            if(mStatus.Equals("start")) {
                sStartFrameNo = StringUtils::ParseInt32(output);
                sStartTime = SystemClock::GetUptimeMillis();
            } else if(mStatus.Equals("end")) {
                sEndFrameNo = StringUtils::ParseInt32(output);
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

String MonkeyGetAppFrameRateEvent::GetNumberOfFrames(
    /* [in] */ FILE *reader)
{
    String noOfFrames;
    char oneLine[500];
    do {
        fgets(oneLine, sizeof(oneLine), reader);
        String line(oneLine);
        if(line.IsNull())
            break;
        AutoPtr<IMatcher> m;
        NO_OF_FRAMES_PATTERN->Matcher(line, (IMatcher**)&m);
        Boolean ret = FALSE;
        m->Matches(&ret);
        if (ret) {
            AutoPtr<IMatchResult> mr;
            m->ToMatchResult((IMatchResult**)&mr);
            mr->Group(1, &noOfFrames);
            break;
        }
    } while(TRUE);
    return noOfFrames;
}

Float MonkeyGetAppFrameRateEvent::GetAverageFrameRate(
    /* [in] */ Int32 totalNumberOfFrame,
    /* [in] */ Float duration)
{
    Float avgFrameRate = 0;
    if (sDuration > 0.0) {
        avgFrameRate = (totalNumberOfFrame / duration);
    }
    return avgFrameRate;
}

ECode MonkeyGetAppFrameRateEvent::WriteAverageFrameRate()
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

CARAPI MonkeyGetAppFrameRateEvent::Initializer()
{
    sStartFrameNo = 0;

    String path;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> pDir, pFile;
    env->GetExternalStorageDirectory((IFile**)&pDir);
    CFile::New(pDir, String("avgAppFrameRateOut.txt"), (IFile**)&pFile);
    pFile->GetAbsolutePath(&path);
    String* tmpPath = const_cast<String*>(&LOG_FILE);
    *tmpPath = path;

    const String str(".* ([0-9]*) frames rendered");
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(str, (IPattern**)&NO_OF_FRAMES_PATTERN);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
