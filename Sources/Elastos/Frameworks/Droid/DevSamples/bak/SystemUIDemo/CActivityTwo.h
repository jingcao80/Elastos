
#ifndef __CACTIVITYTWO_H__
#define __CACTIVITYTWO_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityTwo.h"

using Elastos::ElRefBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUIDemo {


class CActivityTwo : public Activity
{
public:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityTwo* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityTwo* mHost;
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

    CARAPI LaunchActivityOne();

    static Int32 mActivityId;
};

} // namespace SystemUIDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYTWO_H__
