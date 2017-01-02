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

#ifndef __CMONKEYCOMMANDRETURN_H__
#define __CMONKEYCOMMANDRETURN_H__

#include "_CMonkeyCommandReturn.h"
#include "MonkeyCommandReturn.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyCommandReturn), public MonkeyCommandReturn
{
public:
    CAR_OBJECT_DECL()

    CARAPI HasMessage(
        /* [out] */ Boolean *result);

    CARAPI GetMessage(
        /* [out] */ String *message);

    CARAPI WasSuccessful(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ Boolean success);

    CARAPI constructor(
        /* [in] */ Boolean success,
        /* [in] */ const String& message);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYCOMMANDRETURN_H__
