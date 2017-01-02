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

#include "elastos/droid/view/HardwareLayer.h"
#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/internal/utility/CVirtualRefBasePtr.h"

#include <gui/GLConsumer.h>

#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkMatrix.h>
#include <SkXfermode.h>

#include <DeferredLayerUpdater.h>
#include <LayerRenderer.h>
#include <SkiaShader.h>
#include <Rect.h>
#include <RenderNode.h>

#include "Elastos.Droid.View.h"

using Elastos::Droid::Graphics::Matrix;
using Elastos::Droid::Graphics::Paint;
using Elastos::Droid::Graphics::NativePaint;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Internal::Utility::CVirtualRefBasePtr;
using Elastos::Droid::View::IHardwareLayer;

using namespace android;
using namespace android::uirenderer;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(HardwareLayer, Object, IHardwareLayer)

/**
 * Update the paint used when drawing this layer.
 *
 * @param paint The paint used when the layer is drawn into the destination canvas.
 * @see View#setLayerPaint(android.graphics.Paint)
 */
ECode HardwareLayer::SetLayerPaint(
    /* [in] */ IPaint* paint)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    Paint* p = (Paint*)paint;
    nSetLayerPaint(ptr, p->mNativePaint);
    mRenderer->PushLayerUpdate(this);
    return NOERROR;
}

/**
 * Indicates whether this layer can be rendered.
 *
 * @return True if the layer can be rendered into, false otherwise
 */
ECode HardwareLayer::IsValid(
    /* [out] */ Boolean* valid)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    *valid = mFinalizer != NULL && ptr != 0;
    return NOERROR;
}

/**
 * Destroys resources without waiting for a GC.
 */
ECode HardwareLayer::Destory()
{
    Boolean valid;
    if (IsValid(&valid), !valid) {
        // Already destroyed
        return NOERROR;
    }
    mRenderer->OnLayerDestroyed(this);
    mRenderer = NULL;
    mFinalizer->ReleasePtr();
    mFinalizer = NULL;
    return NOERROR;
}

ECode HardwareLayer::GetDeferredLayerUpdater(
    /* [out] */ Int64* result)
{
    return mFinalizer->Get(result);
}

/**
 * Copies this layer into the specified bitmap.
 *
 * @param bitmap The bitmap to copy they layer into
 *
 * @return True if the copy was successful, false otherwise
 */
ECode HardwareLayer::CopyInto(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ Boolean* result)
{
    return mRenderer->CopyLayerInto(this, bitmap, result);
}

/**
 * Update the layer's properties. Note that after calling this isValid() may
 * return false if the requested width/height cannot be satisfied
 *
 * @param width The new width of this layer
 * @param height The new height of this layer
 * @param isOpaque Whether this layer is opaque
 *
 * @return true if the layer's properties will change, false if they already
 *         match the desired values.
 */
ECode HardwareLayer::Prepare(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean isOpaque,
    /* [out] */ Boolean* result)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    return nPrepare(ptr, width, height, isOpaque, result);
}

/**
 * Sets an optional transform on this layer.
 *
 * @param matrix The transform to apply to the layer.
 */
ECode HardwareLayer::SetTransform(
    /* [in] */ IMatrix* matrix)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    Matrix* m = (Matrix*)matrix;
    nSetTransform(ptr, m->mNativeMatrix);
    return mRenderer->PushLayerUpdate(this);
}

/**
 * Indicates that this layer has lost its texture.
 */
ECode HardwareLayer::DetachSurfaceTexture()
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    return mRenderer->DetachSurfaceTexture(ptr);
}

ECode HardwareLayer::GetLayer(
    /* [out] */ Int64* result)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    return nGetLayer(ptr, result);
}

ECode HardwareLayer::SetSurfaceTexture(
    /* [in] */ ISurfaceTexture* surface)
{
    Int64 ptr;
    mFinalizer->Get(&ptr);
    nSetSurfaceTexture(ptr, surface, FALSE);
    return NOERROR;
}

ECode HardwareLayer::UpdateSurfaceTexture()
{
    return mRenderer->PushLayerUpdate(this);;
}

AutoPtr<IHardwareLayer> HardwareLayer::AdoptTextureLayer(
    /* [in] */ IHardwareRenderer* renderer,
    /* [in] */ Int64 deferredUpdater)
{
    return new HardwareLayer(renderer, deferredUpdater);
}

HardwareLayer::HardwareLayer(
    /* [in] */ IHardwareRenderer* renderer,
    /* [in] */ Int64 deferredUpdater)
{
    if (renderer == NULL || deferredUpdater == 0) {
        // throw new IllegalArgumentException("Either hardware renderer: " + renderer
        //         + " or deferredUpdater: " + deferredUpdater + " is invalid");
        assert(0);
    }
    mRenderer = renderer;
    CVirtualRefBasePtr::New(deferredUpdater, (IVirtualRefBasePtr**)&mFinalizer);
}

ECode HardwareLayer::nPrepare(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean isOpaque,
    /* [out] */ Boolean* res)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    Boolean changed = FALSE;
    changed |= layer->setSize(width, height);
    changed |= layer->setBlend(!isOpaque);
    *res = changed;
    return NOERROR;
}

ECode HardwareLayer::nSetLayerPaint(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [in] */ Int64 paintPtr)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    if (layer) {
        NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
        layer->setPaint(paint);
    }
    return NOERROR;
}

ECode HardwareLayer::nSetTransform(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [in] */ Int64 matrixPtr)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    layer->setTransform(matrix);
    return NOERROR;
}

ECode HardwareLayer::nSetSurfaceTexture(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [in] */ ISurfaceTexture* surface,
    /* [in] */ Boolean isAlreadyAttached)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    CSurfaceTexture* texture = (CSurfaceTexture*)surface;
    sp<GLConsumer> surfaceTexture((GLConsumer*)texture->mSurfaceTexture);
    layer->setSurfaceTexture(surfaceTexture, !isAlreadyAttached);
    return NOERROR;
}

ECode HardwareLayer::nUpdateSurfaceTexture(
    /* [in] */ Int64 layerUpdaterPtr)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    layer->updateTexImage();
    return NOERROR;
}

ECode HardwareLayer::nUpdateRenderLayer(
    /* [in] */ Int64 layerUpdater,
    /* [in] */ Int64 displayList,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode HardwareLayer::nGetLayer(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [out] */ Int64* res)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    *res = reinterpret_cast<Int64>( layer->backingLayer() );
    return NOERROR;
}

ECode HardwareLayer::nGetTexName(
    /* [in] */ Int64 layerUpdaterPtr,
    /* [out] */ Int32* res)
{
    DeferredLayerUpdater* layer = reinterpret_cast<DeferredLayerUpdater*>(layerUpdaterPtr);
    *res = layer->backingLayer()->getTexture();
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos