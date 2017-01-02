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

#ifndef __ELASTOS_DROID_PROVIDER_USERDICTIONARY_H__
#define __ELASTOS_DROID_PROVIDER_USERDICTIONARY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Provider {

class UserDictionary
{
public:
    class Words
    {
    public:
        static CARAPI AddWord(
            /* [in] */ IContext* context,
            /* [in] */ const String& word,
            /* [in] */ Int32 frequency,
            /* [in] */ Int32 localeType);

        static CARAPI AddWord(
            /* [in] */ IContext* context,
            /* [in] */ const String& word,
            /* [in] */ Int32 frequency,
            /* [in] */ const String& shortcut,
            /* [in] */ ILocale* locale);

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

public:
    static const AutoPtr<IUri> CONTENT_URI;

private:
    static const Int32 FREQUENCY_MIN;
    static const Int32 FREQUENCY_MAX;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_USERDICTIONARY_H__