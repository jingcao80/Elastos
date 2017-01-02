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

#include "elastos/droid/internal/http/multipart/Part.h"
#include "elastos/droid/internal/http/multipart/CPartHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL(CPartHelper, Singleton, IPartHelper)

CAR_SINGLETON_IMPL(CPartHelper)

ECode CPartHelper::GetBoundary(
    /* [out] */ String* boundary)
{
    return Part::GetBoundary(boundary);
}

ECode CPartHelper::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts)
{
    return Part::SendParts(outStream, parts);
}

ECode CPartHelper::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary)
{
    return Part::SendParts(outStream, parts, partBoundary);
}

ECode CPartHelper::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [out] */ Int64* length)
{
    return Part::GetLengthOfParts(parts, length);
}

ECode CPartHelper::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary,
    /* [out] */ Int64* length)
{
    return Part::GetLengthOfParts(parts, partBoundary, length);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
