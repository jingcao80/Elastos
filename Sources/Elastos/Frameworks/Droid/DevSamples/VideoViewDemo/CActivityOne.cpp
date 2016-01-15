
#include "CActivityOne.h"
// #include "Uri.h"
#include <stdio.h>
#include <unistd.h>
#include "R.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::Widget::IMediaController;
using Elastos::Droid::Widget::IMediaPlayerControl;
using Elastos::Droid::Widget::CMediaController;
using Elastos::Droid::Content::EIID_IContext;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VideoViewDemo {

CActivityOne::PlayListener::PlayListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

PInterface CActivityOne::PlayListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnTouchListener) {
        return (IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnKeyListener) {
        return (IViewOnKeyListener*)this;
    }
    else if (riid == EIID_IMediaPlayerOnCompletionListener) {
        return(IMediaPlayerOnCompletionListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::PlayListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::PlayListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::PlayListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
        *pIID = EIID_IRunnable;
    }
    else if (pObject == (IInterface*)(IMediaPlayerOnCompletionListener*)this) {
        *pIID = EIID_IMediaPlayerOnCompletionListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::PlayListener::OnClick(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CActivityOne::PlayListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
printf("FILE=[%s], FUNC=[%s], LINE=[%d]\n", __FILE__, __FUNCTION__, __LINE__);
    mHost->Finish();
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    // gid_t groups0[2];
    // groups0[0] = 1015;
    // groups0[1] = 3003;
    // setgroups(2, groups0);

    // int ngroups = getgroups(0, NULL);
    // gid_t* groups = (gid_t*)malloc(ngroups * sizeof(gid_t));
    // int val = getgroups(ngroups, groups);
    // if (val >= 0) {
    //     for (int i = 0; i < ngroups; ++i) {
    //         printf("group: %d\n", groups[i]);
    //     }
    // }

    // GetWindow()->SetFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN,
    //            IWindowManagerLayoutParams::FLAG_FULLSCREEN);

    SetContentView(R::layout::main);

    AutoPtr<IView> view;
    view = FindViewById(R::id::videoView1);
    assert(view != NULL);
    mVideoView = IVideoView::Probe(view);

    String videoPath = String("/data/data/com.elastos.runtime/elastos/VideoViewDemo/data/Transformer3.flv");
    //String videoPath = String("/sdcard/rainbow.mp4");

    // AutoPtr<IIntent> startedIntent;
    // GetIntent((IIntent**)&startedIntent);
    // if (startedIntent != NULL) {
    //     String tmpStr;
    //     startedIntent->GetStringExtra(String(IIntent::EXTRA_STREAM), &tmpStr);
    //     if (!tmpStr.IsNull()) {
    //         videoPath = tmpStr;
    //     }
    // }

    /***
     * 将播放器关联上一个音频或者视频文件
     * videoView.setVideoURI(Uri uri)
     * videoView.setVideoPath(String path)
     * 以上两个方法都可以。
     */

    mVideoView->SetVideoPath(videoPath);
    /**
     * w为其提供一个控制器，控制其暂停、播放……等功能
     */

    AutoPtr<IMediaController> mc;
    CMediaController::New((IContext*)this->Probe(EIID_IContext), (IMediaController**)&mc);
    mVideoView->SetMediaController(mc);
    mc->SetMediaPlayer(IMediaPlayerControl::Probe(mVideoView));
    Boolean rst;
    mVideoView->RequestFocus(&rst);
    /**
     * 视频或者音频到结尾时触发的方法
     */
    // videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
    //     @Override
    //     public void onCompletion(MediaPlayer mp) {
    //         Log.i("通知", "完成");
    //     }
    // });

    // videoView.setOnErrorListener(new MediaPlayer.OnErrorListener() {

    //     @Override
    //     public boolean onError(MediaPlayer mp, int what, int extra) {
    //         Log.i("通知", "播放中出现错误");
    //         return false;
    //     }
    // });
    IMediaPlayerControl::Probe(mVideoView)->Start();
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace VideoViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
