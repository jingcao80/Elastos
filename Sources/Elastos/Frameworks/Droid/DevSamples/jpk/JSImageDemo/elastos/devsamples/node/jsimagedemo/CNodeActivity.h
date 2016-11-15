#ifndef __CNodeActivity_H__
#define __CNodeActivity_H__

#include "CNodeListener.h"

#define JSActCarClassHead EE(JSPkgName,JSActName)
#define JSActNameStr FF(JSActName)

#include JSActCarClassHead

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
namespace JSPkgName {

JSCarClass(JSActName)
    , public Activity
    , public IActivityOne
{
public:
    class MyHandler : public Handler
    {
    public:
        JS_TO_STRING_IMPL(JSActName,MyHandler)

        MyHandler(
            /* [in] */ JSActName* host)
            : mHost(host)
        {};

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        JSActName* mHost;
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

private:
    static const String TAG;

    String mPackageName;
    String mActivityName;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IActivityListener> mListener;
};

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos

#endif // __H__
