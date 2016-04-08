
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::ElRefBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUIDemo {


class CActivityOne : public Activity
{
public:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActivityOne* mHost;
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

    CARAPI LaunchActivityTwo();

    CARAPI CreateNavigationBar();
private:
    static Boolean mIsNavigationBarAdded;

    static Int32 mActivityId;
};

} // namespace SystemUIDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
