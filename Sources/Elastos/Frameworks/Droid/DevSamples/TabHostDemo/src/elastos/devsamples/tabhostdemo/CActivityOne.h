
#ifndef __Elastos_DevSamples_TabHostDemo_CActivityOne_H__
#define __Elastos_DevSamples_TabHostDemo_CActivityOne_H__

#include <elastos/droid/app/TabActivity.h>
#include <Elastos.Droid.Widget.h>
#include "_Elastos_DevSamples_TabHostDemo_CActivityOne.h"

using Elastos::Droid::App::TabActivity;
using Elastos::Droid::Widget::ITabHostOnTabChangeListener;

namespace Elastos {
namespace DevSamples {
namespace TabHostDemo {

CarClass(CActivityOne)
    , public TabActivity
{
public:
    class MyListener
        : public Object
        , public ITabHostOnTabChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnTabChanged(
            /* [in] */ const String& tabId);

    private:
        CActivityOne* mHost;
    };

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
};

} // namespace TabHostDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_TabHostDemo_CActivityOne_H__
