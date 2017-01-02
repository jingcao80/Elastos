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

#include "elastos/droid/internal/utility/CPreconditions.h"
#include "elastos/droid/internal/utility/Preconditions.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CPreconditions, Singleton, IPreconditions)

CAR_SINGLETON_IMPL(CPreconditions)

ECode CPreconditions::CheckNotNull(
    /* [in] */ IInterface* reference)
{
    return Preconditions::CheckNotNull(reference);
}

ECode CPreconditions::CheckNotNull(
    /* [in] */ IInterface* reference,
    /* [in] */ IObject* errorMessage)
{
    return Preconditions::CheckNotNull(reference, errorMessage);
}

ECode CPreconditions::CheckNotNull(
    /* [in] */ IInterface* reference,
    /* [in] */ const String& errorMessage)
{
    return Preconditions::CheckNotNull(reference, errorMessage);
}

ECode CPreconditions::CheckState(
    /* [in] */ Boolean expression)
{
    return Preconditions::CheckState(expression);
}

ECode CPreconditions::CheckFlagsArgument(
    /* [in] */ Int32 requestedFlags,
    /* [in] */ Int32 allowedFlags)
{
    return Preconditions::CheckFlagsArgument(requestedFlags, allowedFlags);
}

ECode CPreconditions::CheckArgumentNonnegative(
    /* [in] */ Int32 value,
    /* [in] */ const String& errorMessage)
{
    return Preconditions::CheckArgumentNonnegative(value, errorMessage);
}

ECode CPreconditions::CheckArgumentNonnegative(
    /* [in] */ Int64 value,
    /* [in] */ const String& errorMessage)
{
    return Preconditions::CheckArgumentNonnegative(value, errorMessage);
}

ECode CPreconditions::CheckArgumentPositive(
    /* [in] */ Int32 value,
    /* [in] */ const String& errorMessage)
{
    return Preconditions::CheckArgumentPositive(value, errorMessage);
}

ECode CPreconditions::CheckArgumentFinite(
    /* [in] */ Float value,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckArgumentFinite(value, valueName);
}

ECode CPreconditions::CheckArgumentInRange(
    /* [in] */ Float value,
    /* [in] */ Float lower,
    /* [in] */ Float upper,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckArgumentInRange(value, lower, upper, valueName);
}

ECode CPreconditions::CheckArgumentInRange(
    /* [in] */ Int32 value,
    /* [in] */ Int32 lower,
    /* [in] */ Int32 upper,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckArgumentInRange(value, lower, upper, valueName);
}

ECode CPreconditions::CheckArrayElementsNotNull(
    /* [in] */ ArrayOf<IInterface*>* value,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckArrayElementsNotNull(value, valueName);
}

ECode CPreconditions::CheckCollectionElementsNotNull(
    /* [in] */ ICollection* value,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckCollectionElementsNotNull(value, valueName);
}

ECode CPreconditions::CheckCollectionNotEmpty(
    /* [in] */ ICollection* value,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckCollectionNotEmpty(value, valueName);
}

ECode CPreconditions::CheckArrayElementsInRange(
    /* [in] */ ArrayOf<Float>* value,
    /* [in] */ Float lower,
    /* [in] */ Float upper,
    /* [in] */ const String& valueName)
{
    return Preconditions::CheckArrayElementsInRange(value, lower, upper, valueName);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
