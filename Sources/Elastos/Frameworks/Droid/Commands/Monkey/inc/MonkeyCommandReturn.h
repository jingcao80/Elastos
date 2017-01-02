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

#ifndef __MONKEYCOMMANDRETURN_H_
#define __MONKEYCOMMANDRETURN_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyCommandReturn
{
public:
    MonkeyCommandReturn();

    virtual CARAPI HasMessage(
        /* [out] */ Boolean *result);

    virtual CARAPI GetMessage(
        /* [out] */ String *message);

    virtual CARAPI WasSuccessful(
        /* [out] */ Boolean *result);

protected:
    CARAPI Init(
        /* [in] */ Boolean success,
        /* [in] */ const String& message = String());
private:
    volatile const Boolean mSuccess;
    volatile const String mMessage;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYCOMMANDRETURN_H_
