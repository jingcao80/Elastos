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

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/webkit/WebIconDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

AutoPtr<IWebIconDatabase> WebIconDatabase::GetInstance()
{
    // XXX: Must be created in the UI thread.
    AutoPtr<IWebIconDatabase> temp;
    WebViewFactory::GetProvider()->GetWebIconDatabase((IWebIconDatabase**)&temp);
    return temp;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
