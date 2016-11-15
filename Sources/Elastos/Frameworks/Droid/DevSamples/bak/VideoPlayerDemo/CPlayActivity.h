
#ifndef __CPLAYERACTIVITY_H__
#define __CPLAYERACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "_CPlayActivity.h"
#include <elautoptr.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IVideoView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VideoPlayerDemo {

class CPlayActivity : public Activity
{
public:
    class PlayListener
        : public ElRefBase
        , public IViewOnClickListener
        , public IMediaPlayerOnCompletionListener
    {
    public:
        PlayListener(
            /* [in] */ CPlayActivity* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnCompletion(
            /* [in] */ IMediaPlayer* mp);

    private:
        CPlayActivity* mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    friend class MyListener;

    AutoPtr<IVideoView> mVideoView;
};

} // namespace VideoPlayerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CPLAYERACTIVITY_H__
