#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "app/Activity.h"
#include "_CMainActivity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace InteractB {

class CMainActivity : public Activity
{
public:
	class ButtonClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
    	ButtonClickListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
    };

public:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<ButtonClickListener> mButtonListener;
    static const String TAG;
};

} //namespace InteractB
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos

#endif // __CMAINACTIVITY_H__
