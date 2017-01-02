//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
