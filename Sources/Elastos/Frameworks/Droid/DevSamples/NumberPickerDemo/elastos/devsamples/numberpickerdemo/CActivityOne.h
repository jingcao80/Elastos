
#ifndef __Elastos_DevSamples_NumberPickerDemo_CActivityOne_H__
#define __Elastos_DevSamples_NumberPickerDemo_CActivityOne_H__

#include <Elastos.Droid.Widget.h>
#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_NumberPickerDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::INumberPicker;

namespace Elastos {
namespace DevSamples {
namespace NumberPickerDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

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
    AutoPtr<INumberPicker> mNumber;
};

} // namespace NumberPickerDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_NumberPickerDemo_CActivityOne_H__
