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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class PageTransitionTypes
{
public:
    static const Int32 PAGE_TRANSITION_LINK = 0;
    static const Int32 PAGE_TRANSITION_TYPED = 1;
    static const Int32 PAGE_TRANSITION_AUTO_BOOKMARK = 2;
    static const Int32 PAGE_TRANSITION_AUTO_SUBFRAME = 3;
    static const Int32 PAGE_TRANSITION_MANUAL_SUBFRAME = 4;
    static const Int32 PAGE_TRANSITION_GENERATED = 5;
    static const Int32 PAGE_TRANSITION_AUTO_TOPLEVEL = 6;
    static const Int32 PAGE_TRANSITION_FORM_SUBMIT = 7;
    static const Int32 PAGE_TRANSITION_RELOAD = 8;
    static const Int32 PAGE_TRANSITION_KEYWORD = 9;
    static const Int32 PAGE_TRANSITION_KEYWORD_GENERATED = 10;
    static const Int32 PAGE_TRANSITION_LAST_CORE = PAGE_TRANSITION_KEYWORD_GENERATED;
    static const Int32 PAGE_TRANSITION_CORE_MASK = 0xFF;
    static const Int32 PAGE_TRANSITION_BLOCKED = 0x00800000;
    static const Int32 PAGE_TRANSITION_FORWARD_BACK = 0x01000000;
    static const Int32 PAGE_TRANSITION_FROM_ADDRESS_BAR = 0x02000000;
    static const Int32 PAGE_TRANSITION_HOME_PAGE = 0x04000000;
    static const Int32 PAGE_TRANSITION_FROM_API = 0x08000000;
    static const Int32 PAGE_TRANSITION_CHAIN_START = 0x10000000;
    static const Int32 PAGE_TRANSITION_CHAIN_END = 0x20000000;
    static const Int32 PAGE_TRANSITION_CLIENT_REDIRECT = 0x40000000;
    static const Int32 PAGE_TRANSITION_SERVER_REDIRECT = 0x80000000;
    static const Int32 PAGE_TRANSITION_IS_REDIRECT_MASK = 0xC0000000;
    static const Int32 PAGE_TRANSITION_QUALIFIER_MASK = 0xFFFFFF00;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_PAGETRANSITIONTYPES_H__
