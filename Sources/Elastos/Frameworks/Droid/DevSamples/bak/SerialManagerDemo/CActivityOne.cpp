
#include "CActivityOne.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/System.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elautoptr.h>
#include <stdio.h>
#include <unistd.h>
#include "R.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CSystemVibrator;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Core::System;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Hardware::CSerialManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SerialManagerDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    Slogger::I("CActivityOne::OnCreate","============GetSystemService");
    GetSystemService(IContext::SERIAL_SERVICE, (IInterface**)&mSm);
    assert(mSm != NULL);

    Slogger::I("CActivityOne::OnCreate","============GetSerialPorts");
    AutoPtr<ArrayOf<String> > ports;
    mSm->GetSerialPorts((ArrayOf<String>**)&ports);

    Slogger::I("CActivityOne::OnCreate","SerialPorts Num=%d",ports->GetLength());
    /*******************
    *
    *ports->GetLength()=0,  has no serial devices
    *
    ********************/
    for(int i=0;i<ports->GetLength();i++)
    {
        Slogger::I("CActivityOne::OnCreate","port%d name=%d",i,(*ports)[i].string());
    }

    //Slogger::I("CActivityOne::OnCreate","============OpenSerialPort, name=%s",name.string());
    //mSm->OpenSerialPort(const String& name, Int32 speed, (ISerialPort**)&mSp);
    //Slogger::I("CActivityOne::OnCreate","============Read");
    //mSp->Read([in] IByteBuffer* buffer, [out] Int32* num);
    //Slogger::I("CActivityOne::OnCreate","============Write");
    //mSp->Write([in] IByteBuffer* buffer, [in] Int32 length);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace SerialManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
