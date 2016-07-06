#ifndef __CEditActivity_H__
#define __CEditActivity_H__

#define JSPkgName JSGallery
#define JSActName CEditActivity

#ifndef JSCarClassHead

#define AA(y) #y
#define BB(x,y) AA(x##y.h)
#define CC(x,y,z) BB(x##z,y)
#define DD(x,y,z) CC(z##x,y,_)
#define EE(x,y) DD(x,y,_Elastos_DevSamples_Node_)
#define FF(x) AA(x)

#define JSCarClassHead EE(JSPkgName,JSActName)

#define JSPkgNameStr FF(JSPkgName)
#define JSActNameStr FF(JSActName)

#define JSCarClass(x) CarClass(x)

#define JS_TO_STRING_IMPL_0(x) TO_STRING_IMPL(#x)
#define JS_TO_STRING_IMPL(x,y) JS_TO_STRING_IMPL_0(x::y)

#define JS_CAR_INTERFACE_IMPL(x,y,z) CAR_INTERFACE_IMPL(x,y,z)
#define JS_CAR_OBJECT_IMPL(x) CAR_OBJECT_IMPL(x)

#endif

#include JSCarClassHead

#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;

using Elastos::Droid::Os::Handler;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::EIID_IHandler;
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
    class MyHandler
        : public Handler
    {
    public:
        JS_TO_STRING_IMPL(JSActName,MyHandler)

        MyHandler(
            /* [in] */ JSActName* host)
            : mHost(host)
        {
            constructor(true);
        };

        CAR_INTERFACE_DECL()

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<JSActName> mHost;
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