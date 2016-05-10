
#ifndef __Elastos_DevSamples_TableLayoutDemo_CActivityOne_H__
#define __Elastos_DevSamples_TableLayoutDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_TableLayoutDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITableLayout;

namespace Elastos {
namespace DevSamples {
namespace TableLayoutDemo {

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
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    AutoPtr<ITableLayout> mLayout;
    AutoPtr<IButton> mButton1;
    AutoPtr<IButton> mButton2;
    AutoPtr<IButton> mButton3;
};

} // namespace TableLayoutDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_TableLayoutDemo_CActivityOne_H__
