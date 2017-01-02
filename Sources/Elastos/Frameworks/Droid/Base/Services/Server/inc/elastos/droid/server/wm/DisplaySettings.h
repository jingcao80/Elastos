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

#ifndef __ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__
#define __ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Utility::IAtomicFile;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Current persistent settings about a display
 */
class DisplaySettings : public Object
{
public:
    class Entry : public Object
    {
    public:
        Entry(
            /* [in] */ const String& name)
            : mName(name)
            , mOverscanLeft(0)
            , mOverscanTop(0)
            , mOverscanRight(0)
            , mOverscanBottom(0)
        {}

    public:
        String mName;
        Int32 mOverscanLeft;
        Int32 mOverscanTop;
        Int32 mOverscanRight;
        Int32 mOverscanBottom;
    };

public:
    DisplaySettings(
        /* [in] */ IContext* context);

    CARAPI_(void) GetOverscanLocked(
        /* [in] */ const String& name,
        /* [in] */ IRect* outRect);

    CARAPI_(void) SetOverscanLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI ReadSettingsLocked();

    CARAPI_(void) WriteSettingsLocked();

private:
    CARAPI_(Int32) GetIntAttribute(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& name);

    ECode ReadDisplay(
        /* [in] */ IXmlPullParser* parser);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IAtomicFile> mFile;
    HashMap<String, AutoPtr<Entry> > mEntries;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__
