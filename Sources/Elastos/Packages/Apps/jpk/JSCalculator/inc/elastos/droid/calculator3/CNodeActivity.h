//#ifndef __CNodeActivity_H__
//#define __CNodeActivity_H__

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

// namespace Elastos {
// namespace DevSamples {
// namespace Node {
// namespace JSPkgName {

namespace Elastos {
namespace Droid {
namespace Calculator3 {

JSCarClass(JSActName)
    , public Activity
    , public ICalculator
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
        AutoPtr<JSActName> mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI _OnBackPressed();
    CARAPI OnBackPressed();

    CARAPI _OnUserInteraction();
    CARAPI OnUserInteraction();

protected:
    CARAPI _OnCreate(
        /* [in] */ IBundle* savedInstanceState);
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI _OnSaveInstanceState(
        /*[in] */ /*@NonNull */IBundle* outState);
    CARAPI OnSaveInstanceState(
        /*[in] */ /*@NonNull */IBundle* outState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    static const String TAG;

    String mPackageName;
    String mActivityName;

    AutoPtr<IHandler> mHandler;
    AutoPtr<ICalculatorListener> mListener;
};

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

// } // namespace JSPkgName
// } // namespace Node
// } // namespace DevSamples
// } // namespace Elastos

//#endif // __H__