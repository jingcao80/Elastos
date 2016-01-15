
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

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace VibratorDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    CSystemVibrator::New((IVibrator**)&mVb);
    assert(mVb != NULL);

    /*************************************************
    *
    *Vibrate(ArrayOf<Int64> &pattern, int mode); mode=-1 : once
    *
    **************************************************/
    // PFL_EX("===Begin Vibrate mode: once,short");
    // ArrayOf<Int64>* pattern1 = ArrayOf<Int64>::Alloc(4);
    // (*pattern1)[0]=1000;
    // (*pattern1)[1]=10;
    // (*pattern1)[2]=100;
    // (*pattern1)[3]=1000;  //short
    // PFL_EX("CActivityOne::OnCreate 1");
    // mVb->Vibrate(*pattern1, -1);
    // PFL_EX("CActivityOne::OnCreate 2");
    // sleep(5);
    // mVb->Cancel();
    // PFL_EX("CActivityOne::OnCreate 3");
    // ArrayOf<Int64>::Free(pattern1);


    /*************************************************
    *
    *Vibrate(ArrayOf<Int64> &pattern, int mode); mode!=-1 : repeat
    *
    **************************************************/
    // PFL_EX("===Begin Vibrate mode: repeat,long");
    // ArrayOf<Int64>* pattern2= ArrayOf<Int64>::Alloc(4);
    // (*pattern2)[0]=100;
    // (*pattern2)[1]=100;
    // (*pattern2)[2]=100;
    // (*pattern2)[3]=1000; //long
    // PFL_EX("CActivityOne::OnCreate 4");
    // mVb->Vibrate(*pattern2, 0);
    // PFL_EX("CActivityOne::OnCreate 5");
    // sleep(15);
    // PFL_EX("CActivityOne::OnCreate 6");
    // mVb->Cancel();
    // PFL_EX("CActivityOne::OnCreate 7");
    // ArrayOf<Int64>::Free(pattern2);


    /*************************************************
    *
    *Vibrate(ArrayOf<Int64> &pattern, int mode); mode!=-1 : repeat
    *
    **************************************************/
    // PFL_EX("===Begin Vibrate mode: repeat,rhythm");
    // ArrayOf<Int64>* pattern3 = ArrayOf<Int64>::Alloc(5);
    // (*pattern3)[0]=1000;
    // (*pattern3)[1]=50;
    // (*pattern3)[2]=1000;
    // (*pattern3)[3]=50;
    // (*pattern3)[4]=1000;
    // PFL_EX("CActivityOne::OnCreate 8");
    // mVb->Vibrate(*pattern3, 0);
    // PFL_EX("CActivityOne::OnCreate 9");
    // sleep(15);
    // PFL_EX("CActivityOne::OnCreate 10");
    // mVb->Cancel();
    // PFL_EX("CActivityOne::OnCreate 11");
    // ArrayOf<Int64>::Free(pattern3);


    /*************************************************
    *
    *Vibrate(Int64 milliseconds); milliseconds : vibrate time
    *
    **************************************************/
    PFL_EX("===Begin Vibrate mode: vibrate time");
    mVb->Vibrate(6000);
    PFL_EX("CActivityOne::OnCreate 12");
    sleep(10);
    PFL_EX("CActivityOne::OnCreate 13");
    mVb->Cancel();
    PFL_EX("CActivityOne::OnCreate 14");


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

} // namespace VibratorDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
