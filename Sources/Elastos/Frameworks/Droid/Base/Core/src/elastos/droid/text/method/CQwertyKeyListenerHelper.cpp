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

#include "elastos/droid/text/method/CQwertyKeyListenerHelper.h"
#include "elastos/droid/text/method/CQwertyKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CQwertyKeyListenerHelper)

CAR_INTERFACE_IMPL(CQwertyKeyListenerHelper, Singleton, IQwertyKeyListenerHelper)

ECode CQwertyKeyListenerHelper::GetInstance(
    /* [in] */ Boolean autoText,
    /* [in] */ Capitalize cap,
    /* [out] */ IQwertyKeyListener** ret)
{
    return QwertyKeyListener::GetInstance(autoText, cap, ret);
}

ECode CQwertyKeyListenerHelper::GetInstanceForFullKeyboard(
    /* [out] */ IQwertyKeyListener** ret)
{
    return QwertyKeyListener::GetInstanceForFullKeyboard(ret);
}

ECode CQwertyKeyListenerHelper::MarkAsReplaced(
    /* [in] */ ISpannable* content,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const String& original)
{
    return QwertyKeyListener::MarkAsReplaced(content, start, end, original);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
