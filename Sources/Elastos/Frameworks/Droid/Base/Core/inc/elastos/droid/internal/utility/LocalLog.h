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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class LocalLog
    : public Object
    , public ILocalLog
{
public:
    CAR_INTERFACE_DECL()

    LocalLog();

    CARAPI constructor(
        /* [in] */ const String& tag);

    CARAPI W(
        /* [in] */ const String& msg);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& header,
        /* [in] */ const String& prefix,
        /* [out] */ Boolean* result);

private:
    String mTag;
    Int32 mMaxLines;
    List<String> mLines;
    Object mLinesLock;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_LOCALLOG_H__
