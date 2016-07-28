
#include "elastos/droid/systemui/statusbar/GestureRecorder.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

////////////////////////////////////////////////////////////////////
//        class GestureRecorder::GHandler
////////////////////////////////////////////////////////////////////
GestureRecorder::GHandler::GHandler(
    /* [in] */ GestureRecorder* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode GestureRecorder::GHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == SAVE_MESSAGE) {
        mHost->Save();
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////////////
//        class GestureRecorder::Gesture::MotionEventRecord
////////////////////////////////////////////////////////////////////
GestureRecorder::Gesture::MotionEventRecord::MotionEventRecord(
    /* [in] */ Int64 when,
    /* [in] */ IMotionEvent* event)
{
    mTime = when;
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    helper->Obtain(event, (IMotionEvent**)&mEvent);
}

String GestureRecorder::Gesture::MotionEventRecord::ActionName(
    /* [in] */ Int32 action)
{
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            return String("down");
        case IMotionEvent::ACTION_UP:
            return String("up");
        case IMotionEvent::ACTION_MOVE:
            return String("move");
        case IMotionEvent::ACTION_CANCEL:
            return String("cancel");
        default:
            return StringUtils::ToString(action);
    }
}

String GestureRecorder::Gesture::MotionEventRecord::ToJson()
{
    Int32 action = 0;
    mEvent->GetAction(&action);
    Float x = 0, y = 0, size = 0, p = 0;
    mEvent->GetRawX(&x);
    mEvent->GetRawY(&y);
    mEvent->GetSize(&size);
    mEvent->GetPressure(&p);
    String str;
    str.AppendFormat(
            ("{\"type\":\"motion\", \"time\":%d, \"action\":\"%s\", \"x\":%.2f, \"y\":%.2f, \"s\":%.2f, \"p\":%.2f}"),
            mTime, ActionName(action).string(), x, y, size, p
            );
    return str;
}

////////////////////////////////////////////////////////////////////
//        class GestureRecorder::Gesture::TagRecord
////////////////////////////////////////////////////////////////////
GestureRecorder::Gesture::TagRecord::TagRecord(
    /* [in] */ Int64 when,
    /* [in] */ const String& tag,
    /* [in] */ const String& info)
    : mTag(tag)
    , mInfo(info)
{
    mTime = when;
}

String GestureRecorder::Gesture::TagRecord::ToJson()
{
    String str;
    str.AppendFormat("{\"type\":\"tag\", \"time\":%ld, \"tag\":\"%s\", \"info\":\"%s\"}",
            mTime,
            mTag.string(),
            mInfo.string()
            );
    return str;
}

////////////////////////////////////////////////////////////////////
//        class GestureRecorder::Gesture
////////////////////////////////////////////////////////////////////
GestureRecorder::Gesture::Gesture()
    : mDownTime(-1)
    , mComplete(FALSE)
{
    CLinkedList::New((ILinkedList**)&mRecords);
}

void GestureRecorder::Gesture::Add(
    /* [in] */ IMotionEvent* ev)
{
    Int64 t = 0;
    IInputEvent::Probe(ev)->GetEventTime(&t);
    AutoPtr<MotionEventRecord> record = new MotionEventRecord(t, ev);
    mRecords->Add((IObject*)record->Probe(EIID_IObject));
    if (mDownTime < 0) {
        ev->GetDownTime(&mDownTime);
    }
    else {
        Int64 dt = 0;
        ev->GetDownTime(&dt);
        if (mDownTime != dt) {
            Logger::W(TAG, "Assertion failure in GestureRecorder: event downTime (%lld ) does not match gesture downTime (%lld)", dt, mDownTime);
        }
    }

    Int32 masked = 0;
    switch (ev->GetActionMasked(&masked), masked) {
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mComplete = TRUE;
    }
}

void GestureRecorder::Gesture::Tag(
    /* [in] */ Int64 when,
    /* [in] */ const String& tag,
    /* [in] */ const String& info)
{
    AutoPtr<TagRecord> r = new TagRecord(when, tag, info);
    mRecords->Add((IObject*)r->Probe(EIID_IObject));
    mTags.Insert(tag);
}

Boolean GestureRecorder::Gesture::IsComplete()
{
    return mComplete;
}

String GestureRecorder::Gesture::ToJson()
{
    StringBuilder sb;
    Boolean first = TRUE;
    sb.Append("[");
    AutoPtr<IIterator> ator;
    mRecords->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<Record> r = (Record*) IObject::Probe(obj);
        if (!first) sb.Append(", ");
        first = FALSE;
        sb.Append(r->ToJson());
    }
    sb.Append("]");
    return sb.ToString();
}

