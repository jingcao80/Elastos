
#include "CMainActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::CSoundPool;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PlaySoundPoolDemo {

CAR_INTERFACE_IMPL(CMainActivity::PlayClickListener, IViewOnClickListener)

CAR_INTERFACE_IMPL(CMainActivity::StopClickListener, IViewOnClickListener)

ECode CMainActivity::PlayClickListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::D("CMainActivity::RecorderClickListener", "OnClick");
    return mHost->PlaySounds(1,1);
}

ECode CMainActivity::StopClickListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::D("CMainActivity::StopClickListener", "OnClick, mSoundPool = %p",mSoundPool);

    mSoundPool->Pause((*mSpMap)[1]);
    return NOERROR;
}

ECode CMainActivity::PlaySounds(
    /* [in] */ Int32 sound,
    /* [in] */ Int32 number)
{
    AutoPtr<IInterface> server;
    GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&server);
    AutoPtr<IAudioManager> am = IAudioManager::Probe(server);

    Int32 audioMaxVolumn, audioCurrentVolumn;
    am->GetStreamMaxVolume(IAudioManager::STREAM_MUSIC, &audioMaxVolumn);
    am->GetStreamVolume(IAudioManager::STREAM_MUSIC, &audioCurrentVolumn);
    Float volumnRatio = (Float)audioCurrentVolumn/(Float)audioMaxVolumn;

    Logger::D("CMainActivity::PlaySounds", "audioCurrentVolumn = %d, audioMaxVolumn = %d volumnRatio = %f",audioCurrentVolumn,audioMaxVolumn,volumnRatio);
    Int32 result;
    mSoundPool->Play(mSoundPoolMap[sound], volumnRatio, volumnRatio, 1, number, 1, &result);
}

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D("CMainActivity", "OnCreate");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view;
    view = FindViewById(R::id::button01);
    assert(view != NULL);
    mPlayButton = IButton::Probe(view);
    assert(mPlayButton != NULL);

    view = NULL;
    view = FindViewById(R::id::button02);
    assert(view != NULL);
    mStopButton = IButton::Probe(view);
    assert(mStopButton != NULL);

    CSoundPool::New(2, IAudioManager::STREAM_MUSIC, 0, (ISoundPool**)&mSoundPool);
    Int32 sound;
    Logger::D("CMainActivity::OnCreate", "before LoadEx");
    mSoundPool->Load(this, R::raw::supermario, 1, &sound);
    Logger::D("CMainActivity::OnCreate", "after LoadEx");
    mSoundPoolMap[1] = sound;

    AutoPtr<PlayClickListener> pl = new PlayClickListener(this);
    AutoPtr<StopClickListener> sl = new StopClickListener(this, mSoundPool, &mSoundPoolMap);
    mPlayButton->SetOnClickListener(pl);
    mStopButton->SetOnClickListener(sl);

    return NOERROR;
}

ECode CMainActivity::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnRestart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}


} // namespace PlaySoundPoolDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

