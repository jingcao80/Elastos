
#include "StopWatch.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

//=================================================================
// StopWatch::NullStopWatch
//=================================================================
const AutoPtr<NullStopWatch> StopWatch::NullStopWatch::INSTANCE = new NullStopWatch();

StopWatch::NullStopWatch::NullStopWatch()
{}

ECode StopWatch::NullStopWatch::Lap(
    /* [in] */ const String& lapLabel)
{
    // noop
    return NOERROR;
}

ECode StopWatch::NullStopWatch::StopAndLog(
    /* [in] */ const String& TAG,
    /* [in] */ Int32 timeThresholdToLog)
{
    // noop
    return NOERROR;
}

//=================================================================
// StopWatch
//=================================================================
StopWatch::StopWatch(
    /* [in] */ const String& label)
{
    CArrayList::New((IArrayList**)&mTimes);
    CArrayList::New((IArrayList**)&mLapLabels);
    mLabel = label;
    Lap(String(""));
}

AutoPtr<IStopWatch> StopWatch::Start(
    /* [in] */ const String& label)
{
    AutoPtr<StopWatch> obj = new StopWatch(label);
    return (IStopWatch*)obj;
}

ECode StopWatch::Lap(
    /* [in] */ const String& lapLabel)
{
    AutoPtr<ISystem> sysObj;
    CSystem::AcquireSingleton((ISystem**)&sysObj);
    Int64 currentMillis;
    sysObj->GetCurrentTimeMillis(&currentMillis);
    mTimes->Add(CoreUtils::Convert(currentMillis));
    mLapLabels->Add(CoreUtils::Convert(lapLabel));

    return NOERROR;
}

ECode StopWatch::StopAndLog(
    /* [in] */ const String& TAG,
    /* [in] */ Int32 timeThresholdToLog)
{
    Lap(String(""));

    AutoPtr<IInterface> item;
    mTimes->Get(0, (IInterface**)&item);
    Int64 start = CoreUtils::Unbox(item);

    Int32 size;
    mTimes->GetSize(&size);
    item = NULL;
    mTimes->Get(size - 1, (IInterface**)&item);
    Int64 stop = CoreUtils::Unbox(item);

    Int64 total = stop - start;
    if (total < timeThresholdToLog) return NOERROR;


    AutoPtr<StringBuilder> sb = new StringBuilder();
    sb->Append(mLabel);
    sb->Append(",");
    sb->Append(total);
    sb->Append(": ");

    Int64 last = start;
    mTimes->GetSize(&size);
    for (Int32 i = 1; i < size; i++) {
        AutoPtr<IInterface> value;
        mTimes->Get(i, (IInterface**)&value);
        Int64 current = CoreUtils::Unbox(value);
        value = NULL;
        mLapLabels->Get(i, (IInterface**)&value);
        sb->Append(CoreUtils::Unbox(value));
        sb->Append(",");
        sb->Append((current - last));
        sb->Append(" ");
        last = current;
    }

    Logger::V(TAG, sb->ToString());

    return NOERROR;
}

AutoPtr<IStopWatch> StopWatch::GetNullStopWatch()
{
    return (IStopWatch*)NullStopWatch::INSTANCE;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
