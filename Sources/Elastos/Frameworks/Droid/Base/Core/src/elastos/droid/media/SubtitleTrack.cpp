#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/SubtitleTrack.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CPair.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/CoreUtils.h>
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Utility::CPair;
using Elastos::Utility::CVector;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger64;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Media {
//===============================================================
            // SubtitleTrack::CueList
//===============================================================
const String SubtitleTrack::CueList::TAG("CueList");

SubtitleTrack::CueList::CueList()
    : DEBUG(FALSE)
{}

SubtitleTrack::CueList::~CueList()
{}

CAR_INTERFACE_IMPL(SubtitleTrack::CueList, Object, ISubtitleTrackCueList);

ECode SubtitleTrack::CueList::constructor(
    /* [in] */ SubtitleTrack* host)
{
    mHost = host;
    return NOERROR;
}

ECode SubtitleTrack::CueList::Add(
    /* [in] */ ISubtitleTrackCue* cue)
{
    // ignore non-positive-duration cues
    if (((Cue*)cue)->mStartTimeMs >= ((Cue*)cue)->mEndTimeMs)
        return NOERROR;

    Boolean flag = AddEvent(cue, ((Cue*)cue)->mStartTimeMs);
    if (!flag) return NOERROR;
    Int64 UNUSED(lastTimeMs) = ((Cue*)cue)->mStartTimeMs;
    AutoPtr<ArrayOf<Int64> > array = ((Cue*)cue)->mInnerTimesMs;
    if (array != NULL) {
        Int64 length = array->GetLength();
        for (Int64 timeMs = 0; timeMs < length; timeMs++) {
            if ((timeMs > mHost->mLastTimeMs) && (timeMs < ((Cue*)cue)->mEndTimeMs)) {
                flag = AddEvent(cue, timeMs);
                lastTimeMs = timeMs;
            }
        }
    }

    flag = AddEvent(cue, ((Cue*)cue)->mEndTimeMs);
    return NOERROR;
}

ECode SubtitleTrack::CueList::Remove(
    /* [in] */ ISubtitleTrackCue* cue)
{
    RemoveEvent(cue, ((Cue*)cue)->mStartTimeMs);
    if (((Cue*)cue)->mInnerTimesMs != NULL) {
        AutoPtr<ArrayOf<Int64> > mit = ((Cue*)cue)->mInnerTimesMs;
        Int64 length = mit->GetLength();
        for (Int64 timeMs = 0; timeMs < length; timeMs++) {
            RemoveEvent(cue, timeMs);
        }
    }
    RemoveEvent(cue, ((Cue*)cue)->mEndTimeMs);
    return NOERROR;
}

ECode SubtitleTrack::CueList::EntriesBetween(
    /* [in] */ Int64 lastTimeMs,
    /* [in] */ Int64 timeMs,
    /* [out] */ IIterable** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<CueListIterable> cli = new CueListIterable();

    *result = (IIterable*)(cli.Get());
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SubtitleTrack::CueList::NextTimeAfter(
    /* [in] */ Int64 timeMs,
    /* [out] */ Int64* result)
{
    AutoPtr<ISortedMap> tail = NULL;
    ECode ec = NOERROR;
    // try {
    ec = mCues->GetTailMap(CoreUtils::Convert(timeMs + 1), (ISortedMap**)&tail);
    FAIL_GOTO(ec, EXIT);
    if (tail != NULL) {
        AutoPtr<IInterface> outface;
        ec = tail->GetFirstKey((IInterface**)&outface);
        FAIL_GOTO(ec, EXIT);
        AutoPtr<IInteger64> fstKey = IInteger64::Probe(outface);
        Int64 value;
        ec = fstKey->GetValue(&value);
        FAIL_GOTO(ec, EXIT);
        *result = value;
        return NOERROR;
    } else {
        *result = -1;
        return NOERROR;
    }
    // } catch(IllegalArgumentException e) {
        // return -1;
    // } catch(NoSuchElementException e) {
        // return -1;
    // }
EXIT:
    *result = -1;
    return NOERROR;
}

Boolean SubtitleTrack::CueList::AddEvent(
    /* [in] */ ISubtitleTrackCue* cue,
    /* [in] */ Int64 timeMs)
{
    AutoPtr<IInterface> interf;
    mCues->Get(CoreUtils::Convert(timeMs).Get(), (IInterface**)&interf);
    AutoPtr<IVector> cues = IVector::Probe(interf);
    Boolean bContains = FALSE;
    cues->Contains(cue, &bContains);
    if (cues == NULL) {
        CVector::New(2, (IVector**)&cues);
        mCues->Put(CoreUtils::Convert(timeMs).Get(), cues.Get());
    } else if (bContains) {
        // do not duplicate cues
        return FALSE;
    }

    cues->Add(cue, &bContains);
    return TRUE;
}

void SubtitleTrack::CueList::RemoveEvent(
    /* [in] */ ISubtitleTrackCue* cue,
    /* [in] */ Int64 timeMs)
{
    AutoPtr<IInterface> interf;
    mCues->Get(CoreUtils::Convert(timeMs), (IInterface**)&interf);
    AutoPtr<IVector> cues = IVector::Probe(interf);

    if (cues != NULL) {
        cues->Remove(cue);
        Int32 size;
        cues->GetSize(&size);
        if (size == 0) {
            mCues->Remove(CoreUtils::Convert(timeMs));
        }
    }
}

ECode SubtitleTrack::CueList::GetDEBUG(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DEBUG;
    return NOERROR;
}

ECode SubtitleTrack::CueList::SetDEBUG(
    /* [in] */ Boolean dg)
{
    DEBUG = dg;
    return NOERROR;
}

//===============================================================
            // SubtitleTrack::CueList::EntryIterator
//===============================================================
CAR_INTERFACE_IMPL(SubtitleTrack::CueList::EntryIterator, Object, IIterator)

SubtitleTrack::CueList::EntryIterator::EntryIterator()
    : mCurrentTimeMs(0)
    , mDone(FALSE)
{}

ECode SubtitleTrack::CueList::EntryIterator::constructor(
    /* [in] */ ISortedMap* cues,
    /* [in] */ CueList* host)
{
    mHost = host;
    Boolean bRet = FALSE;
    mHost->GetDEBUG(&bRet);
    if (bRet) Slogger::V(TAG, "%p", cues);
    mRemainingCues = cues;
    mLastListIterator = NULL;
    NextKey();
    return NOERROR;
}

ECode SubtitleTrack::CueList::EntryIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mDone;
    return NOERROR;
}

ECode SubtitleTrack::CueList::EntryIterator::GetNext(
    /* [out] */ IPair** result)
{
    VALIDATE_NOT_NULL(result);
    if (mDone) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    AutoPtr<IInterface> interListIter;
    mListIterator->GetNext((IInterface**)&interListIter);
    CPair::New(CoreUtils::Convert(mCurrentTimeMs).Get(), interListIter.Get(), (IPair**)&mLastEntry);
    mLastListIterator = mListIterator;
    Boolean hasNext = FALSE;
    mListIterator->HasNext(&hasNext);
    if (!hasNext) {
        NextKey();
    }
    *result = mLastEntry;
    return NOERROR;
}

ECode SubtitleTrack::CueList::EntryIterator::Remove()
{
    // only allow removing end tags
    AutoPtr<IInterface> second;
    mLastEntry->GetSecond((IInterface**)&second);
    AutoPtr<ISubtitleTrackCue> stc = ISubtitleTrackCue::Probe(second);
    Int64 endTimeMs;
    stc->GetEndTimeMs(&endTimeMs);

    AutoPtr<IInterface> first;
    mLastEntry->GetFirst((IInterface**)&first);
    AutoPtr<IInteger64> f = IInteger64::Probe(first);
    Int64 fValue;
    f->GetValue(&fValue);
    if (mLastListIterator == NULL || endTimeMs != fValue) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // remove end-cue
    mLastListIterator->Remove();
    mLastListIterator = NULL;
    AutoPtr<IInterface> values;
    AutoPtr<ISortedMap> icus = mHost->mCues;
    icus->Get(first, (IInterface**)&values);
    AutoPtr<IVector> vector = IVector::Probe(values);
    Int32 size;
    vector->GetSize(&size);
    if (size == 0) {
        icus->Remove(first);
    }

    // remove rest of the cues
    Cue* cue = (Cue*)stc.Get();
    mHost->RemoveEvent(cue, cue->mStartTimeMs);
    AutoPtr<ArrayOf<Int64> > itms = cue->mInnerTimesMs;
    if (itms != NULL) {
        Int32 length = itms->GetLength();
        for (Int64 timeMs = 0; timeMs < (Int64)length; timeMs++) {
            mHost->RemoveEvent(cue, timeMs);
        }
    }
    return NOERROR;
}

ECode SubtitleTrack::CueList::EntryIterator::GetNext(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}

void SubtitleTrack::CueList::EntryIterator::NextKey()
{
    Boolean hasNext = FALSE;
    ECode ec = NOERROR;
    do {
        // try {
        if (mRemainingCues == NULL) {
            return;
        }
        AutoPtr<IInterface> stMapfirst;
        mRemainingCues->GetFirstKey((IInterface**)&stMapfirst);
        AutoPtr<IInteger64> inter = IInteger64::Probe(stMapfirst);
        ec = inter->GetValue(&mCurrentTimeMs);
        FAIL_GOTO(ec, EXIT);
        AutoPtr<IInterface> inter_;
        ec = mRemainingCues->Get(CoreUtils::Convert(mCurrentTimeMs).Get(), (IInterface**)&inter_);
        FAIL_GOTO(ec, EXIT);
        AutoPtr<IVector> vector = IVector::Probe(inter_);
        ec = vector->GetIterator((IIterator**)&mListIterator);
        FAIL_GOTO(ec, EXIT);
        // try {
        ec = mRemainingCues->GetTailMap(CoreUtils::Convert(mCurrentTimeMs + 1).Get(), (ISortedMap**)&mRemainingCues);
        // } catch (IllegalArgumentException e) {
        if (!SUCCEEDED(ec)) {
            if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
                mRemainingCues = NULL;
            }
        }

        // }
        mDone = FALSE;
    // } catch (NoSuchElementException e) {
    } while (!(mListIterator->HasNext(&hasNext), hasNext));

    EXIT:
    {
        mDone = TRUE;
        mRemainingCues = NULL;
        mListIterator = NULL;
    }
}

