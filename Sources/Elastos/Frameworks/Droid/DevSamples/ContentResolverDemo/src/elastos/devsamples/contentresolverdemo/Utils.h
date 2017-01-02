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

#ifndef __Elastos_DevSamples_ContentProviderDemo_Utils_H__
#define __Elastos_DevSamples_ContentProviderDemo_Utils_H__

#include <Elastos.Droid.Net.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace DevSamples {
namespace ContentResolverDemo {

class Utils
{
public:
    static const String DB_NAME;
    static const String TABLE_NAME;
    static const Int32 VERSION;

    static const String TAG_ID;
    static const String USERNAME;
    static const String SEX;
    static const String EMAIL;

    static const String AUTOHORITY;
    static const Int32 ITEM = 1;
    static const Int32 ITEM_ID = 2;

    static const String CONTENT_TYPE;
    static const String CONTENT_ITEM_TYPE;

    static const AutoPtr<IUri> CONTENT_URI;

private:
    Utils();
    Utils(const Utils&);
};

} // namespace ContentResolverDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ContentProviderDemo_Utils_H__
