#ifndef __ELASTOS_DROID_MEDIA_SUBTITLETRACK_H__
#define __ELASTOS_DROID_MEDIA_SUBTITLETRACK_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IPair;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISortedMap;
using Elastos::Utility::IVector;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Media {

class SubtitleTrack
    : public Object
    , public ISubtitleTrack
    , public IMediaTimeProviderOnMediaTimeListener
{
public:
    class CueList
        : public Object
        , public ISubtitleTrackCueList
    {
    public:
        class CueListIterable
            : public Object
            , public IIterable
        {
        public:
            CAR_INTERFACE_DECL()

            CueListIterable();

            CARAPI constructor(
                /* [in] */ Int64 value,
                /* [in] */ Int64 value_,
                /* [in] */ CueList* cueList);

            CARAPI GetIterator(
                /* [out] */ IIterator** it);

        private:
            Int64 mLTimeMs;
            Int64 mTimeMs;
            CueList* mHost;
        };

        class EntryIterator
            : public IIterator
            , public Object
        {
        public:
            CAR_INTERFACE_DECL()

            EntryIterator();

            CARAPI constructor(
                /* [in] */ ISortedMap* cues,
                /* [in] */ CueList* host);

            CARAPI HasNext(
                /* [out] */ Boolean* result);

            CARAPI GetNext(
                /* [out] */ IPair** result);

            CARAPI GetNext(
                /* [out] */ IInterface** object);

            CARAPI Remove();

        private:
            CARAPI_(void) NextKey();

        private:
            Int64 mCurrentTimeMs;
            AutoPtr<IIterator> mListIterator;
            Boolean mDone;
            AutoPtr<ISortedMap> mRemainingCues;
            AutoPtr<IIterator> mLastListIterator;
            AutoPtr<IPair> mLastEntry;
            CueList* mHost;
        };

    public:
        CueList();

        virtual ~CueList();

        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ SubtitleTrack* host);

        CARAPI Add(
            /* [in] */ ISubtitleTrackCue* cue);

        CARAPI Remove(
            /* [in] */ ISubtitleTrackCue* cue);

        CARAPI EntriesBetween(
            /* [in] */ Int64 lastTimeMs,
            /* [in] */ Int64 timeMs,
            /* [out] */ IIterable** result);

        CARAPI NextTimeAfter(
            /* [in] */ Int64 timeMs,
            /* [out] */ Int64* result);

        CARAPI GetDEBUG(
            /* [out] */ Boolean* result);

        CARAPI SetDEBUG(
            /* [in] */ Boolean dg);

    private:
        CARAPI_(Boolean) AddEvent(
            /* [in] */ ISubtitleTrackCue* cue,
            /* [in] */ Int64 timeMs);

        CARAPI_(void) RemoveEvent(
            /* [in] */ ISubtitleTrackCue* cue,
            /* [in] */ Int64 timeMs);

    public:
        Boolean DEBUG;

    private:
        static const String TAG;
        // simplistic, inefficient implementation
        AutoPtr<ISortedMap> mCues;

        AutoPtr<SubtitleTrack> mHost;
    };

    /** @hide */
    class Cue
        : public Object
        , public ISubtitleTrackCue
    {
    public:
        CAR_INTERFACE_DECL()

        Cue();

        ~Cue();

        CARAPI constructor();

        CARAPI OnTime(
            /* [in] */ Int64 timeMs);

        CARAPI GetStartTimeMs(
            /* [out] */ Int64* startTimeMs);

        CARAPI SetStartTimeMs(
            /* [in] */ Int64 startTimeMs);

        CARAPI GetEndTimeMs(
            /* [out] */ Int64* endTimeMs);

        CARAPI SetEndTimeMs(
            /* [in] */ Int64 endTimeMs);

        CARAPI GetRunID(
            /* [out] */ Int64* runID);

        CARAPI SetRunID(
            /* [in] */ Int64 runID);

        CARAPI GetInnerTimesMs(
            /* [out, callee] */ ArrayOf<Int64>** innerTimesMs);

        CARAPI SetInnerTimesMs(
            /* [in] */ ArrayOf<Int64>* innerTimesMs);
    public:
        Int64 mStartTimeMs;
        Int64 mEndTimeMs;
        AutoPtr<ArrayOf<Int64> > mInnerTimesMs;
        Int64 mRunID;

        /** @hide */
        Cue* mNextInRun;
    };

private:
    class Run
        : public Object
        , public ISubtitleTrackRun
    {
    public:
        CAR_INTERFACE_DECL()

        Run();

        ~Run();

        //TODO
        //StoreByEndTimeMs(
        //    [in] ILongSparseArray* runsByEndTime);

        CARAPI RemoveAtEndTimeMs();

    public:
        AutoPtr<Cue> mFirstCue;
        Run* mNextRunAtEndTimeMs;
        Run* mPrevRunAtEndTimeMs;
        Int64 mEndTimeMs;
        Int64 mRunID;

    private:
        Int64 mStoredEndTimeMs;
    };

    class SubTrackRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SubTrackRunnable(
            /* [in] */ SubtitleTrack* host,
            /* [in] */ Int64 thenMs)
            : mHost(host)
            , mThenMs(thenMs)
        {}

        CARAPI Run();

    public:
        SubtitleTrack* mHost;
        Int64 mThenMs;
    };

