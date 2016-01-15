#ifndef __CACTIONBARACTIVITY_H__
#define __CACTIONBARACTIVITY_H__

#include "app/Activity.h"
#include "_CActionBarActivity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Actionbar {

class CActionBarActivity : public Activity
{
public:
	class MenuItemClickListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        MenuItemClickListener(
            /* [in] */ CActionBarActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActionBarActivity* mHost;
    };

public:
    CActionBarActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

    CARAPI GoSecondActivity(
    	/* [in] */ IView* view);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnPause();

    CARAPI OnResume();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<MenuItemClickListener> mItemClickListener;
    static const String TAG;
};

}//end namespace Actionbar
}//end namespace DevSamples
}//end namespace Droid
}//end namespace Elastos

#endif // __CACTIONBARACTIVITY_H__