////////////////////////////////////////////////////////////////////
//        class GestureRecorder
////////////////////////////////////////////////////////////////////
const Boolean GestureRecorder::DEBUG = FALSE; 
const String GestureRecorder::TAG("GestureRecorder"); //GestureRecorder.class.getSimpleName()
const Int64 GestureRecorder::SAVE_DELAY = 5000; // ms
const Int32 GestureRecorder::SAVE_MESSAGE = 6351;

GestureRecorder::GestureRecorder(
    /* [in] */ const String& filename)
    : mLastSaveLen(-1)
{
    mHandler = new GHandler(this);

    mLogfile = filename;
    CLinkedList::New((ILinkedList**)&mGestures);
    mCurrentGesture = NULL;
}

void GestureRecorder::Add(
    /* [in] */ IMotionEvent* ev)
{
    {    AutoLock syncLock(mGestures);
        if (mCurrentGesture == NULL || mCurrentGesture->IsComplete()) {
            mCurrentGesture = new Gesture();
            mGestures->Add((IObject*)mCurrentGesture->Probe(EIID_IObject));
        }
        mCurrentGesture->Add(ev);
    }
    SaveLater();
}

void GestureRecorder::Tag(
    /* [in] */ Int64 when,
    /* [in] */ const String& tag,
    /* [in] */ const String& info)
{
    {    AutoLock syncLock(mGestures);
        if (mCurrentGesture == NULL) {
            mCurrentGesture = new Gesture();
            mGestures->Add((IObject*)mCurrentGesture->Probe(EIID_IObject));
        }
        mCurrentGesture->Tag(when, tag, info);
    }
    SaveLater();
}

void GestureRecorder::Tag(
    /* [in] */ Int64 when,
    /* [in] */ const String& tag)
{
    Tag(when, tag, String(NULL));
}

void GestureRecorder::Tag(
    /* [in] */ const String& tag)
{
    Tag(SystemClock::GetUptimeMillis(), tag, String(NULL));
}

void GestureRecorder::Tag(
    /* [in] */ const String& tag,
    /* [in] */ const String& info)
{
    Tag(SystemClock::GetUptimeMillis(), tag, info);
}

/**
 * Generates a JSON string capturing all completed gestures.
 * Not threadsafe; call with a lock.
 */
String GestureRecorder::ToJsonLocked()
{
    StringBuilder sb;
    Boolean first = TRUE;
    sb.Append("[");
    Int32 count = 0;

    AutoPtr<IIterator> ator;
    mGestures->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<Gesture> g = (Gesture*)IObject::Probe(obj);

        if (!g->IsComplete()) continue;
        if (!first) sb.Append("," );
        first = FALSE;
        sb.Append(g->ToJson());
        count++;
    }

    mLastSaveLen = count;
    sb.Append("]");
    return sb.ToString();
}

String GestureRecorder::ToJson()
{
    String s;
    {    AutoLock syncLock(mGestures);
        s = ToJsonLocked();
    }
    return s;
}

void GestureRecorder::SaveLater()
{
    mHandler->RemoveMessages(SAVE_MESSAGE);
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessageDelayed(SAVE_MESSAGE, SAVE_DELAY, &tmp);
}

void GestureRecorder::Save()
{
    String log;
    {    AutoLock syncLock(mGestures);
        AutoPtr<IFileWriter> writer;
        CFileWriter::New(mLogfile, /*append=*/ TRUE, (IFileWriter**)&writer);
        AutoPtr<IBufferedWriter> w;
        CBufferedWriter::New(IWriter::Probe(writer), (IBufferedWriter**)&w);
        AutoPtr<ICharSequence> value;
        CString::New(ToJsonLocked() + "\n", (ICharSequence**)&value);
        if (FAILED(IAppendable::Probe(w)->Append(value))) {
            log.AppendFormat("Couldn't write gestures to %s", mLogfile.string());
            Logger::E(TAG, log);
            mLastSaveLen = -1;
        }
        ICloseable::Probe(w)->Close();
        mGestures->Clear();
        // If we have a pending gesture, push it back
        if (mCurrentGesture != NULL && !mCurrentGesture->IsComplete()) {
            mGestures->Add((IObject*)mCurrentGesture->Probe(EIID_IObject));
        }
        if (DEBUG) {
            log = NULL;
            log.AppendFormat("Wrote %d complete gestures to %s", mLastSaveLen, mLogfile.string());
            Logger::V(TAG, log);
        }
    }
}

void GestureRecorder::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Save();
    if (mLastSaveLen >= 0) {
        pw->Println(StringUtils::ToString(mLastSaveLen) + " gestures written to " + mLogfile);
    }
    else {
        pw->Println(String("error writing gestures"));
    }
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