//===============================================================
            // SubtitleTrack::CueList::CueListIterable
//===============================================================
CAR_INTERFACE_IMPL(SubtitleTrack::CueList::CueListIterable, Object, IIterable)

SubtitleTrack::CueList::CueListIterable::CueListIterable()
{}

ECode SubtitleTrack::CueList::CueListIterable::constructor(
    /* [in] */ Int64 value,
    /* [in] */ Int64 value_,
    /* [in] */ CueList* host)
{
    mLTimeMs = value;
    mTimeMs = value_;
    mHost = host;
    return NOERROR;
}

ECode SubtitleTrack::CueList::CueListIterable::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    Boolean bDeBug = FALSE;
    mHost->GetDEBUG(&bDeBug);
    if (bDeBug) {
        Slogger::E(TAG, "slice ( + %ld , %ld]=", mLTimeMs, mTimeMs);
    }
    AutoPtr<ISortedMap> outSortedMap;
    AutoPtr<ISortedMap> icues = mHost->mCues;
    icues->GetSubMap(CoreUtils::Convert(mLTimeMs + 1).Get(), CoreUtils::Convert(mTimeMs + 1).Get(), (ISortedMap**)&outSortedMap);
    AutoPtr<SubtitleTrack::CueList::EntryIterator> entry = new SubtitleTrack::CueList::EntryIterator();
    entry->constructor(outSortedMap.Get(), mHost);
    *it = (IIterator*)entry;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

