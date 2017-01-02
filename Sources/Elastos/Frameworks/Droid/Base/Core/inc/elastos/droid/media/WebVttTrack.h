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