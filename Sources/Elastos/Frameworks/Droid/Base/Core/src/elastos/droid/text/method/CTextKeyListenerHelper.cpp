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

#include "elastos/droid/text/method/CTextKeyListenerHelper.h"
#include "elastos/droid/text/method/TextKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CTextKeyListenerHelper)

CAR_INTERFACE_IMPL(CTextKeyListenerHelper, Singleton, ITextKeyListenerHelper)

ECode CTextKeyListenerHelper::Clear(
    /* [in] */ IEditable* e)
{
    return TextKeyListener::Clear(e);
}

ECode CTextKeyListenerHelper::GetInstance(
    /* [out] */ ITextKeyListener** ret)
{
    return TextKeyListener::GetInstance(ret);
}

ECode CTextKeyListenerHelper::GetInstance(
    /* [in] */ Boolean autotext,
    /* [in] */ Capitalize cap,
    /* [out] */ ITextKeyListener** ret)
{
    return TextKeyListener::GetInstance(autotext, cap, ret);
}

ECode CTextKeyListenerHelper::ShouldCap(
    /* [in] */ Capitalize cap,
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 off,
    /* [out] */ Boolean* ret)
{
    return TextKeyListener::ShouldCap(cap, cs, off, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