//===============================================================
            // SubtitleTrack::Cue
//===============================================================
CAR_INTERFACE_IMPL(SubtitleTrack::Cue, Object, ISubtitleTrackCue)

SubtitleTrack::Cue::Cue()
{}

SubtitleTrack::Cue::~Cue()
{}

ECode SubtitleTrack::Cue::constructor()
{
    return NOERROR;
}

ECode SubtitleTrack::Cue::OnTime(
    /* [in] */ Int64 timeMs)
{
    return NOERROR;
}

ECode SubtitleTrack::Cue::GetStartTimeMs(
    /* [out] */ Int64* startTimeMs)
{
    VALIDATE_NOT_NULL(startTimeMs);
    *startTimeMs = mStartTimeMs;
    return NOERROR;
}

ECode SubtitleTrack::Cue::SetStartTimeMs(
    /* [in] */ Int64 startTimeMs)
{
    mStartTimeMs = startTimeMs;
    return NOERROR;
}

ECode SubtitleTrack::Cue::GetEndTimeMs(
    /* [out] */ Int64* endTimeMs)
{
    VALIDATE_NOT_NULL(endTimeMs);
    *endTimeMs = mEndTimeMs;
    return NOERROR;
}

ECode SubtitleTrack::Cue::SetEndTimeMs(
    /* [in] */ Int64 endTimeMs)
{
    mEndTimeMs = endTimeMs;
    return NOERROR;
}

