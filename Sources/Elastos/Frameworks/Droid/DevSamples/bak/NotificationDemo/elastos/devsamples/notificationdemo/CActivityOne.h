
#ifndef __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_Elastos_DevSamples_NotificationDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace NotificationDemo {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<CActivityOne> mHost;
        Int32 mRef;
    };

public:
    CAR_OBJECT_DECL()

    CActivityOne();

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
    CARAPI StartStatusBarNotification();

    CARAPI StartNotification();
    CARAPI UpdateNotification();
    CARAPI CancelNotification();

    CARAPI NotifyNotification(
        /* [in] */ Boolean isUpdate);

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI SendNotification(
        /* [in] */ const String& title,
        /* [in] */ const String& message);

private:
    friend class MyListener;

    static const String TAG;
    Int32 mNotificationID;
    Int32 mNumMessages;
};

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_NotificationDemo_CACTIVITYONE_H__
