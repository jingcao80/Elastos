
#include "CVisibilityGL.h"
#include "CMatrixGL.h"
#include "Poly.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

namespace Elastos {
namespace Droid {
namespace Opengl {

static Float Distance2(Float x, Float y, Float z) {
    return x * x + y * y + z * z;
}

static Float Distance(Float x, Float y, Float z) {
    return Elastos::Core::Math::Sqrt(Distance2(x, y, z));
}

static void normalizePlane(float* p) {
    float rdist = 1.0f / Distance(p[0], p[1], p[2]);
    for(int i = 0; i < 4; i++) {
        p[i] *= rdist;
    }
}

static void computeFrustum(const float* m, float* f) {
    float m3 = m[3];
    float m7 = m[7];
    float m11 = m[11];
    float m15 = m[15];
    // right
    f[0] = m3  - m[0];
    f[1] = m7  - m[4];
    f[2] = m11 - m[8];
    f[3] = m15 - m[12];
    normalizePlane(f);
    f+= 4;

    // left
    f[0] = m3  + m[0];
    f[1] = m7  + m[4];
    f[2] = m11 + m[8];
    f[3] = m15 + m[12];
    normalizePlane(f);
    f+= 4;

    // top
    f[0] = m3  - m[1];
    f[1] = m7  - m[5];
    f[2] = m11 - m[9];
    f[3] = m15 - m[13];
    normalizePlane(f);
    f+= 4;

    // bottom
    f[0] = m3  + m[1];
    f[1] = m7  + m[5];
    f[2] = m11 + m[9];
    f[3] = m15 + m[13];
    normalizePlane(f);
    f+= 4;

    // far
    f[0] = m3  - m[2];
    f[1] = m7  - m[6];
    f[2] = m11 - m[10];
    f[3] = m15 - m[14];
    normalizePlane(f);
    f+= 4;

    // near
    f[0] = m3  + m[2];
    f[1] = m7  + m[6];
    f[2] = m11 + m[10];
    f[3] = m15 + m[14];
    normalizePlane(f);
}

static inline float dot3(float x0, float y0, float z0, float x1, float y1, float z1) {
    return x0 * x1 + y0 * y1 + z0 * z1;
}

static inline float signedDistance(const float* pPlane, float x, float y, float z) {
    return dot3(pPlane[0], pPlane[1], pPlane[2], x, y, z) + pPlane[3];
}

// Return true if the sphere intersects or is inside the frustum

static bool sphereHitsFrustum(const float* pFrustum, const float* pSphere) {
    float x = pSphere[0];
    float y = pSphere[1];
    float z = pSphere[2];
    float negRadius = -pSphere[3];
    for (int i = 0; i < 6; i++, pFrustum += 4) {
        if (signedDistance(pFrustum, x, y, z) <= negRadius) {
            return FALSE;
        }
    }
    return TRUE;
}

static void mx4transform(float x, float y, float z, float w, const float* pM, float* pDest) {
    pDest[0] = pM[0 + 4 * 0] * x + pM[0 + 4 * 1] * y + pM[0 + 4 * 2] * z + pM[0 + 4 * 3] * w;
    pDest[1] = pM[1 + 4 * 0] * x + pM[1 + 4 * 1] * y + pM[1 + 4 * 2] * z + pM[1 + 4 * 3] * w;
    pDest[2] = pM[2 + 4 * 0] * x + pM[2 + 4 * 1] * y + pM[2 + 4 * 2] * z + pM[2 + 4 * 3] * w;
    pDest[3] = pM[3 + 4 * 0] * x + pM[3 + 4 * 1] * y + pM[3 + 4 * 2] * z + pM[3 + 4 * 3] * w;
}

static
int visibilityTest(float* pWS, float* pPositions, int positionsLength,
        Char32* pIndices, int indexCount) {
    MallocHelper mallocHelper;
    int result = POLY_CLIP_OUT;
    float* pTransformed = 0;
    int transformedIndexCount = 0;

    if ( indexCount < 3 ) {
        return POLY_CLIP_OUT;
    }

    // Find out how many vertices we need to transform
    // We transform every vertex between the min and max indices, inclusive.
    // This is OK for the data sets we expect to use with this function, but
    // for other loads it might be better to use a more sophisticated vertex
    // cache of some sort.

    int minIndex = 65536;
    int maxIndex = -1;
    for(int i = 0; i < indexCount; i++) {
        int index = pIndices[i];
        if ( index < minIndex ) {
            minIndex = index;
        }
        if ( index > maxIndex ) {
            maxIndex = index;
        }
    }

    if ( maxIndex * 3 > positionsLength) {
        return -1;
    }

    transformedIndexCount = maxIndex - minIndex + 1;
    pTransformed = (float*) mallocHelper.alloc(transformedIndexCount * 4 * sizeof(float));

    if (pTransformed == 0 ) {
        return -2;
    }

    // Transform the vertices
    {
        const float* pSrc = pPositions + 3 * minIndex;
        float* pDst = pTransformed;
        for (int i = 0; i < transformedIndexCount; i++, pSrc += 3, pDst += 4) {
            mx4transform(pSrc[0], pSrc[1], pSrc[2], 1.0f, pWS,  pDst);
        }
    }

    // Clip the triangles

    Poly poly;
    float* pDest = & poly.vert[0].sx;
    for (int i = 0; i < indexCount; i += 3) {
        poly.n = 3;
        memcpy(pDest    , pTransformed + 4 * (pIndices[i    ] - minIndex), 4 * sizeof(float));
        memcpy(pDest + 4, pTransformed + 4 * (pIndices[i + 1] - minIndex), 4 * sizeof(float));
        memcpy(pDest + 8, pTransformed + 4 * (pIndices[i + 2] - minIndex), 4 * sizeof(float));
        result = poly_clip_to_frustum(&poly);
        if ( result != POLY_CLIP_OUT) {
            return result;
        }
    }

    return result;
}

ECode CVisibilityGL::VisibilityTest(
    /* [in] */ ArrayOf<Float>* ws_ref,
    /* [in] */ Int32 wsOffset,
    /* [in] */ ArrayOf<Float>* positions_ref,
    /* [in] */ Int32 positionsOffset,
    /* [in] */ ArrayOf<Char32>* indices_ref,
    /* [in] */ Int32 indicesOffset,
    /* [in] */ Int32 indexCount,
    /* [out] */ Int32* rst)
{

    FloatArrayHelper ws( ws_ref, wsOffset, 16);
    FloatArrayHelper positions(positions_ref, positionsOffset, 0);
    Char32ArrayHelper indices(indices_ref, indicesOffset, 0);

    Boolean wCheck, pCheck, iCheck;
    ws.Check(&wCheck);
    positions.Check(&pCheck);
    indices.Check(&iCheck);
    Boolean checkOK = wCheck && pCheck && iCheck;

    if (! checkOK) {
        // Return value will be ignored, because an exception has been thrown.
        *rst = -1;
        return NOERROR;
    }

    if (indices.mLength < indexCount) {
        *rst = -1;
        SLOGGERE("CVisibilityGL", "ength < offset + indexCount")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ws.Bind();
    positions.Bind();
    indices.Bind();

    return visibilityTest(ws.mData,
            positions.mData, positions.mLength,
            indices.mData, indexCount);
    return NOERROR;
}

ECode CVisibilityGL::FrustumCullSpheres(
    /* [in] */ ArrayOf<Float>* mvp_ref,
    /* [in] */ Int32 mvpOffset,
    /* [in] */ ArrayOf<Float>* spheres_ref,
    /* [in] */ Int32 spheresOffset,
    /* [in] */ Int32 spheresCount,
    /* [in] */ ArrayOf<Int32>* results_ref,
    /* [in] */ Int32 resultsOffset,
    /* [in] */ Int32 resultsCapacity,
    /* [out] */ Int32* rst)
{
    AutoPtr<ArrayOf<Float> > frustum = ArrayOf<Float>::Alloc(6*4);
    Int32 outputCount;
    Int32* pResults;
    Float* pSphere;
    FloatArrayHelper mvp(mvp_ref, mvpOffset, 16);
    FloatArrayHelper spheres(spheres_ref, spheresOffset, spheresCount * 4);
    Int32ArrayHelper results(results_ref, resultsOffset, resultsCapacity);

    Boolean mCheck, sCheck, rCheck;
    mvp.Check(&mCheck);
    spheres.Check(&sCheck);
    results.Check(&rCheck);
    Boolean initializedOK = mCheck && sCheck && rCheck;
        if (! initializedOK) {
        *rst = -1;
        return NOERROR;
    }

    mvp.Bind();
    spheres.Bind();
    results.Bind();

    computeFrustum(mvp.mData, frustum->GetPayload());

    // Cull the spheres

    pSphere = spheres.mData;
    pResults = results.mData;
    outputCount = 0;
    for(Int32 i = 0; i < spheresCount; i++, pSphere += 4) {
        if (sphereHitsFrustum(frustum->GetPayload(), pSphere)) {
            if (outputCount < resultsCapacity) {
                *pResults++ = i;
            }
            outputCount++;
        }
    }
    *rst = outputCount;
    return NOERROR;
}

ECode CVisibilityGL::ComputeBoundingSphere(
    /* [in] */ ArrayOf<Float>* positions_ref,
    /* [in] */ Int32 positionsOffset,
    /* [in] */ Int32 positionsCount,
    /* [in] */ ArrayOf<Float>* sphere_ref,
    /* [in] */ Int32 sphereOffset)
{
    FloatArrayHelper positions(positions_ref, positionsOffset, 0);
    FloatArrayHelper sphere(sphere_ref, sphereOffset, 4);

    Boolean pCheck;
    Boolean sCheck;
    positions.Check(&pCheck);
    sphere.Check(&sCheck);
    Boolean checkOK = pCheck && sCheck;
        if (! checkOK) {
        return NOERROR;
    }

    positions.Bind();
    sphere.Bind();

    if ( positionsCount < 1 ) {
        SLOGGERE("CVisibilityGL", "positionsCount < 1")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const Float* pSrc = positions.mData;

    // find bounding box
    Float x0 = *pSrc++;
    Float x1 = x0;
    Float y0 = *pSrc++;
    Float y1 = y0;
    Float z0 = *pSrc++;
    Float z1 = z0;

    for(Int32 i = 1; i < positionsCount; i++) {
        {
            Float x = *pSrc++;
            if (x < x0) {
                x0 = x;
            }
            else if (x > x1) {
                x1 = x;
            }
        }
        {
            Float y = *pSrc++;
            if (y < y0) {
                y0 = y;
            }
            else if (y > y1) {
                y1 = y;
            }
        }
        {
            Float z = *pSrc++;
            if (z < z0) {
                z0 = z;
            }
            else if (z > z1) {
                z1 = z;
            }
        }
    }

    // Because we know our input meshes fit pretty well into bounding boxes,
    // just take the diagonal of the box as defining our sphere.
    Float* pSphere = sphere.mData;
    Float dx = x1 - x0;
    Float dy = y1 - y0;
    Float dz = z1 - z0;
    *pSphere++ = x0 + dx * 0.5f;
    *pSphere++ = y0 + dy * 0.5f;
    *pSphere++ = z0 + dz * 0.5f;
    *pSphere++ = Distance(dx, dy, dz) * 0.5f;

    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
