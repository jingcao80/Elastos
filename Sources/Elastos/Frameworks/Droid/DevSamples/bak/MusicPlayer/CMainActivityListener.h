#ifndef __CMAINACTIVITYLISTENER_H__
#define __CMAINACTIVITYLISTENER_H__

#include "_CMainActivityListener.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IActivity;

namespace MusicPlayer {

CarClass(CMainActivityListener)
{
public:
    CARAPI constructor(
        /* [in] */ IWeakReference* host);

    CARAPI OnTick(
        /* [in] */ Int32 duration);

    CARAPI OnCompletion();

    CARAPI Run();

private:
    AutoPtr<IWeakReference> mHost;

};

} // namespace MusicPlayer

#endif //__CMAINACTIVITYLISTENER_H__
