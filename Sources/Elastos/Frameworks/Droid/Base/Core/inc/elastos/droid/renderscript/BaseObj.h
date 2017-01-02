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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_BASEOBJ_H__
#define __ELASTOS_DROID_RENDERSCRIPT_BASEOBJ_H__

#include "Elastos.Droid.RenderScript.h"
#include "elastos/droid/renderscript/RenderScript.h"

namespace Elastos {
namespace Droid {
namespace RenderScript {

class BaseObj
    : public Object
    , public IBaseObj
{
public:
    BaseObj();

    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs);

    virtual ~BaseObj();

    CAR_INTERFACE_DECL()

    /**
     * setName assigns a name to an object.  This object can later be looked up
     * by this name.
     *
     * @param name The name to assign to the object.
     */
    CARAPI SetName(
        /* [in] */ const String& name);

    /**
     * @return name of the renderscript object
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Frees any native resources associated with this object.  The
     * primary use is to force immediate cleanup of resources when it is
     * believed the GC will not respond quickly enough.
     */
    CARAPI Destroy();

    /**
     * Calculates the hash code value for a BaseObj.
     *
     * @return int
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Compare the current BaseObj with another BaseObj for equality.
     *
     * @param obj The object to check equality with.
     *
     * @return boolean
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

protected:
    CARAPI SetID(
        /* [in] */ Int64 id);

    /**
     * Lookup the native object ID for this object.  Primarily used by the
     * generated reflected code.
     *
     * @param rs Context to verify against internal context for
     *           match.
     *
     * @return long
     */
    CARAPI GetID(
        /* [in] */ RenderScript* rs,
        /* [out] */ Int64* id);

    CARAPI CheckValid();

    /**
     * If an object came from an a3d file, java fields need to be
     * created with objects from the native layer
     */
    virtual CARAPI_(void) UpdateFromNative();

private:
    CARAPI_(void) HelpDestroy();

protected:
    AutoPtr<RenderScript> mRS;

private:
    Int64 mID;
    Boolean mDestroyed;
    String mName;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_RENDERSCRIPT_BASEOBJ_H__
