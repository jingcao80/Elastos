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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

class AwHashSet
{
public:
    static CARAPI_(void*) ElaHashSetCallback_Init();

private:
    static CARAPI_(Boolean) Add(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(void) Clear(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Clone(
            /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) Contains(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(Boolean) IsEmpty(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Iterator(
            /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) Remove(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(Int32) Size(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHS();

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSI(
            /* [in] */ Int32 p0);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSIF(
            /* [in] */ Int32 p0,
            /* [in] */ Float p1);

    static CARAPI_(AutoPtr<IInterface>) ConstructorJUHSJUC(
            /* [in] */ IInterface* extends);
};

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__
