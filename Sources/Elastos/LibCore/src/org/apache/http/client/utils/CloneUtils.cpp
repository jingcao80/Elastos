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

#include "Elastos.CoreLibrary.Core.h"
#include "org/apache/http/client/utils/CloneUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Utils {

ECode CloneUtils::Clone(
    /* [in] */ IObject* obj,
    /* [out] */ IObject** cloneObj)
{
    VALIDATE_NOT_NULL(cloneObj)

    if (obj == NULL) {
        *cloneObj = NULL;
        return NOERROR;
    }
    if (ICloneable::Probe(obj)) {
        // Class<?> clazz = obj.getClass ();
        // Method m;
        // try {
        //     m = clazz.getMethod("clone", (Class[]) null);
        // } catch (NoSuchMethodException ex) {
        //     throw new NoSuchMethodError(ex.getMessage());
        // }
        // try {
        // return m.invoke(obj, (Object []) null);
        AutoPtr<IInterface> tmp;
        ICloneable::Probe(obj)->Clone((IInterface**)&tmp);
        *cloneObj = IObject::Probe(tmp);
        REFCOUNT_ADD(*cloneObj)
        return NOERROR;
        // } catch (InvocationTargetException ex) {
        //     Throwable cause = ex.getCause();
        //     if (cause instanceof CloneNotSupportedException) {
        //         throw ((CloneNotSupportedException) cause);
        //     } else {
        //         throw new Error("Unexpected exception", cause);
        //     }
        // } catch (IllegalAccessException ex) {
        //     throw new IllegalAccessError(ex.getMessage());
        // }
    }
    else {
        return E_CLONE_NOT_SUPPORTED_EXCEPTION;
        // throw new CloneNotSupportedException();
    }
}

} // namespace Utils
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org