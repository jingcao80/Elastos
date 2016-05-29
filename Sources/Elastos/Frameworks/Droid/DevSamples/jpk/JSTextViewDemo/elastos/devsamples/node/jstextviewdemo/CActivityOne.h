
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#define JSAppName JSTextViewDemo

#include "_Elastos_DevSamples_Node_JSTextViewDemo_CActivityOne.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

using Elastos::Droid::Os::Handler;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace DevSamples {
namespace Node {
//namespace JSTextViewDemo {
namespace JSAppName {

CarClass(CActivityOne)
    , public Activity
    , public IActivityOne
{
public:
    class MyHandler : public Handler
    {
    public:
        TO_STRING_IMPL("CActivityOne::MyHandler")

        MyHandler(
            /* [in] */ CActivityOne* host)
            : mHost(host)
        {};

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<CActivityOne> mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI OnClickPopupWindow(
        /* [in] */ IView* v);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
    /* [in] */ Int32 id);

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI Require(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** object);

private:
    static const String TAG;

    String mPackageName;
    String mActivityName;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IActivityListener> mListener;
};

} // namespace JSTextViewDemo
} // namespace Node
} // namespace DevSamples
} // namespace Elastos

#endif // __CACTIVITYONE_H__
