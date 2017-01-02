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

#include "elastos/droid/content/res/CColorStateListHelper.h"
#include "elastos/droid/content/res/CColorStateList.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CColorStateListHelper, Singleton, IColorStateListHelper)

CAR_SINGLETON_IMPL(CColorStateListHelper)

ECode CColorStateListHelper::ValueOf(
    /* [in] */ Int32 color,
    /* [out] */ IColorStateList** list)
{
    return CColorStateList::ValueOf(color, list);
}

ECode CColorStateListHelper::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IColorStateList** list)
{
    return CColorStateList::CreateFromXml(r, parser, list);
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

