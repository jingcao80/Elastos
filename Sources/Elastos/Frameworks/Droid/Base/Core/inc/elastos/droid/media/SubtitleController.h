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

#ifndef __ELASTOS_DROID_MEDIA_SUBTITLECONTROLLER_H__
#define __ELASTOS_DROID_MEDIA_SUBTITLECONTROLLER_H__

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::Accessibility::ICaptioningManager;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptioningChangeListener;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyle;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

class SubtitleController
    : public Object
    , public ISubtitleController
{
private:
    class MyHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ SubtitleController* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        SubtitleController* mHost;
    };

    class MyCaptioningChangeListener
        : public Object
        , public ICaptioningManagerCaptioningChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyCaptioningChangeListener(
            /* [in] */ SubtitleController* host)
            : mHost(host)
        {}

        CARAPI OnEnabledChanged(
            /* [in] */ Boolean enabled);

        CARAPI OnLocaleChanged(
            /* [in] */ ILocale* locale);

        CARAPI OnUserStyleChanged(
            /* [in] */ ICaptioningManagerCaptionStyle* userStyle);

        CARAPI OnFontScaleChanged(
            /* [in] */ Float fontScale);

    private:
        SubtitleController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SubtitleController();

    virtual ~SubtitleController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMediaTimeProvider* timeProvider,
        /* [in] */ ISubtitleControllerListener* listener);

    CARAPI GetTracks(
        /* [out, callee] */ ArrayOf<ISubtitleTrack*>** result);

    CARAPI GetSelectedTrack(
        /* [out] */ ISubtitleTrack** result);

    CARAPI SelectTrack(
        /* [in] */ ISubtitleTrack* track,
        /* [out] */ Boolean* result);

    CARAPI GetDefaultTrack(
        /* [out] */ ISubtitleTrack** result);

    CARAPI SelectDefaultTrack();

    CARAPI Reset();

    CARAPI AddTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ ISubtitleTrack** result);

    CARAPI Show();

    CARAPI Hide();

    CARAPI RegisterRenderer(
        /* [in] */ ISubtitleControllerRenderer* renderer);

    CARAPI HasRendererFor(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    CARAPI SetAnchor(
        /* [in] */ ISubtitleControllerAnchor* anchor);

protected:
    CARAPI Finalize();

private:
    CARAPI GetRenderingWidget(
        /* [out] */ ISubtitleTrackRenderingWidget** result);

    CARAPI_(void) DoSelectTrack(
        /* [in] */ ISubtitleTrack* track);

    CARAPI_(void) DoSelectDefaultTrack();

    CARAPI_(void) DoShow();

    CARAPI_(void) DoHide();

    CARAPI_(void) CheckAnchorLooper();

    CARAPI_(void) ProcessOnAnchor(
        /* [in] */ IMessage* m);

private:
    AutoPtr<IMediaTimeProvider> mTimeProvider;
    AutoPtr<IVector> mRenderers;
    AutoPtr<IVector> mTracks;
    AutoPtr<ISubtitleTrack> mSelectedTrack;
    Boolean mShowing;
    AutoPtr<ICaptioningManager> mCaptioningManager;
    AutoPtr<IHandler> mHandler;
    Boolean mTrackIsExplicit;
    Boolean mVisibilityIsExplicit;
    AutoPtr<ISubtitleControllerAnchor> mAnchor;
    AutoPtr<ISubtitleControllerListener> mListener;
    AutoPtr<IHandlerCallback> mCallback;
    AutoPtr<ICaptioningManagerCaptioningChangeListener> mCaptioningChangeListener;

    static const Int32 WHAT_SHOW;
    static const Int32 WHAT_HIDE;
    static const Int32 WHAT_SELECT_TRACK;
    static const Int32 WHAT_SELECT_DEFAULT_TRACK;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_SUBTITLECONTROLLER_H__