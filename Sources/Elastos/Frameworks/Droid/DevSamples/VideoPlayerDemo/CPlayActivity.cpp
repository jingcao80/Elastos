
#include "CPlayActivity.h"
#include <unistd.h>
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::Widget::IMediaController;
using Elastos::Droid::Widget::IMediaPlayerControl;
using Elastos::Droid::Widget::CMediaController;
using Elastos::Droid::Provider::IMediaStoreVideoVideoColumns;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VideoPlayerDemo {

CPlayActivity::PlayListener::PlayListener(
    /* [in] */ CPlayActivity* host)
    : mHost(host)
{}

PInterface CPlayActivity::PlayListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnTouchListener) {
        return this;
    }
    else if (riid == EIID_IViewOnKeyListener) {
        return this;
    }
    else if (riid == EIID_IMediaPlayerOnCompletionListener) {
        returnthis;
    }

    return NULL;
}

UInt32 CPlayActivity::PlayListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CPlayActivity::PlayListener::Release()
{
    return ElRefBase::Release();
}

ECode CPlayActivity::PlayListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IRunnable;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_IMediaPlayerOnCompletionListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CPlayActivity::PlayListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean b;
    IMediaPlayerControl::Probe(mHost->mVideoView)->IsPlaying(&b);
    Logger::D("CPlayActivity", "OnClick(), isPlaying = %d", b);
    if(b) {
        IMediaPlayerControl::Probe(mHost->mVideoView)->Pause();
    } else {
        IMediaPlayerControl::Probe(mHost->mVideoView)->Start();
    }

    return NOERROR;
}

ECode CPlayActivity::PlayListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    mHost->Finish();
    return NOERROR;
}

ECode CPlayActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D("CPlayActivity", "OnCreate()");

    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::main);

    AutoPtr<IView> view;
    view = FindViewById(R::id::videoView1);
    assert(view != NULL);
    mVideoView = IVideoView::Probe(view);

    String videoPath;
    AutoPtr<IIntent> startedIntent;
    GetIntent((IIntent**)&startedIntent);
    if (startedIntent != NULL) {
        Int32 position;
        startedIntent->GetInt32Extra(String("position"), -1, &position);
        Logger::D("CPlayActivity", " Media position = %d", position);

        if(position != -1) {
            AutoPtr<IContentResolver> cr;
            GetContentResolver((IContentResolver**)&cr);
            AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(2);
            projections->Set(0, IMediaStoreVideoVideoColumns::ID);
            projections->Set(1, IMediaStoreVideoVideoColumns::DATA);
            AutoPtr<ICursor> c;
            AutoPtr<IUri> uri;
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            helper->Parse(String("content://media/external/video/media"), (IUri**)&uri);

            cr->Query(uri, projections, String(NULL), NULL, String(NULL), (ICursor**)&c);
            Boolean b;
            AutoPtr<ArrayOf<IInterface*> > array;
            if (c != NULL) {
                Int32 count;
                c->GetCount(&count);
                for(Int32 i = 0; i < position + 1; i++) {
                    c->MoveToNext(&b);
                    if(!b) break;
                }
                if(b) {
                    c->GetString(1, &videoPath);
                }
            }
        }
    }

    Logger::D("CPlayActivity", " Media videoPath = %s", videoPath.string());
    mVideoView->SetVideoPath(videoPath);

    AutoPtr<IMediaController> mc;
    CMediaController::New(this, (IMediaController**)&mc);
    mVideoView->SetMediaController(mc);
    mc->SetMediaPlayer(IMediaPlayerControl::Probe(mVideoView));
    Boolean rst;
    mVideoView->RequestFocus(&rst);

    AutoPtr<PlayListener> l = new PlayListener(this);
    mVideoView->SetOnClickListener(l);
    mVideoView->SetOnCompletionListener(l);
    IMediaPlayerControl::Probe(mVideoView)->Start();
    Logger::D("CPlayActivity", "Start, mVideoView = %p", mVideoView.Get());
    return NOERROR;
}

ECode CPlayActivity::OnStart()
{
    Logger::D("CPlayActivity", "OnStart(), mVideoView = %p", mVideoView.Get());
    Boolean b;
    if(IMediaPlayerControl::Probe(mVideoView) == NULL) {
        Logger::D("CPlayActivity", "OnStart(), IMediaPlayerControl::Probe(mVideoView) == NULL");
        return NOERROR;
    }
    // IMediaPlayerControl::Probe(mVideoView)->IsPlaying(&b);
    // Logger::D("CPlayActivity", "OnStart(), isPlaying = %d", b);
    // if(b) {
        Logger::D("CPlayActivity", "OnStart(), To Pause");
        IMediaPlayerControl::Probe(mVideoView)->Pause();
        Logger::D("CPlayActivity", "OnStart(), To Start");
        IMediaPlayerControl::Probe(mVideoView)->Start();
    // }
    Boolean b1, b2, b3;
    IMediaPlayerControl::Probe(mVideoView)->CanPause(&b1);
    IMediaPlayerControl::Probe(mVideoView)->CanSeekForward(&b2);
    IMediaPlayerControl::Probe(mVideoView)->CanSeekBackward(&b3);
    Logger::D("CPlayActivity", "OnStart(), CanPause = %d, CanSeekForward = %d, CanSeekBackward = %d", b1, b2, b3);

    // TODO: Add your code here
    return NOERROR;
}

ECode CPlayActivity::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CPlayActivity::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CPlayActivity::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CPlayActivity::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CPlayActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace VideoPlayerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
