#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/WebVttTrack.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::AutoLock;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Media {

const String WebVttTrack::TAG(String("WebVttTrack"));

WebVttTrack::WebVttTrack()
    : mCurrentRunID(0)
{}

WebVttTrack::~WebVttTrack()
{}

CAR_INTERFACE_IMPL_3(WebVttTrack, Object, IWebVttTrack, ISubtitleTrack, IWebVttCueListener)

ECode WebVttTrack::constructor(
    /* [in] */ IWebVttRenderingWidget* renderingWidget,
    /* [in] */ IMediaFormat* format)
{
    SubtitleTrack::constructor(format);
    mRenderingWidget = (WebVttRenderingWidget*)renderingWidget;
    return NOERROR;
}

ECode WebVttTrack::GetRenderingWidget(
    /* [out] */ ISubtitleTrackRenderingWidget** result)
{
    // IWebVttRenderingWidget
    VALIDATE_NOT_NULL(result);
    *result = mRenderingWidget;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebVttTrack::OnData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean eos,
    /* [in] */ Int64 runID)
{
    // try {
        // String str = new String(data, "UTF-8");
        String str(*data);

        // implement intermixing restriction for WebVTT only for now
        {    AutoLock syncLock(mParser);
            if (/*mCurrentRunID != null && */runID != mCurrentRunID) {
                Slogger::E(TAG, "Run #%lld in progress.  Cannot process run #%lld", mCurrentRunID, runID);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            mCurrentRunID = runID;
            mParser->Parse(str);
            if (eos) {
                SubtitleTrack::FinishedRun(runID);
                mParser->Eos();
                mRegions.Clear();
                // mCurrentRunID = null;
            }
        }
    // } catch (java.io.UnsupportedEncodingException e) {
        // Log.w(TAG, "subtitle data is not UTF-8 encoded: " + e);
    // }
        return NOERROR;
}

ECode WebVttTrack::OnCueParsed(
    /* [in] */ ITextTrackCue* cue)
{
    {    AutoLock syncLock(mParser);
        String regionId;
        cue->GetRegionId(&regionId);
        // resolve region
        if (regionId.GetLength() != 0) {
            AutoPtr<ITextTrackRegion> region = mRegions[regionId];
            cue->SetRegion(region.Get());
        }

        if (DEBUG) Slogger::V(TAG, "adding cue %p", cue);

        // tokenize text track string-lines into lines of spans
        mTokenizer->Reset();
        AutoPtr<ArrayOf<ICharSequence*> > strings;
        cue->GetStrings((ArrayOf<ICharSequence*>**)&strings);
        Int32 len = strings->GetLength();
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<ICharSequence> cs = (*strings)[i];
            String s;
            cs->ToString(&s);
            mTokenizer->Tokenize(s);
        }
        AutoPtr<ArrayOf<IArrayOf*> > lines;
        mExtractor->GetText((ArrayOf<IArrayOf*>**)&lines);
        cue->SetLines(lines);
        AutoPtr<IStringBuilder> sbr;
        AutoPtr<IStringBuilder> osb;
        cue->AppendStringsToBuilder(sbr, (IStringBuilder**)&osb);
        osb->Append(String(" simplified to: "));
        AutoPtr<IStringBuilder> fosb;
        cue->AppendLinesToBuilder(osb, (IStringBuilder**)&fosb);
        String string;
        ICharSequence::Probe(fosb)->ToString(&string);

        if (DEBUG) Slogger::V(TAG, string.string());

        // extract inner timestamps
        AutoPtr<ArrayOf<IArrayOf*> > lines_;
        cue->GetLines((ArrayOf<IArrayOf*>**)&lines_);
        Int32 size = lines_->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IArrayOf> line = (*lines_)[i];
            Int32 length;
            line->GetLength(&length);
            for (Int32 j = 0; i < length; ++j) {
                AutoPtr<IInterface> obj;
                line->Get(j, (IInterface**)&obj);
                AutoPtr<ITextTrackCueSpan> span = ITextTrackCueSpan::Probe(obj);
                Int64 timestampMs;
                span->GetTimeStampMs(&timestampMs);
                Int64 startTimeMs;
                ISubtitleTrackCue::Probe(cue)->GetStartTimeMs(&startTimeMs);
                Int64 endTimeMs;
                ISubtitleTrackCue::Probe(cue)->GetEndTimeMs(&endTimeMs);
                AutoPtr<IInteger64> tsms;
                CoreUtils::Convert(timestampMs);
                Boolean flag = FALSE;
                mTimestamps->Contains(tsms.Get(), &flag);
                if(timestampMs > startTimeMs &&
                    timestampMs < endTimeMs && !flag) {
                    mTimestamps->Add(tsms.Get());
                }
            }
        }

        mTimestamps->GetSize(&size);
        if (size > 0) {
            AutoPtr<ArrayOf<Int64> > innerTimesMs = ArrayOf<Int64>::Alloc(size);
            for (Int32 ix=0; ix < size; ++ix) {
                AutoPtr<IInterface> obj;
                mTimestamps->Get(ix, (IInterface**)&obj);
                AutoPtr<IInteger64> obj_ = IInteger64::Probe(obj);
                Int64 value;
                obj_->GetValue(&value);
                innerTimesMs->Set(ix, value);
                ISubtitleTrackCue::Probe(cue)->SetInnerTimesMs(innerTimesMs);
            }
            mTimestamps->Clear();
        } else {
            ISubtitleTrackCue::Probe(cue)->SetInnerTimesMs(NULL);
        }

        ISubtitleTrackCue::Probe(cue)->SetRunID(mCurrentRunID);
    }

    Boolean ret = FALSE;
    return SubtitleTrack::AddCue(ISubtitleTrackCue::Probe(cue), &ret);
}

ECode WebVttTrack::OnRegionParsed(
    /* [in] */ ITextTrackRegion* region)
{
    String id;
    region->GetId(&id);
    {    AutoLock syncLock(mParser);
        mRegions[id] = region;
    }
    return NOERROR;
}

ECode WebVttTrack::UpdateView(
    /* [in] */ IVector* activeCues)
{
    if (!SubtitleTrack::mVisible) {
        // don't keep the state if we are not visible
        return NOERROR;
    }

    if (SubtitleTrack::DEBUG && SubtitleTrack::mTimeProvider != NULL) {
        // try {
        ECode ec = NOERROR;
        Int64 v;
        ec = mTimeProvider->GetCurrentTimeUs(FALSE, TRUE, &v);
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
             Slogger::D(TAG, "at (illegal state) the active cues are:");
             return NOERROR;
        }
        Int64 v_ = v / 1000;
        Slogger::D(TAG, "at %lld ms the active cues are:", v_);
        // } catch (IllegalStateException e) {
            // Log.d(TAG, "at (illegal state) the active cues are:");
        // }

    }

    if (mRenderingWidget != NULL) {
        return mRenderingWidget->SetActiveCues(activeCues);
    }

    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos