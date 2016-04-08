#ifndef __CPLAYACTIVITYLISTENER_H__
#define __CPLAYACTIVITYLISTENER_H__

#include "_CPlayActivityListener.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IActivity;

namespace MusicPlayer {

CarClass(CPlayActivityListener)
{
public:
    CARAPI constructor(
        /* [in] */ IWeakReference* host);

    CARAPI OnTick(
        /* [in] */ Int32 duration);

    CARAPI OnCompletion();

private:
    AutoPtr<IWeakReference> mHost;

};

} // namespace MusicPlayer

#endif //__CPLAYACTIVITYLISTENER_H__
