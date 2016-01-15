#ifndef __CSECONDBARACTIVITY_H__
#define __CSECONDBARACTIVITY_H__

#include "app/Activity.h"
#include "_CSecondBarActivity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Actionbar {

class CSecondBarActivity : public Activity
{
public:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

private:
    static const String TAG;
};

}//end namespace Actionbar
}//end namespace DevSamples
}//end namespace Droid
}//end namespace Elastos

#endif // __CSECONDBARACTIVITY_H__
