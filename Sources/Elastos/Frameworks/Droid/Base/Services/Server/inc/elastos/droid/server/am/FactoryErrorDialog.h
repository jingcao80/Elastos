
#ifndef __ELASTOS_DROID_SERVER_AM_FACTORYERRORDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_FACTORYERRORDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::HandlerBase;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class FactoryErrorDialog
    : public BaseErrorDialog
{
private:
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler();

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    FactoryErrorDialog(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* msg);

    CARAPI_(void) OnStop();

private:
    AutoPtr<MyHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_FACTORYERRORDIALOG_H__
