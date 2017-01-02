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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CMessage.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CMessageHelper, Singleton, IMessageHelper)

CAR_SINGLETON_IMPL(CMessageHelper)

ECode CMessageHelper::Obtain(
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain();
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IMessage* orig,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(orig);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, obj);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, arg1, arg2);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, what, arg1, arg2, obj);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

ECode CMessageHelper::Obtain(
    /* [in] */ IHandler* target,
    /* [in] */ IRunnable* callback,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessage> m = CMessage::Obtain(target, callback);
    *msg = m;
    REFCOUNT_ADD(*msg);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