ECode SubtitleTrack::Cue::GetRunID(
    /* [out] */ Int64* runID)
{
    VALIDATE_NOT_NULL(runID);
    *runID = mRunID;
    return NOERROR;
}

ECode SubtitleTrack::Cue::SetRunID(
    /* [in] */ Int64 runID)
{
    mRunID = runID;
    return NOERROR;
}

ECode SubtitleTrack::Cue::GetInnerTimesMs(
    /* [out] */ ArrayOf<Int64>** innerTimesMs)
{
    VALIDATE_NOT_NULL(innerTimesMs);
    *innerTimesMs = mInnerTimesMs;
    REFCOUNT_ADD(*innerTimesMs);
    return NOERROR;
}

ECode SubtitleTrack::Cue::SetInnerTimesMs(
    /* [in] */ ArrayOf<Int64>* innerTimesMs)
{
    mInnerTimesMs = innerTimesMs;
    return NOERROR;
}

//===============================================================
            // SubtitleTrack::Run
//===============================================================
SubtitleTrack::Run::Run()
    : mEndTimeMs(-1)
    , mRunID(0)
    , mStoredEndTimeMs(-1)
{}

SubtitleTrack::Run::~Run()
{}

CAR_INTERFACE_IMPL(SubtitleTrack::Run, Object, ISubtitleTrackRun)

ECode SubtitleTrack::Run::StoreByEndTimeMs(
    /* [in] */ IInt64SparseArray* runsByEndTime)
{
    // remove old value if any
    Int32 ix;
    runsByEndTime->IndexOfKey(mStoredEndTimeMs, &ix);
    if (ix >= 0) {
        if (mPrevRunAtEndTimeMs == NULL) {
            // Assert(this == runsByEndTime->ValueAt(ix));
            if (mNextRunAtEndTimeMs == NULL) {
                runsByEndTime->RemoveAt(ix);
            }
            else {
                runsByEndTime->SetValueAt(ix, (IInterface*)(IObject*)mNextRunAtEndTimeMs);
            }
        }
        RemoveAtEndTimeMs();
    }

    // add new value
    if (mEndTimeMs >= 0) {
        mPrevRunAtEndTimeMs = NULL;
        runsByEndTime->Get(mEndTimeMs, (IInterface**)&mNextRunAtEndTimeMs);
        if (mNextRunAtEndTimeMs != NULL) {
            mNextRunAtEndTimeMs->mPrevRunAtEndTimeMs = this;
        }
        runsByEndTime->Put(mEndTimeMs, (IInterface*)(IObject*)this);
        mStoredEndTimeMs = mEndTimeMs;
    }
    return NOERROR;
}

ECode SubtitleTrack::Run::RemoveAtEndTimeMs()
{
    AutoPtr<Run> prev = mPrevRunAtEndTimeMs;

    if (mPrevRunAtEndTimeMs != NULL) {
        mPrevRunAtEndTimeMs->mNextRunAtEndTimeMs = mNextRunAtEndTimeMs;
        mPrevRunAtEndTimeMs = NULL;
    }
    if (mNextRunAtEndTimeMs != NULL) {
        mNextRunAtEndTimeMs->mPrevRunAtEndTimeMs = prev;
        mNextRunAtEndTimeMs = NULL;
    }
    return NOERROR;
}

//===============================================================
            // SubtitleTrack::SubTrackRunnable
//===============================================================
CAR_INTERFACE_IMPL(SubtitleTrack::SubTrackRunnable, Object, IRunnable)

ECode SubtitleTrack::SubTrackRunnable::Run()
{
    // even with synchronized, it is possible that we are going
    // to do multiple updates as the runnable could be already
    // running.
    synchronized(this) {
        mHost->mRunnable = NULL;
        mHost->UpdateActiveCues(TRUE, mThenMs);
        mHost->UpdateView(mHost->mActiveCues.Get());
    }
    return NOERROR;
}

//===============================================================
            // SubtitleTrack
