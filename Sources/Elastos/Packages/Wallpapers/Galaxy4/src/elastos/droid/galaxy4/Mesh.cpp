
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
