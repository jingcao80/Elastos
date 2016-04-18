
#ifndef __Elastos_DevSamples_ServiceDemo_CActivityOne_H__
#define __Elastos_DevSamples_ServiceDemo_CActivityOne_H__

#include <elastos/droid/app/Activity.h>
#include <Elastos.Droid.Widget.h>
#include "_Elastos_DevSamples_ServiceDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class ButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ButtonListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI UpdateTextView(
        /* [in] */ const String& timeStr);

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

    CARAPI ClickStartService();

    CARAPI ClickBindService();

    CARAPI ClickUnbindService();

    CARAPI ClickStopService();

private:
    AutoPtr<IServiceConnection> mServiceConnection;
    AutoPtr<ITextView> mTextView;
};

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ServiceDemo_CActivityOne_H__
