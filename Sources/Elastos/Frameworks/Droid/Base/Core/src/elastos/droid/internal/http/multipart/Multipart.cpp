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

#include "elastos/droid/internal/http/multipart/CByteArrayPartSource.h"
#include "elastos/droid/internal/http/multipart/CFilePart.h"
#include "elastos/droid/internal/http/multipart/CFilePartSource.h"
#include "elastos/droid/internal/http/multipart/CMultipartEntity.h"
#include "elastos/droid/internal/http/multipart/CStringPart.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_OBJECT_IMPL(CByteArrayPartSource)

CAR_OBJECT_IMPL(CFilePart)

CAR_OBJECT_IMPL(CFilePartSource)

CAR_OBJECT_IMPL(CMultipartEntity)

CAR_OBJECT_IMPL(CStringPart)

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
