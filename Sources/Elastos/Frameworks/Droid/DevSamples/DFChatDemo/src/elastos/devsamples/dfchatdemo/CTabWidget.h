
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CTABWIDGET_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CTABWIDGET_H__

#include "_Elastos_DevSamples_DFChatDemo_CTabWidget.h"
#include <elastos/droid/app/TabActivity.h>

using Elastos::Droid::App::TabActivity;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

CarClass(CTabWidget)
    , public TabActivity
{
public:
    CARAPI constructor();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CTABWIDGET_H__
