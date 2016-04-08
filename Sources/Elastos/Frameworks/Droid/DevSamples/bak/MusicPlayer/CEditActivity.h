#ifndef __CEDITACTIVITY_H__
#define __CEDITACTIVITY_H__

#include "_CEditActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;

namespace MusicPlayer {

class CEditActivity: public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CEditActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEditActivity* mActivity;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);
};

} // namespace MusicPlayer

#endif // __CPlayACTIVITY_H__
