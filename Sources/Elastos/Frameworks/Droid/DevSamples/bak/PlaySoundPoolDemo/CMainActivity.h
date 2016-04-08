
#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "_CMainActivity.h"
#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Media::ISoundPool;
using Elastos::Droid::Widget::IButton;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PlaySoundPoolDemo {

class CMainActivity
    : public Activity
{
private:
    class PlayClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        PlayClickListener(
            /* [in] */  CMainActivity* host)
             : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
    };

    class StopClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        StopClickListener(
            /* [in] */  CMainActivity* host,
            /* [in] */  ISoundPool* sp,
            /* [in] */  HashMap<Int32, Int32>* spMap)
             : mHost(host)
             , mSoundPool(sp)
             , mSpMap(spMap)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
        HashMap<Int32, Int32>* mSpMap;
        ISoundPool* mSoundPool;
    };

public:
    CARAPI PlaySounds(
        /* [in] */ Int32 sound,
        /* [in] */ Int32 number);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IButton> mPlayButton;
    AutoPtr<IButton> mStopButton;
    AutoPtr<ISoundPool> mSoundPool;
    HashMap<Int32, Int32> mSoundPoolMap;
};

} // namespace PlaySoundPoolDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CMAINACTIVITY_H__

