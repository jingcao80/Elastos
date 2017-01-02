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

#include "CAm.h"
#include "Am.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_SINGLETON_IMPL(CAm);

CAR_INTERFACE_IMPL(CAm, Singleton, IAm);

ECode CAm::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    Am* am = new Am;
    if (!am) return E_OUT_OF_MEMORY;

    return am->Run(&const_cast<ArrayOf<String>&>(args));
}

}
}
}
}

