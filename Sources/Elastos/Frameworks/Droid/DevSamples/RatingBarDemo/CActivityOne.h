
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IOnRatingBarChangeListener;
using Elastos::Droid::Widget::IRatingBar;
using Elastos::Droid::Widget::CRatingBar;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace RatingBarDemo {


class CActivityOne : public Activity
{
public:

    class RatingBarListener
        : public ElRefBase
        , public IOnRatingBarChangeListener
    {
    public:
        RatingBarListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI OnRatingChanged(
            /* [in] */ IRatingBar* ratingBar,
            /* [in] */ Float rating,
            /* [in] */ Boolean fromUser);
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
};

} // namespace RatingBarDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
