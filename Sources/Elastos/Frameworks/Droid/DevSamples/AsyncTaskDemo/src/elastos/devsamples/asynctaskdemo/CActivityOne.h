
#ifndef __Elastos_DevSamples_AsyncTaskDemo_CActivityOne_H__
#define __Elastos_DevSamples_AsyncTaskDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_AsyncTaskDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace DevSamples {
namespace AsyncTaskDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
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

public:
   AutoPtr<IButton> mLoadButton;
   AutoPtr<IImageView> mImageView;
};

} // namespace AsyncTaskDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_AsyncTaskDemo_CActivityOne_H__
