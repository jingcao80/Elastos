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

#include "elastos/droid/server/firewall/CategoryFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

FilterFactory::FilterFactory()
{}

ECode FilterFactory::constructor(
    /* in */ const String& tag)
{
    if (tag.IsNull()) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    mTag = tag;

    return NOERROR;
}

String FilterFactory::GetTagName()
{
    return mTag;
}

} // Firewall
} // Server
} // Droid
} // Elastos
