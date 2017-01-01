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

#ifndef __ORG_ALLJOYN_BUS_SIGNATURE_H__
#define __ORG_ALLJOYN_BUS_SIGNATURE_H__

#include <elastos/coredef.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * Signature provides static methods for converting between Java and DBus type signatures.
 * This class is used internally.
 */
class Signature
{
public:
    static AutoPtr<ArrayOf<String> > Split(
        /* [in] */ const String& signature);

    /**
     * Compute the DBus type signature of the type.
     *
     * @param type the Java type
     * @param signature the annotated signature for the type
     * @return the DBus type signature
     * @throws AnnotationBusException if the signature cannot be computed
     */
    static CARAPI_(String) TypeSig(
        /* [in] */ IDataTypeInfo* type,
        /* [in] */ const String& signature);

    /**
     * Compute the DBus type signature of the types.
     * @param types The Java types.
     * @param signature The annotated signature for the types.
     */
    static CARAPI_(String) TypeSig(
        /* [in] */ ArrayOf<IParamInfo*>* types,
        /* [in] */ const String& signature,
        /* [in] */ Boolean input);

    static CARAPI_(String) TypeSig(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& signature);

private:
    Signature();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_SIGNATURE_H__
