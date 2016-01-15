#ifndef __ELASTOS_DROID_MEDIA_WEBVTTTRACK_H__
#define __ELASTOS_DROID_MEDIA_WEBVTTTRACK_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/SubtitleTrack.h"
#include "elastos/droid/media/Tokenizer.h"
#include "elastos/droid/media/UnstyledTextExtractor.h"
#include "elastos/droid/media/WebVttParser.h"
#include "elastos/droid/media/WebVttRenderingWidget.h"
#include <elastos/core/Object.h>
#include <elastos/utility/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

class WebVttTrack
    : public SubtitleTrack
    , public IWebVttCueListener
{
public:
    WebVttTrack();

    virtual ~WebVttTrack();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IWebVttRenderingWidget* renderingWidget,
        /* [in] */ IMediaFormat* format);

    CARAPI GetRenderingWidget(
        /* [out] */ ISubtitleTrackRenderingWidget** result);

    CARAPI OnData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean eos,
        /* [in] */ Int64 runID);

    CARAPI OnCueParsed(
        /* [in] */ ITextTrackCue* cue);

    CARAPI OnRegionParsed(
        /* [in] */ ITextTrackRegion* region);

    CARAPI UpdateView(
        /* [in] */ IVector* activeCues);

private:
    static const String TAG;
    AutoPtr<WebVttParser> mParser;
    AutoPtr<UnstyledTextExtractor> mExtractor;
    AutoPtr<Tokenizer> mTokenizer;
    AutoPtr<IVector> mTimestamps;
    AutoPtr<WebVttRenderingWidget> mRenderingWidget;
    HashMap<String, AutoPtr<ITextTrackRegion> > mRegions;
    Int64 mCurrentRunID;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_WEBVTTTRACK_H__