//===============================================================
const String SubtitleTrack::TAG("SubtitleTrack");

CAR_INTERFACE_IMPL_2(SubtitleTrack, Object, ISubtitleTrack, IMediaTimeProviderOnMediaTimeListener)

SubtitleTrack::SubtitleTrack()
    : DEBUG(FALSE)
    , mVisible(FALSE)
    , mNextScheduledTimeMs(-1)
{}

SubtitleTrack::~SubtitleTrack()
{}

ECode SubtitleTrack::constructor(
    /* [in] */ IMediaFormat* format)
{
    mFormat = format;
    AutoPtr<CueList> cl = new CueList();
    mCues = (ISubtitleTrackCueList*)cl;
    ClearActiveCues();
    mLastTimeMs = -1;
    return NOERROR;
}

ECode SubtitleTrack::GetFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFormat;
    return NOERROR;
}

/**
 * @hide
 */
ECode SubtitleTrack::OnTimedEvent(
    /* [in] */ Int64 timeUs)
{
    if (DEBUG) Slogger::E(TAG, "onTimedEvent %ld", timeUs);
    synchronized(this) {
        Int64 timeMs = timeUs / 1000;
        UpdateActiveCues(FALSE, timeMs);
        TakeTime(timeMs);
    }
    UpdateView(mActiveCues.Get());
    return ScheduleTimedEvents();
}

/**
 * @hide
 */
ECode SubtitleTrack::OnSeek(
    /* [in] */ Int64 timeUs)
{
    if (DEBUG) Slogger::E(TAG, "onSeek %ld", timeUs);
    synchronized(this) {
        Int64 timeMs = timeUs / 1000;
        UpdateActiveCues(TRUE, timeMs);
        TakeTime(timeMs);
    }
    UpdateView(mActiveCues.Get());
    return ScheduleTimedEvents();
}

/**
 * @hide
 */
ECode SubtitleTrack::OnStop()
{
    synchronized(this) {
        if (DEBUG) Slogger::E(TAG, "onStop");
        ClearActiveCues();
        mLastTimeMs = -1;
    }
    UpdateView(mActiveCues.Get());
    mNextScheduledTimeMs = -1;
    return mTimeProvider->NotifyAt(IMediaTimeProvider::NO_TIME, this);
}

/** @hide */
ECode SubtitleTrack::Show()
{
    if (mVisible) {
        return NOERROR;
    }

    mVisible = TRUE;
    AutoPtr<ISubtitleTrackRenderingWidget> renderingWidget;
    GetRenderingWidget((ISubtitleTrackRenderingWidget**)&renderingWidget);
    if (renderingWidget != NULL) {
        renderingWidget->SetVisible(TRUE);
    }
    if (mTimeProvider != NULL) {
        mTimeProvider->ScheduleUpdate(this);
    }
    return NOERROR;
}

/** @hide */
ECode SubtitleTrack::Hide()
{
    if (!mVisible) {
        return NOERROR;
    }

    if (mTimeProvider != NULL) {
        mTimeProvider->CancelNotifications(this);
    }
    AutoPtr<ISubtitleTrackRenderingWidget> renderingWidget;
    GetRenderingWidget((ISubtitleTrackRenderingWidget**)&renderingWidget);
    if (renderingWidget != NULL) {
        renderingWidget->SetVisible(FALSE);
    }
    mVisible = FALSE;
    return NOERROR;
}

ECode SubtitleTrack::SetTimeProvider(
    /* [in] */ IMediaTimeProvider* timeProvider)
{
    if (mTimeProvider.Get() == timeProvider) {
        return NOERROR;
    }
    if (mTimeProvider != NULL) {
        mTimeProvider->CancelNotifications(this);
    }
    mTimeProvider = timeProvider;
    if (mTimeProvider != NULL) {
        mTimeProvider->ScheduleUpdate(this);
    }
    return NOERROR;
}

/** @hide update mRunsByEndTime with given end time */
ECode SubtitleTrack::SetRunDiscardTimeMs(
    /* [in] */ Int64 runID,
    /* [in] */ Int64 timeMs)
{
    assert(0 && "TODO");
    if (runID != 0 && runID != ~0) {
        AutoPtr<Run> run;
        // Run run = mRunsByID->Get(runID);
        if (run != NULL) {
            run->mEndTimeMs = timeMs;
            // run->StoreByEndTimeMs(mRunsByEndTime);
        }
    }
    return NOERROR;
}

