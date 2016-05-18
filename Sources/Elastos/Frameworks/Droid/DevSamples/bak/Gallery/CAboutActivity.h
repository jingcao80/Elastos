
#ifndef __CABOUTACTIVITY_H__
#define __CABOUTACTIVITY_H__

#include "_CAboutActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

namespace Gallery {

class CAboutActivity
    : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CAboutActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CAboutActivity* mHost;
    };

friend class MyListener;

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    static const String TAG;

private:
    AutoPtr<IImageView> mBackButton;
};

} // namespace Gallery

#endif // __CABOUTACTIVITY_H__
