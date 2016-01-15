
#include "CVideoPlayer.h"
#include <elastos/coredef.h>
#include <Elastos.Core.h>
#include <stdio.h>

using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Net::IURLConnection;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IInputStreamReader;
using Elastos::Droid::Widget::IMediaController;
using Elastos::Droid::Widget::IMediaPlayerControl;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

ECode CVideoPlayer::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(2130903046);//R.layout.videoplayer
    mVideoView = IVideoView::Probe(FindViewById(0x7f07000a));//R::Id::VideoView
    AutoPtr<IMediaController> mediaController;
//    FAIL_RETURN(CMediaController::New(this, (IMediaController**)&mediaController));
    mediaController->SetAnchorView(mVideoView);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(GetIntent((IIntent**)&intent));
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data == NULL)
        return NOERROR;
    String spec;
    String scheme;
    data->GetScheme(&scheme) ;
    if (String("vnd.youtube").Equals(scheme)) {
        String ssp;
        data->GetSchemeSpecificPart(&ssp);
        String id = ssp.Substring(0, ssp.IndexOf('?'));
        FAIL_RETURN(GetSpecFromYouTubeVideoID(id, &spec));
    }
    if (spec == NULL)
        return NOERROR;
    AutoPtr<IUri> video;
//    FAIL_RETURN(Uri::Parse(spec, (IUri**)&video));
    FAIL_RETURN(mVideoView->SetMediaController(mediaController));
    FAIL_RETURN(mVideoView->SetVideoURI(video));
    FAIL_RETURN(IMediaPlayerControl::Probe(mVideoView)->Start());

    return NOERROR;
}


ECode CVideoPlayer::GetSpecFromYouTubeVideoID(
    /* [in] */ const String& id,
    /* [out] */ String* spec)
{
    ECode ec;
    AutoPtr<IInputStream> stream;
    AutoPtr<IInputStreamReader> streamReader;
    AutoPtr<IBufferedReader> br;
    StringBuf_<6144> sb;
    String line;
    AutoPtr<IUri> fakeUri;
    String stream_map;
    StringTokenizer* streams = NULL;

    String info_uri("http://www.youtube.com/get_video_info?&video_id=");
    info_uri += id;
    AutoPtr<IURL> info_url;
    ASSERT_SUCCEEDED(CURL::New(info_uri, (IURL**)&info_url));
    AutoPtr<IURLConnection> urlConnection;
    ec = info_url->OpenConnection((IURLConnection**)&urlConnection);
    if (FAILED(ec)) { goto exception;}

    ec = urlConnection->GetInputStream((IInputStream**)&streamReader);
    if (FAILED(ec)) { goto exception;}

    ASSERT_SUCCEEDED(CInputStreamReader::New(stream, (IInputStreamReader**)&streamReader));

    ASSERT_SUCCEEDED(CBufferedReader::New(streamReader,(IBufferedReader**)&br));

    br->ReadLine(&line);
    while (line != NULL)
    {
        sb.Append(line);
        ec = br->ReadLine(&line);
        if (FAILED(ec)) { goto exception;}
    }

//    ec = Uri::Parse(String("fake:/fake?") + sb.GetPayload(), (IUri**)&fakeUri);
//    if (FAILED(ec)) { goto exception;}

    assert(0);
/*    ec = fakeUri->GetQueryParameter("url_encoded_fmt_stream_map", &stream_map);
    if (FAILED(ec)) { goto exception;}*/

    if (stream_map == NULL)
    {
        *spec = NULL;
        return NOERROR;
    }

    streams = new StringTokenizer(stream_map, ",");

    while (streams->HasMoreTokens()) {
        CString st = streams->NextToken();
//        ec = Uri::Parse(String("fake:/fake?") + st, (IUri**)&fakeUri);
//        if (FAILED(ec)) { goto exception;}
        String url;
        assert(0);
/*        ec = fakeUri->GetQueryParameter("url", &url);
        if (FAILED(ec)) { goto exception;}*/

        String type;
        assert(0);
/*        ec = fakeUri->GetQueryParameter("type", &type);
        if (FAILED(ec)) { goto exception;}*/

        if (type != NULL && url != NULL &&
            (type.StartWith("video/mp4") || type.StartWith("video/webm"))) {
            *spec = url;
        }
    }

    delete streams;
    return NOERROR;

exception:
    printf("VideoPlayer"  "exception");
    delete streams;
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
