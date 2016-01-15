
#ifndef  __CTIMESERVICE_H__
#define  __CTIMESERVICE_H__

#include "_CTimeService.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Utility::ITimer;
using Elastos::Utility::ITimerTask;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {

class CTimeService: public Elastos::Droid::App::Service
{
public:
    CTimeService();

    CARAPI OnCreate();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnDestroy();

    CARAPI UpdateView();

private:
    static const Int32 mDigits[] /*= { R::drawable::digit00, R::drawable::digit01,
            R::drawable::digit02, R::drawable::digit03, R::drawable::digit04,
            R::drawable::digit05, R::drawable::digit06, R::drawable::digit07,
            R::drawable::digit08, R::drawable::digit09, }*/;

    static const Int32 mDigitViews[]/* = { R::id::img01, R::id::img02, R::id::img03,
            R::id::img04, R::id::img06, R::id::img07, R::id::img09, R::id::img10,
            R::id::img12, R::id::img13, R::id::img15, R::id::img16, R::id::img18,
            R::id::img19 }*/;

    AutoPtr<ITimer> mTimer;
    AutoPtr<ITimerTask> mTimerTask;

    friend class MyTimerTask;
};

} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  // __CTIMESERVICE_H__
