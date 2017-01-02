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

#include "Mesh.h"
#include "RenderScript.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

//=================================================
// Mesh::AllocationBuilder
//=================================================
Mesh::AllocationBuilder::AllocationBuilder(
    /* [in] */ RenderScript* rs)
    : mRS(rs)
{
}

ECode Mesh::AllocationBuilder::Create(
    /* [out] */ Mesh** mesh)
{
    VALIDATE_NOT_NULL(mesh);
    FAIL_RETURN(mRS->Validate());

    // AutoPtr< ArrayOf<Int64> > vtx = ArrayOf<Int64>::Alloc(mVertexTypeCount);
    // long[] idx = new long[mIndexTypes.size()];
    // int[] prim = new int[mIndexTypes.size()];

    // Allocation[] indexBuffers = new Allocation[mIndexTypes.size()];
    // Primitive[] primitives = new Primitive[mIndexTypes.size()];
    // Allocation[] vertexBuffers = new Allocation[mVertexTypeCount];

    // for(int ct = 0; ct < mVertexTypeCount; ct ++) {
    //     Entry entry = mVertexTypes[ct];
    //     vertexBuffers[ct] = entry.a;
    //     vtx[ct] = entry.a.getID(mRS);
    // }

    // for(int ct = 0; ct < mIndexTypes.size(); ct ++) {
    //     Entry entry = (Entry)mIndexTypes.elementAt(ct);
    //     long allocID = (entry.a == null) ? 0 : entry.a.getID(mRS);
    //     indexBuffers[ct] = entry.a;
    //     primitives[ct] = entry.prim;

    //     idx[ct] = allocID;
    //     prim[ct] = entry.prim.mID;
    // }

    // long id = mRS.nMeshCreate(vtx, idx, prim);
    // Mesh newMesh = new Mesh(id, mRS);
    // newMesh.mVertexBuffers = vertexBuffers;
    // newMesh.mIndexBuffers = indexBuffers;
    // newMesh.mPrimitives = primitives;

    // *mesh = newMesh;
    // REFCOUNT_ADD(*mesh);
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
