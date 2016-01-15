
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TableLayoutDemo {

class CActivityOne;

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

class CActivityOne : public Activity
{
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
};

} // namespace EditTextDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