public:
    CAR_INTERFACE_DECL()

    SubtitleTrack();

    virtual ~SubtitleTrack();

    CARAPI constructor(
        /* [in] */ IMediaFormat* format);

    CARAPI GetFormat(
        /* [out] */ IMediaFormat** result);

    virtual CARAPI OnData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean eos,
        /* [in] */ Int64 runID) = 0;

    virtual CARAPI GetRenderingWidget(
        /* [out] */ ISubtitleTrackRenderingWidget** result) = 0;

    virtual CARAPI UpdateView(
        /* [in] */ IVector* activeCues) = 0;

    /**
     * @hide
     */
    CARAPI OnTimedEvent(
        /* [in] */ Int64 timeUs);

    /**
     * @hide
     */
    CARAPI OnSeek(
        /* [in] */ Int64 timeUs);

    /**
     * @hide
     */
    CARAPI OnStop();

    /** @hide */
    CARAPI Show();

    /** @hide */
    CARAPI Hide();

    CARAPI SetTimeProvider(
        /* [in] */ IMediaTimeProvider* timeProvider);

    /** @hide update mRunsByEndTime with given end time */
    CARAPI SetRunDiscardTimeMs(
        /* [in] */ Int64 runID,
        /* [in] */ Int64 timeMs);

    CARAPI IsTimedText(
        /* [out] */ Boolean* result);

    CARAPI OnData(
        /* [in] */ ISubtitleData* data);

protected:
    /** @hide */
    CARAPI UpdateActiveCues(
        /* [in] */ Boolean rebuild,
        /* [in] */ Int64 timeMs);

    CARAPI Finalize();

    /** @hide */
    CARAPI ClearActiveCues();

    /** @hide */
    CARAPI ScheduleTimedEvents();

    CARAPI AddCue(
        /* [in] */ ISubtitleTrackCue* cue,
        /* [out] */ Boolean* result);

     /** @hide update mRunsByEndTime (with default end time) */
    CARAPI FinishedRun(
        /* [in] */ Int64 runID);

private:
    CARAPI RemoveRunsByEndTimeIndex(
        /* [in] */ Int32 ix);

    CARAPI TakeTime(
        /* [in] */ Int64 timeMs);

public:
    Boolean DEBUG;

protected:
    /** @hide TODO private */
    // AutoPtr<ILongSparseArray> mRunsByEndTime;
    /** @hide TODO private */
    // AutoPtr<ILongSparseArray> mRunsByID;
    /** @hide TODO private */
    AutoPtr<ISubtitleTrackCueList> mCues;
    /** @hide TODO private */
    AutoPtr<IVector> mActiveCues;
    /** @hide */
    Boolean mVisible;
    /** @hide */
    AutoPtr<IHandler> mHandler;
    /** @hide */
    AutoPtr<IMediaTimeProvider> mTimeProvider;

private:
    Int64 mLastUpdateTimeMs;
    Int64 mLastTimeMs;
    AutoPtr<IRunnable> mRunnable;
    AutoPtr<IMediaFormat> mFormat;
    Int64 mNextScheduledTimeMs;
    static const String TAG;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // __ELASTOS_DROID_MEDIA_SUBTITLETRACK_H__