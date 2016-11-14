
#ifndef __ELASTOS_DROID_WIDGET_DOUBLEDIGITMANAGER_H__
#define __ELASTOS_DROID_WIDGET_DOUBLEDIGITMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::IInteger32;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Widget {

class DoubleDigitManager
    : public Object
    , public IDoubleDigitManager
{
private:
    class DoubleDigitManagerRunnable
        : public Runnable
    {
    public:
        DoubleDigitManagerRunnable(
            /* [in] */ DoubleDigitManager* host);

        CARAPI Run();

    private:
        DoubleDigitManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DoubleDigitManager();

    CARAPI constructor(
        /* [in] */ Int64 timeoutInMillis,
        /* [in] */ IDoubleDigitManagerCallBack* callBack);

    CARAPI ReportDigit(
            /* [in] */ Int32 digit);

private:
    Int64 mTimeoutInMillins;

    AutoPtr<IDoubleDigitManagerCallBack> mCallBack;

    AutoPtr<IInteger32> mIntermediateDigit;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
