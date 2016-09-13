
#ifndef __Elastos_DevSamples_ImageDemo_CActivityOne_H__
#define __Elastos_DevSamples_ImageDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include <elastos/droid/view/OrientationEventListener.h>
#include "_Elastos_DevSamples_ImageDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::OrientationEventListener;

namespace Elastos {
namespace DevSamples {
namespace ImageDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class OrientationListener
        : public OrientationEventListener
    {
    public:
        OrientationListener();

        ~OrientationListener();

        CARAPI OnOrientationChanged(
            /* [in] */ Int32 orientation);
    };

    class MyListener
        : public Object
        , public IViewOnClickListener
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

public:
    CAR_OBJECT_DECL()

    CActivityOne();

    ~CActivityOne();

    UInt32 AddRef();

    UInt32 Release();

    CARAPI constructor();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

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
    Boolean mDestroyed;
    AutoPtr<OrientationListener> mOrientationListener;
};

} // namespace ImageDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ImageDemo_CActivityOne_H__
