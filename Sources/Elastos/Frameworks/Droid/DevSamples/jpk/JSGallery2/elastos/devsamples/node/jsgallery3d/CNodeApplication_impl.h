#include "CNodeApplication.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::EIID_IApplication;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

EXTERN IHandler* myHandler;

const String JSActName::TAG(JSActNameStr);

#ifndef DBG_TAG_DEF
#define DBG_TAG_DEF
static const String DBG_TAG(JSActNameStr);
#endif

ECode JSActName::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessageListener> listener = IMessageListener::Probe(mHost->mListener);
    return listener->OnHandleMessage(mHost, msg);
}

CAR_INTERFACE_IMPL(JSActName, Application, IApplicationOne)

JS_CAR_OBJECT_IMPL(JSActName)

ECode JSActName::constructor()
{
    Logger::I(TAG, " >> constructor()");

    return Application::constructor();
}

ECode JSActName::OnCreate()
{
    Logger::D(TAG, "OnCreate()-----");
    Application::OnCreate();

    mHandler = new MyHandler(this);

    myHandler = mHandler;

    String _pkgPath = String("/data/temp/node/bin/");
    String _pkgName = String(JSPkgNameStr);
    String _nspName = String("Elastos.DevSamples.Node.");

    mPackageName = _nspName + _pkgName;
    mActivityName = String(JSActNameStr);

    String _helperEcoName = _pkgPath + mPackageName + String(".Helper.eco");
    String _helperClsName = mPackageName + String(".") + mActivityName + String("Helper");

    AutoPtr<IInterface> helper;
    ECode ec = JSEvtName::Require(_helperEcoName, _helperClsName, (IInterface**)&helper);

    if (FAILED(ec)) {
        ALOGD("OnCreate========create Helper failed!======nodejs module will be used");
        AutoPtr<IInterface> _this = this->Probe(EIID_IInterface);
        JSEvtName::RegisterActivity(mPackageName, mActivityName, _this, (IActivityListener**)&mListener, mHandler.Get());
    }
    else {
        ALOGD("OnCreate========create Helper success!======C++ epk will be used");
        mListener = IApplicationListener::Probe(helper);
    }

    return mListener->OnCreate(this);
}

ECode JSActName::OnTest()
{
    return NOERROR;
}

} // namespace JSPkgName
} // namespace Node
} // namespace DevSamples
} // namespace Elastos