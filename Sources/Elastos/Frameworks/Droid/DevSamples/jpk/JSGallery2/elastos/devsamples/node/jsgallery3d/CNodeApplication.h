//#ifndef __CNodeActivity_H__
//#define __CNodeActivity_H__

#include "CNodeListener.h"

#define JSActCarClassHead EE(JSPkgName,JSActName)
#define JSActNameStr FF(JSActName)

//#define JSINC(x) __##x

//#ifndef JSINC(JSActName)
//#define JSINC(JSActName)

#include JSActCarClassHead

//#include <elastos/droid/app/Activity.h>
#include <elastos/droid/app/Application.h>
#include <elastos/droid/os/Handler.h>

//using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Application;
//using Elastos::Droid::View::IViewOnClickListener;

using Elastos::Droid::Os::Handler;

//using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

JSCarClass(JSActName)
    , public Application
    , public IApplicationOne
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

    CARAPI OnTest();

protected:
    CARAPI OnCreate();

private:
    static const String TAG;

    String mPackageName;
    String mActivityName;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IApplicationListener> mListener;
};

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos

//#endif // __H__