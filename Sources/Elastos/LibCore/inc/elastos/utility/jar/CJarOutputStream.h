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

#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__

#include "_Elastos_Utility_Jar_CJarOutputStream.h"
#include "ZipOutputStream.h"

using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::ZipOutputStream;
using Elastos::Utility::Jar::IManifest;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CJarOutputStream)
    , public ZipOutputStream
    , public IJarOutputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ IManifest* manifest);

    CARAPI constructor(
        /* [in] */ IOutputStream* os);

private:
    AutoPtr<IManifest> mManifest;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJAROUTPUTSTREAM_H__
