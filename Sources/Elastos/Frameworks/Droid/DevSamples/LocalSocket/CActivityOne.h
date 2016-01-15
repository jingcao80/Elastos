
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include <tstprintf.h>
#include <elastos/core/Math.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalServerSocket;
using Elastos::Droid::Net::CLocalServerSocket;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::Math;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileDescriptorHelper;
using Elastos::IO::CFileDescriptorHelper;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;

class CActivityOne : public Activity
{
public:
    ECode Test1();
    ECode Test2();

    ECode HandleMessage(int index);
    ECode PostcppCallback(int index);


protected:
    CARAPI OnCreate(/* [in] */ IBundle* savedInstanceState);
    CARAPI OnStart() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnResume() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnPause() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnStop() { printf("%s\n", __FUNCTION__);return NOERROR;}
    CARAPI OnDestroy() { printf("%s\n", __FUNCTION__);return NOERROR;}

public:
    static const String mSockAddr;

private:
    static void* EntryRoutine(void *arg);

    AutoPtr<IHandler> mDefaultHandler;

    Boolean mbMarked;
    int mPeriodic;
};

#endif // __CACTIVITYONE_H__
