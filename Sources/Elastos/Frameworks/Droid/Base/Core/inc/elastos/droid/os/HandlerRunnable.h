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

#ifndef __ELASTOS_DROID_OS_HANDLERRUNNABLE_H__
#define __ELASTOS_DROID_OS_HANDLERRUNNABLE_H__

#include <elastos/droid/os/Handler.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Base class for HandlerRunnable
 */
class ECO_PUBLIC HandlerRunnable
    : public Handler
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("HandlerRunnable")

    HandlerRunnable(
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ ILooper* looper,
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ IHandlerCallback* callback,
        /* [in] */ Boolean takeStrongRefOfCallback,
        /* [in] */ Boolean async = FALSE);

    HandlerRunnable(
        /* [in] */ ILooper* looper,
        /* [in] */ IHandlerCallback* callback,
        /* [in] */ Boolean takeStrongRefOfCallback,
        /* [in] */ Boolean async = FALSE);

    virtual ~HandlerRunnable();

    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);
};


} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_HANDLERRUNNABLE_H__