ECode SubtitleTrack::IsTimedText(
    /* [out] */ Boolean* result)
{
    AutoPtr<ISubtitleTrackRenderingWidget> str;
    GetRenderingWidget((ISubtitleTrackRenderingWidget**)&str);
    *result = (str == NULL);
    return NOERROR;
}


ECode SubtitleTrack::OnData(
    /* [in] */ ISubtitleData* data)
{
    Int64 runID;
    data->GetStartTimeUs(&runID);
    runID += 1;
    AutoPtr<ArrayOf<Byte> > result;
    data->GetData((ArrayOf<Byte>**)&result);
    OnData(result.Get(), TRUE/* eos */, runID);
    Int64 startTimeUs;
    data->GetStartTimeUs(&startTimeUs);
    Int64 durationUs;
    data->GetDurationUs(&durationUs);
    return SetRunDiscardTimeMs(runID,
            (startTimeUs + durationUs) / 1000);
}

/** @hide */
ECode SubtitleTrack::UpdateActiveCues(
    /* [in] */ Boolean rebuild,
    /* [in] */ Int64 timeMs)
{
    // out-of-order times mean seeking or new active cues being added
    // (during their own timespan)
    if (rebuild || mLastUpdateTimeMs > timeMs) {
        ClearActiveCues();
    }

    AutoPtr<IIterable> iterable;

    mCues->EntriesBetween(mLastUpdateTimeMs, timeMs, (IIterable**)&iterable);
    AutoPtr<IIterator> it;
    iterable->GetIterator((IIterator**)&it);
    Boolean flag = FALSE;
    while(it->HasNext(&flag), flag) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPair> event = IPair::Probe(obj);
        AutoPtr<ISubtitleTrackCue> st;
        event->GetSecond((IInterface**)&st);
        Cue* cue = (Cue*)st.Get();

        AutoPtr<IInterface> fst;
        event->GetFirst((IInterface**)&fst);
        AutoPtr<IInteger64> lfst = IInteger64::Probe(fst);
        Int64 vlfst;
        lfst->GetValue(&vlfst);
        if (cue->mEndTimeMs == vlfst) {
            // remove past cues
            if (DEBUG) Slogger::V(TAG, "Removing %p", cue);
            mActiveCues->Remove(st);
            if (cue->mRunID == 0) {
                it->Remove();
            }
        }
        else if (cue->mStartTimeMs == vlfst) {
            // add new cues
            // TRICKY: this will happen in start order
            if (DEBUG) Slogger::V(TAG, "Adding %p", cue);
            if (cue->mInnerTimesMs != NULL) {
                cue->OnTime(timeMs);
            }
            mActiveCues->Add(st);
        }
        else if (cue->mInnerTimesMs != NULL) {
            // cue is modified
            cue->OnTime(timeMs);
        }
    }

    /* complete any runs */
    assert(0 && "TODO"); // ILongSparseArray
/*        while (mRunsByEndTime.size() > 0 &&
           mRunsByEndTime.keyAt(0) <= timeMs) {
        removeRunsByEndTimeIndex(0); // removes element
    }*/

    mLastUpdateTimeMs = timeMs;
    return NOERROR;
}

ECode SubtitleTrack::Finalize()
{
    /* remove all cues (untangle all cross-links) */
    Int32 size;
    assert(0 && "TODO");
    // mRunsByEndTime.size();
    for(Int32 ix = size - 1; ix >= 0; ix--) {
        RemoveRunsByEndTimeIndex(ix);
    }
    return NOERROR;
}

/** @hide */
ECode SubtitleTrack::ClearActiveCues()
{
    Int32 size;
    mActiveCues->GetSize(&size);
    if (DEBUG) Slogger::V(TAG, "Clearing %d active cues", size);
    mActiveCues->Clear();
    mLastUpdateTimeMs = -1;
    return NOERROR;
}

/** @hide */
ECode SubtitleTrack::ScheduleTimedEvents()
{
    /* get times for the next event */
    if (mTimeProvider != NULL) {
    mCues->NextTimeAfter(mLastTimeMs, &mNextScheduledTimeMs);
    if (DEBUG) Slogger::D(TAG, "sched @%d after %d", mNextScheduledTimeMs, mLastTimeMs);
        mTimeProvider->NotifyAt(
                mNextScheduledTimeMs >= 0 ?
                    (mNextScheduledTimeMs * 1000) : IMediaTimeProvider::NO_TIME,
                this);
    }
    return NOERROR;
}

