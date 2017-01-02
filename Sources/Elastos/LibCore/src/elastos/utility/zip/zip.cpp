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

#include "CZipEntry.h"
#include "CZipFile.h"
#include "CDeflaterInputStream.h"
#include "CDeflaterOutputStream.h"
#include "CInflaterInputStream.h"
#include "CInflaterOutputStream.h"
#include "CZipInputStream.h"
#include "CZipOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_OBJECT_IMPL(CZipEntry)

CAR_OBJECT_IMPL(CZipFile)

CAR_OBJECT_IMPL(CDeflaterInputStream)

CAR_OBJECT_IMPL(CDeflaterOutputStream)

CAR_OBJECT_IMPL(CInflaterInputStream)

CAR_OBJECT_IMPL(CInflaterOutputStream)

CAR_OBJECT_IMPL(CZipInputStream)

CAR_OBJECT_IMPL(CZipOutputStream)

} // namespace Zip
} // namespace Utility
} // namespace Elastos
