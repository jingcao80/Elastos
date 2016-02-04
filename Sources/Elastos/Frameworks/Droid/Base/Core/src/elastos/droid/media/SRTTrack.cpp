#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/SRTTrack.h"
#include "elastos/droid/media/CTextTrackCue.h"
#include "elastos/droid/media/CTextTrackCueSpan.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::IMessage;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::Charset::CCharsets;
using Elastos::IO::Charset::ICharsets;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::Utility::CArrayList;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Droid {
namespace Media {

const Int32 SRTTrack::MEDIA_TIMED_TEXT = 99;   // MediaPlayer.MEDIA_TIMED_TEXT
const Int32 SRTTrack::KEY_STRUCT_TEXT = 16;    // TimedText.KEY_STRUCT_TEXT
const Int32 SRTTrack::KEY_START_TIME = 7;      // TimedText.KEY_START_TIME
const Int32 SRTTrack::KEY_LOCAL_SETTING = 102; // TimedText.KEY_START_TIME
const String SRTTrack::TAG(String("SRTTrack"));

SRTTrack::SRTTrack()
{}

SRTTrack::~SRTTrack()
{}

CAR_INTERFACE_IMPL_4(SRTTrack, Object, IWebVttCueListener, ISRTTrack, IMediaTimeProviderOnMediaTimeListener, IMediaTimeProviderOnMediaTimeListener)

ECode SRTTrack::constructor(
	/* [in] */ IWebVttRenderingWidget* renderingWidget,
	/* [in] */ IMediaFormat* format)
{
	WebVttTrack::constructor(renderingWidget, format);
	mEventHandler = NULL;
	return NOERROR;
}

ECode SRTTrack::constructor(
	/* [in] */ IHandler* eventHandler,
	/* [in] */ IMediaFormat* format)
{
	WebVttTrack::constructor(NULL, format);
    mEventHandler = eventHandler;
    return NOERROR;
}

ECode SRTTrack::OnData(
	/* [in] */ ISubtitleData* data)
{
	// try {
	VALIDATE_NOT_NULL(data);
	AutoPtr<CTextTrackCue> cue;
	CTextTrackCue::NewByFriend((CTextTrackCue**)&cue);
	Int64 startTime;
	data->GetStartTimeUs(&startTime);
	Int64 duration;
	data->GetDurationUs(&duration);
    ISubtitleTrackCue::Probe(cue)->SetStartTimeMs(startTime / 1000);
    cue->SetEndTimeMs((startTime + duration) / 1000);

    AutoPtr<ArrayOf<Byte> > data_;
    data->GetData((ArrayOf<Byte>**)&data_);
 	//todo
 	// paragraph = new String(data.getData(), "UTF-8");
    String paragraph(*data_);
    AutoPtr<ArrayOf<String> > lines;
    StringUtils::Split(paragraph, String("\\r?\\n"), (ArrayOf<String>**)&lines);
    Int32 length = lines->GetLength();
    AutoPtr<ArrayOf<ITextTrackCueSpan*> > ttc = ArrayOf<ITextTrackCueSpan*>::Alloc(length);
    AutoPtr<ArrayOf<IArrayOf*> > arrays = ArrayOf<IArrayOf*>::Alloc(length);
    AutoPtr<ITextTrackCueSpan> tspan;
    String line;
    AutoPtr<IArrayOf>  arrayof;
    for (Int32 i = 0; i < length; ++i) {
        tspan = NULL;
        line = (*lines)[i];
        CTextTrackCueSpan::New(line, -1, (ITextTrackCueSpan**)&tspan);
        ttc->Set(i, tspan);
        arrayof = NULL;
        arrayof = CoreUtils::Convert(ttc.Get());
        arrays->Set(i, arrayof);
    }
    cue->SetLines(arrays.Get());

    Boolean flag = FALSE;
    SubtitleTrack::AddCue(ISubtitleTrackCue::Probe(cue), &flag);
    // } catch (UnsupportedEncodingException e) {
        // Log.w(TAG, "subtitle data is not UTF-8 encoded: " + e);
    // }
    return NOERROR;
}

ECode SRTTrack::OnData(
	/* [in] */ ArrayOf<Byte>* data,
	/* [in] */ Boolean eos,
	/* [in] */ Int64 runID)
{
    // TODO make reentrant
    // try {
    AutoPtr<IInputStreamReader> r;
    AutoPtr<ICharsets> charSet;
    CCharsets::AcquireSingleton((ICharsets**)&charSet);
    AutoPtr<ICharset> utf8;
    charSet->GetUTF_8((ICharset**)&utf8);
    AutoPtr<IInputStream> byteInputStream;
    CByteArrayInputStream::New(data, (IInputStream**)&byteInputStream);
    CInputStreamReader::New(byteInputStream.Get(), utf8.Get(), (IInputStreamReader**)&r);
    AutoPtr<IBufferedReader> br;
    CBufferedReader::New(IReader::Probe(r), (IBufferedReader**)&br);

    String header;
    while ((br->ReadLine(&header), !header.IsNull())) {
        // discard subtitle number
        br->ReadLine(&header);
        if (header.IsNull()) {
            break;
        }

        AutoPtr<ITextTrackCue> cue;
        CTextTrackCue::New((ITextTrackCue**)&cue);
        AutoPtr<ArrayOf<String> > startEnd;
        StringUtils::Split(header, String("-->"), (ArrayOf<String>**)&startEnd);
        ISubtitleTrackCue::Probe(cue)->SetStartTimeMs(ParseMs((*startEnd)[0]));
        ISubtitleTrackCue::Probe(cue)->SetEndTimeMs(ParseMs((*startEnd)[1]));

        String s;
        AutoPtr<IArrayList> paragraph;
        CArrayList::New((IArrayList**)&paragraph);
        while (!((br->ReadLine(&s), s.IsNull()) || s.Trim().Equals(String("")))) {
            paragraph->Add(StringUtils::ParseCharSequence(s).Get());
        }

        Int32 size;
        paragraph->GetSize(&size);

        AutoPtr<ArrayOf<IInterface*> > outArray;
        paragraph->ToArray((ArrayOf<IInterface*>**)&outArray);
        AutoPtr<ArrayOf<ITextTrackCueSpan*> > ttc = ArrayOf<ITextTrackCueSpan*>::Alloc(size);
        AutoPtr<ArrayOf<IArrayOf*> > arrays = ArrayOf<IArrayOf*>::Alloc(size);
        AutoPtr<ArrayOf<ICharSequence*> > strings = ArrayOf<ICharSequence*>::Alloc(size);
        AutoPtr<ITextTrackCueSpan> tspan;
        String line;
        AutoPtr<IArrayOf>  arrayof;
        AutoPtr<IInterface> obj;
        for (Int32 i = 0; i < size; ++i) {
            tspan = NULL;
            obj = NULL;
            paragraph->Get(i, (IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&line);
            CTextTrackCueSpan::New(line, -1, (ITextTrackCueSpan**)&tspan);
            ttc->Set(i, tspan);
            arrayof = NULL;
            arrayof = CoreUtils::Convert(ttc.Get());
            arrays->Set(i, arrayof);
            strings->Set(i, ICharSequence::Probe((*outArray)[i]));
        }
        cue->SetStrings(strings.Get());
        cue->SetLines(arrays.Get());

        Boolean flag = FALSE;
        SubtitleTrack::AddCue(ISubtitleTrackCue::Probe(cue), &flag);
    }
    return NOERROR;
    // } catch (UnsupportedEncodingException e) {
    //     Log.w(TAG, "subtitle data is not UTF-8 encoded: " + e);
    // } catch (IOException ioe) {
    //     // shouldn't happen
    //     Log.e(TAG, ioe.getMessage(), ioe);
    // }
}

ECode SRTTrack::UpdateView(
	/* [in] */ IVector* activeCues)
{
    AutoPtr<ISubtitleTrackRenderingWidget> str;
    WebVttTrack::GetRenderingWidget((ISubtitleTrackRenderingWidget**)&str);

    if (str != NULL) {
        return WebVttTrack::UpdateView(activeCues);
    }

    if (mEventHandler == NULL) {
        return NOERROR;
    }

    Int32 i_ = 0;
    AutoPtr<IInterface> obj;
    Int32 size;
    activeCues->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        obj = NULL;
        activeCues->Get(i, (IInterface**)&obj);
        AutoPtr<ISubtitleTrackCue> cue = ISubtitleTrackCue::Probe(obj);
        AutoPtr<TextTrackCue> ttc = (TextTrackCue*)cue.Get();

        AutoPtr<IParcel> parcel;
        CParcel::New((IParcel**)&parcel);
        parcel->WriteInt32(KEY_LOCAL_SETTING);
        parcel->WriteInt32(KEY_START_TIME);
        Int64 startTimeMs;
        cue->GetStartTimeMs(&startTimeMs);
        parcel->WriteInt64(startTimeMs);

        parcel->WriteInt32(KEY_STRUCT_TEXT);
        StringBuilder sb;
        Int32 length = ttc->mStrings->GetLength();
        AutoPtr<ICharSequence> line;
        for (Int32 i = 0; i < length; i++) {
            line = (*(ttc->mStrings))[i];
            sb.Append(line);
            sb.Append('\n');
        }

        String str = sb.ToString();
        AutoPtr<ArrayOf<Byte> > buf = str.GetBytes();

        Int32 len = buf->GetLength();
        parcel->WriteInt32(len);
        parcel->WriteArrayOf((Handle32)buf.Get());
        AutoPtr<IMessage> msg;
        mEventHandler->ObtainMessage(MEDIA_TIMED_TEXT, i_, i_, parcel, (IMessage**)&msg);
        Boolean flag = FALSE;
        mEventHandler->SendMessage(msg, &flag);
    }

   return activeCues->Clear();
}

Int64 SRTTrack::ParseMs(
	/* [in] */ const String& in)
{
    AutoPtr<ArrayOf<String> > splits;
    StringUtils::Split(in, String(":"), (ArrayOf<String>**)&splits);
    Int64 hours = StringUtils::ParseInt64((*splits)[0].Trim());
    Int64 minutes = StringUtils::ParseInt64((*splits)[1].Trim());
    AutoPtr<ArrayOf<String> > strings;
    StringUtils::Split((*splits)[2], String(","), (ArrayOf<String>**)&strings);
    Int64 seconds = StringUtils::ParseInt64((*strings)[0].Trim());
    Int64 millies = StringUtils::ParseInt64((*strings)[1].Trim());

    return hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + millies;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media