ECode SubtitleTrack::AddCue(
    /* [in] */ ISubtitleTrackCue* cue,
    /* [out] */ Boolean* result)
{
    mCues->Add(cue);

    Int64 runID;
    cue->GetRunID(&runID);
    Int64 endTimeMs;
    cue->GetEndTimeMs(&endTimeMs);
    Int64 startTimeMs;
    cue->GetStartTimeMs(&startTimeMs);
    if (runID != 0) {
        AutoPtr<Run> run;
        // Run run = mRunsByID.get(cue.mRunID);
        if (run == NULL) {
            run = new Run();
            // mRunsByID->Put(((Cue*)cue)->mRunID, run);

            run->mEndTimeMs = endTimeMs;
        } else if (run->mEndTimeMs < endTimeMs) {
            run->mEndTimeMs = endTimeMs;
        }

        // link-up cues in the same run
        ((Cue*)cue)->mNextInRun = run->mFirstCue;
        run->mFirstCue = (Cue*)cue;
    }

    // if a cue is added that should be visible, need to refresh view
    Int64 nowMs = -1;
    if (mTimeProvider != NULL) {
        // try {
    mTimeProvider->GetCurrentTimeUs(
            FALSE /* precise */, TRUE /* monotonic */, &nowMs);
    nowMs /= 1000;
        // } catch (IllegalStateException e) {
            // handle as it we are not playing
        // }
    }

    if (DEBUG) Slogger::V(TAG, "mVisible=%d, %d <= %d, %d >= %d", mVisible, startTimeMs, nowMs, endTimeMs, mLastTimeMs);

    if (mVisible &&
            startTimeMs <= nowMs &&
            // we don't trust nowMs, so check any cue since last callback
            endTimeMs >= mLastTimeMs) {
        if (mRunnable != NULL) {
            mHandler->RemoveCallbacks(mRunnable.Get());
        }
        Int64 thenMs = nowMs;
        mRunnable = new SubTrackRunnable(this, thenMs);
        // delay update so we don't update view on every cue.  TODO why 10?
        Boolean flag = FALSE;
        mHandler->PostDelayed(mRunnable, 10 /* delay */, &flag);
        if (flag) {
            if (DEBUG) Slogger::V(TAG, "scheduling update");
        } else {
            if (DEBUG) Slogger::W(TAG, "failed to schedule subtitle view update");
        }
        return true;
    }

    if ((mVisible &&
            endTimeMs >= mLastTimeMs &&
            startTimeMs < mNextScheduledTimeMs) ||
             mNextScheduledTimeMs < 0) {
        ScheduleTimedEvents();
    }

    *result = FALSE;
    return NOERROR;
}

 /** @hide update mRunsByEndTime (with default end time) */
ECode SubtitleTrack::FinishedRun(
    /* [in] */ Int64 runID)
{
    if (runID != 0 && runID != ~0) {
        AutoPtr<Run> run;
        assert(0 && "TODO");
        // mRunsByID->Get(runID);
        if (run != NULL) {
            // run->StoreByEndTimeMs(mRunsByEndTime);
        }
    }
    return NOERROR;
}

ECode SubtitleTrack::RemoveRunsByEndTimeIndex(
    /* [in] */ Int32 ix)
{
    AutoPtr<Run> run;
    //TODO
    // mRunsByEndTime->ValueAt(ix);
    while (run != NULL) {
        Cue* cue = run->mFirstCue;
        while (cue != NULL) {
            mCues->Remove(cue);
            Cue* nextCue = cue->mNextInRun;
            cue->mNextInRun = NULL;
            cue = nextCue;
        }
        // mRunsByID->Remove(run.mRunID);
        AutoPtr<Run> nextRun = run->mNextRunAtEndTimeMs;
        run->mPrevRunAtEndTimeMs = NULL;
        run->mNextRunAtEndTimeMs = NULL;
        run = nextRun;
    }
    //TODO
    // return mRunsByEndTime->RemoveAt(ix);
    return NOERROR;
}

ECode SubtitleTrack::TakeTime(
    /* [in] */ Int64 timeMs)
{
    mLastTimeMs = timeMs;
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media