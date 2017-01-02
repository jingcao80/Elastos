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

#include "elastos/droid/server/am/ActivityResult.h"

using Elastos::Droid::App::CResultInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ActivityResult::ActivityResult(
    /* [in] */ ActivityRecord* from,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
    : mFrom(from)
{
    CResultInfo::New(resultWho, requestCode, resultCode, data, (IResultInfo**)&mResultInfo);
}

ActivityResult::~ActivityResult()
{
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
