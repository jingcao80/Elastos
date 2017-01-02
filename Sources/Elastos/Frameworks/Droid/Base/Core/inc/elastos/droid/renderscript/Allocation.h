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

#ifndef __ELASTOS_DROID_RENDERSCRIPT_ALLOCATION_H__
#define __ELASTOS_DROID_RENDERSCRIPT_ALLOCATION_H__

#include "elastos/droid/renderscript/BaseObj.h"
#include "elastos/droid/renderscript/Type.h"
#include "elastos/droid/renderscript/Element.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace RenderScript {

class Allocation
    : public BaseObj
    , public IAlloction
{
public:
    Allocation();

    virtual ~Allocation();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs,
        /* [in] */ Type* t,
        /* [in] */ Int32 usage);

    CARAPI GetElement(
        /* [out] */ IElement** element);

    /**
     * Get the usage flags of the Allocation.
     *
     * @return usage this Allocation's set of the USAGE_* flags OR'd together
     *
     */
    CARAPI GetUsage(
        /* [out] */ Int32* usage);

    /**
     * Get the size of the Allocation in bytes.
     *
     * @return size of the Allocation in bytes.
     *
     */
    CARAPI GetBytesSize(
        /* [out] */ Int32* bytesSize);

    /**
     * Get the {@link android.renderscript.Type} of the Allocation.
     *
     * @return Type
     *
     */
    CARAPI GetType(
        /* [out] */ IType** type);

    /**
     * Propagate changes from one usage of the Allocation to the
     * other usages of the Allocation.
     *
     */
    CARAPI SyncAll(
        /* [in] */ Int32 srcLocation);

    /**
     * Send a buffer to the output stream.  The contents of the Allocation will
     * be undefined after this operation. This operation is only valid if {@link
     * #USAGE_IO_OUTPUT} is set on the Allocation.
     *
     *
     */
    CARAPI IoSend();

    /**
     * Receive the latest input into the Allocation. This operation
     * is only valid if {@link #USAGE_IO_INPUT} is set on the Allocation.
     *
     */
    CARAPI IoReceive();

    /**
     * Copy an array of RS objects to the Allocation.
     *
     * @param d Source array.
     */
    CARAPI CopyFrom(
        /* [in] */ ArrayOf<IBaseObj*>* d);

    /**
     * Copy into this Allocation from an array. This method does not guarantee
     * that the Allocation is compatible with the input buffer; it copies memory
     * without reinterpretation.
     *
     * @param array The source data array
     */
    CARAPI CopyFromUnchecked(
        /* [in] */ IInterface* array);

    /**
     * Copy into this Allocation from an array. This method does not guarantee
     * that the Allocation is compatible with the input buffer; it copies memory
     * without reinterpretation.
     *
     * @param d the source data array
     */
    CARAPI CopyFromUnchecked(
        /* [in] */ ArrayOf<Int32>* d);

    /**
     * Copy into this Allocation from an array. This method does not guarantee
     * that the Allocation is compatible with the input buffer; it copies memory
     * without reinterpretation.
     *
     * @param d the source data array
     */
    CARAPI CopyFromUnchecked(
        /* [in] */ ArrayOf<Int16>* d);

    /**
     * Copy into this Allocation from an array. This method does not guarantee
     * that the Allocation is compatible with the input buffer; it copies memory
     * without reinterpretation.
     *
     * @param d the source data array
     */
    CARAPI CopyFromUnchecked(
        /* [in] */ ArrayOf<Byte>* d);

    /**
     * Copy into this Allocation from an array. This method does not guarantee
     * that the Allocation is compatible with the input buffer; it copies memory
     * without reinterpretation.
     *
     * @param d the source data array
     */
    CARAPI CopyFromUnchecked(
        /* [in] */ ArrayOf<Float>* d);

    /**
     * Copy into this Allocation from an array.  This variant is type checked
     * and will generate exceptions if the Allocation's {@link
     * android.renderscript.Element} does not match the array's
     * primitive type.
     *
     * @param array The source data array
     */
    CARAPI CopyFrom(
        /* [in] */ IInterface* array);

    /**
     * Copy into this Allocation from an array.  This variant is type checked
     * and will generate exceptions if the Allocation's {@link
     * android.renderscript.Element} is not a 32 bit integer type.
     *
     * @param d the source data array
     */
    CARAPI CopyFrom(
        /* [in] */ ArrayOf<Int32>* d);

    /**
     * Copy into this Allocation from an array.  This variant is type checked
     * and will generate exceptions if the Allocation's {@link
     * android.renderscript.Element} is not a 16 bit integer type.
     *
     * @param d the source data array
     */
    CARAPI CopyFrom(
        /* [in] */ ArrayOf<Int16>* d);

    /**
     * Copy into this Allocation from an array.  This variant is type checked
     * and will generate exceptions if the Allocation's {@link
     * android.renderscript.Element} is not an 8 bit integer type.
     *
     * @param d the source data array
     */
    CARAPI CopyFrom(
        /* [in] */ ArrayOf<Byte>* d);

    /**
     * Copy into this Allocation from an array.  This variant is type checked
     * and will generate exceptions if the Allocation's {@link
     * android.renderscript.Element} is not a 32 bit float type.
     *
     * @param d the source data array
     */
    CARAPI CopyFrom(
        /* [in] */ ArrayOf<Float>* d);

    /**
     * Copy into an Allocation from a {@link android.graphics.Bitmap}.  The
     * height, width, and format of the bitmap must match the existing
     * allocation.
     *
     * <p>If the {@link android.graphics.Bitmap} is the same as the {@link
     * android.graphics.Bitmap} used to create the Allocation with {@link
     * #createFromBitmap} and {@link #USAGE_SHARED} is set on the Allocation,
     * this will synchronize the Allocation with the latest data from the {@link
     * android.graphics.Bitmap}, potentially avoiding the actual copy.</p>
     *
     * @param b the source bitmap
     */
    CARAPI CopyFrom(
        /* [in] */ IBitmap* b);

    /**
     * Copy an Allocation from an Allocation.  The types of both allocations
     * must be identical.
     *
     * @param a the source allocation
     */
    CARAPI CopyFrom(
        /* [in] */ IAllocation* a);

    /**
     * This is only intended to be used by auto-generated code reflected from
     * the RenderScript script files and should not be used by developers.
     *
     * @param xoff
     * @param fp
     */
    CARAPI SetFromFieldPacker(
        /* [in] */ Int32 xoff,
        /* [in] */ IFieldPacker* fp);

    /**
     * This is only intended to be used by auto-generated code reflected from
     * the RenderScript script files.
     *
     * @param xoff
     * @param component_number
     * @param fp
     */
    CARAPI SetFromFieldPacker(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 component_number,
        /* [in] */ IFieldPacker* fp);

    /**
     * Generate a mipmap chain. This is only valid if the Type of the Allocation
     * includes mipmaps.
     *
     * <p>This function will generate a complete set of mipmaps from the top
     * level LOD and place them into the script memory space.</p>
     *
     * <p>If the Allocation is also using other memory spaces, a call to {@link
     * #syncAll syncAll(Allocation.USAGE_SCRIPT)} is required.</p>
     */
    CARAPI GenerateMipmaps();

    /**
     * Copy an array into part of this Allocation.  This method does not
     * guarantee that the Allocation is compatible with the input buffer.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param array The source data array
     */
    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ Interface* array);

    /**
     * Copy an array into part of this Allocation.  This method does not
     * guarantee that the Allocation is compatible with the input buffer.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* d);

    /**
     * Copy an array into part of this Allocation.  This method does not
     * guarantee that the Allocation is compatible with the input buffer.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int16>* d);

    /**
     * Copy an array into part of this Allocation.  This method does not
     * guarantee that the Allocation is compatible with the input buffer.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Byte>* d);

    /**
     * Copy an array into part of this Allocation.  This method does not
     * guarantee that the Allocation is compatible with the input buffer.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* d);


    /**
     * Copy an array into part of this Allocation.  This variant is type checked
     * and will generate exceptions if the Allocation type does not
     * match the component type of the array passed in.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param array The source data array.
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ Interface* array);

    /**
     * Copy an array into part of this Allocation.  This variant is type checked
     * and will generate exceptions if the Allocation type is not a 32 bit
     * integer type.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* d);

    /**
     * Copy an array into part of this Allocation.  This variant is type checked
     * and will generate exceptions if the Allocation type is not a 16 bit
     * integer type.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int16>* d);

    /**
     * Copy an array into part of this Allocation.  This variant is type checked
     * and will generate exceptions if the Allocation type is not an 8 bit
     * integer type.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Byte>* d);

    /**
     * Copy an array into part of this Allocation.  This variant is type checked
     * and will generate exceptions if the Allocation type is not a 32 bit float
     * type.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param d the source data array.
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* d);

     /**
     * Copy part of an Allocation into this Allocation.
     *
     * @param off The offset of the first element to be copied.
     * @param count The number of elements to be copied.
     * @param data the source data allocation.
     * @param dataOff off The offset of the first element in data to
     *          be copied.
     */
    CARAPI Copy1DRangeFrom(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ IAllocation* data,
        /* [in] */ Int32 dataOff);

    /**
     * Copy from an array into a rectangular region in this Allocation.  The
     * array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param array Data to be placed into the Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IInterface* array);

    /**
     * Copy from an array into a rectangular region in this Allocation.  The
     * array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param data to be placed into the Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * Copy from an array into a rectangular region in this Allocation.  The
     * array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param data to be placed into the Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ArrayOf<Int16>* data);

    /**
     * Copy from an array into a rectangular region in this Allocation.  The
     * array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param data to be placed into the Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ArrayOf<Int32>* data);

    /**
     * Copy from an array into a rectangular region in this Allocation.  The
     * array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param data to be placed into the Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ ArrayOf<Float>* data);

    /**
     * Copy a rectangular region from an Allocation into a rectangular region in
     * this Allocation.
     *
     * @param xoff X offset of the region in this Allocation
     * @param yoff Y offset of the region in this Allocation
     * @param w Width of the region to update.
     * @param h Height of the region to update.
     * @param data source Allocation.
     * @param dataXoff X offset in source Allocation
     * @param dataYoff Y offset in source Allocation
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IAllocation* data,
        /* [in] */ Int32 dataXoff,
        /* [in] */ Int32 dataYoff);

    /**
     * Copy a {@link android.graphics.Bitmap} into an Allocation.  The height
     * and width of the update will use the height and width of the {@link
     * android.graphics.Bitmap}.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param data the Bitmap to be copied
     */
    CARAPI Copy2DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ IBitmap* data);

    /**
     * @hide
     * Copy a rectangular region from the array into the allocation.
     * The array is assumed to be tightly packed.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param zoff Z offset of the region to update in this Allocation
     * @param w Width of the region to update
     * @param h Height of the region to update
     * @param d Depth of the region to update
     * @param data to be placed into the allocation
     */
    CARAPI Copy3DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 zoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 d,
        /* [in] */ IInterface* array);

    /**
     * @hide
     * Copy a rectangular region into the allocation from another
     * allocation.
     *
     * @param xoff X offset of the region to update in this Allocation
     * @param yoff Y offset of the region to update in this Allocation
     * @param zoff Z offset of the region to update in this Allocation
     * @param w Width of the region to update.
     * @param h Height of the region to update.
     * @param d Depth of the region to update.
     * @param data source allocation.
     * @param dataXoff X offset of the region in the source Allocation
     * @param dataYoff Y offset of the region in the source Allocation
     * @param dataZoff Z offset of the region in the source Allocation
     */
    CARAPI Copy3DRangeFrom(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 zoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 d,
        /* [in] */ IAllocation* data,
        /* [in] */ Int32 dataXoff,
        /* [in] */ Int32 dataYoff,
        /* [in] */ Int32 dataZoff);

    /**
     * Copy from the Allocation into a {@link android.graphics.Bitmap}.  The
     * bitmap must match the dimensions of the Allocation.
     *
     * @param b The bitmap to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ IBitmap* b);

    /**
     * Copy from the Allocation into an array.  The array must be at
     * least as large as the Allocation.  The
     * {@link android.renderscript.Element} must match the component
     * type of the array passed in.
     *
     * @param array The array to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ IInterface* array);

    /**
     * Copy from the Allocation into a byte array.  The array must be at least
     * as large as the Allocation.  The allocation must be of an 8 bit integer
     * {@link android.renderscript.Element} type.
     *
     * @param d The array to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ ArrayOf<Byte>* d);

    /**
     * Copy from the Allocation into a short array.  The array must be at least
     * as large as the Allocation.  The allocation must be of an 16 bit integer
     * {@link android.renderscript.Element} type.
     *
     * @param d The array to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ ArrayOf<Int16>* d);

    /**
     * Copy from the Allocation into a int array.  The array must be at least as
     * large as the Allocation.  The allocation must be of an 32 bit integer
     * {@link android.renderscript.Element} type.
     *
     * @param d The array to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ ArrayOf<Int32>* d);

    /**
     * Copy from the Allocation into a float array.  The array must be at least
     * as large as the Allocation.  The allocation must be of an 32 bit float
     * {@link android.renderscript.Element} type.
     *
     * @param d The array to be set from the Allocation.
     */
    CARAPI CopyTo(
        /* [in] */ ArrayOf<Float>* d);

    /**
     * Resize a 1D allocation.  The contents of the allocation are preserved.
     * If new elements are allocated objects are created with null contents and
     * the new region is otherwise undefined.
     *
     * <p>If the new region is smaller the references of any objects outside the
     * new region will be released.</p>
     *
     * <p>A new type will be created with the new dimension.</p>
     *
     * @param dimX The new size of the allocation.
     *
     * @deprecated RenderScript objects should be immutable once created.  The
     * replacement is to create a new allocation and copy the contents. This
     * function will throw an exception if API 21 or higher is used.
     */
    CARAPI Resize(
        /* [in] */ Int32 dimX);

    /**
     * Returns the handle to a raw buffer that is being managed by the screen
     * compositor. This operation is only valid for Allocations with {@link
     * #USAGE_IO_INPUT}.
     *
     * @return Surface object associated with allocation
     *
     */
    CARAPI GetSurface(
        /* [out] */ ISurface** surface);

    /**
     * Associate a {@link android.view.Surface} with this Allocation. This
     * operation is only valid for Allocations with {@link #USAGE_IO_OUTPUT}.
     *
     * @param sur Surface to associate with allocation
     */
    CARAPI SetSurface(
        /* [in] */ ISurface* sur);

    /**
     * Set a notification handler for {@link #USAGE_IO_INPUT}.
     *
     * @param callback instance of the OnBufferAvailableListener
     *                 class to be called when buffer arrive.
     */
    CARAPI SetOnBufferAvailableListener(
        /* [in] */ IOnBufferAvailableListener* cb);

    /**
     * Creates a new Allocation with the given {@link
     * android.renderscript.Type}, mipmap flag, and usage flags.
     *
     * @param type RenderScript type describing data layout
     * @param mips specifies desired mipmap behaviour for the
     *             allocation
     * @param usage bit field specifying how the Allocation is
     *              utilized
     */
    static CARAPI CreateTyped(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IType* type,
        /* [in] */ AllocationMipmapControl mips,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation with the size specified by the type and no mipmaps
     * generated by default
     *
     * @param rs Context to which the allocation will belong.
     * @param type renderscript type describing data layout
     * @param usage bit field specifying how the allocation is
     *              utilized
     *
     * @return allocation
     */
    static CARAPI CreateTyped(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IType* type,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation for use by scripts with a given {@link
     * android.renderscript.Type} and no mipmaps
     *
     * @param rs Context to which the Allocation will belong.
     * @param type RenderScript Type describing data layout
     *
     * @return allocation
     */
    static CARAPI CreateTyped(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IType* type,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation with a specified number of given elements
     *
     * @param rs Context to which the Allocation will belong.
     * @param e Element to use in the Allocation
     * @param count the number of Elements in the Allocation
     * @param usage bit field specifying how the Allocation is
     *              utilized
     *
     * @return allocation
     */
    static CARAPI CreateSized(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IElement* e,
        /* [in] */ Int32 count,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation with a specified number of given elements
     *
     * @param rs Context to which the Allocation will belong.
     * @param e Element to use in the Allocation
     * @param count the number of Elements in the Allocation
     *
     * @return allocation
     */
    static CARAPI CreateSized(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IElement* e,
        /* [in] */ Int32 count,
        /* [out] */ IAllocation** allocation);

    static CARAPI ElementFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [out] */ IElement** e);

    static CARAPI TypeFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [in] */ AllocationMipmapControl mip,
        /* [out] */ IType** t);

    /**
     * Creates an Allocation from a {@link android.graphics.Bitmap}.
     *
     * @param rs Context to which the allocation will belong.
     * @param b Bitmap source for the allocation data
     * @param mips specifies desired mipmap behaviour for the
     *             allocation
     * @param usage bit field specifying how the allocation is
     *              utilized
     *
     * @return Allocation containing bitmap data
     *
     */
    static CARAPI CreateFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [in] */ AllocationMipmapControl mip,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation from a {@link android.graphics.Bitmap}.
     *
     * <p>With target API version 18 or greater, this Allocation will be created
     * with {@link #USAGE_SHARED}, {@link #USAGE_SCRIPT}, and {@link
     * #USAGE_GRAPHICS_TEXTURE}. With target API version 17 or lower, this
     * Allocation will be created with {@link #USAGE_GRAPHICS_TEXTURE}.</p>
     *
     * @param rs Context to which the allocation will belong.
     * @param b bitmap source for the allocation data
     *
     * @return Allocation containing bitmap data
     *
     */
    static CARAPI CreateFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates a cubemap Allocation from a {@link android.graphics.Bitmap}
     * containing the horizontal list of cube faces. Each face must be a square,
     * have the same size as all other faces, and have a width that is a power
     * of 2.
     *
     * @param rs Context to which the allocation will belong.
     * @param b Bitmap with cubemap faces layed out in the following
     *          format: right, left, top, bottom, front, back
     * @param mips specifies desired mipmap behaviour for the cubemap
     * @param usage bit field specifying how the cubemap is utilized
     *
     * @return allocation containing cubemap data
     *
     */
    static CARAPI CreateCubemapFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [in] */ AllocationMipmapControl mips,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates a non-mipmapped cubemap Allocation for use as a graphics texture
     * from a {@link android.graphics.Bitmap} containing the horizontal list of
     * cube faces. Each face must be a square, have the same size as all other
     * faces, and have a width that is a power of 2.
     *
     * @param rs Context to which the allocation will belong.
     * @param b bitmap with cubemap faces layed out in the following
     *          format: right, left, top, bottom, front, back
     *
     * @return allocation containing cubemap data
     *
     */
    static CARAPI CreateCubemapFromBitmap(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* b,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates a cubemap Allocation from 6 {@link android.graphics.Bitmap}
     * objects containing the cube faces. Each face must be a square, have the
     * same size as all other faces, and have a width that is a power of 2.
     *
     * @param rs Context to which the allocation will belong.
     * @param xpos cubemap face in the positive x direction
     * @param xneg cubemap face in the negative x direction
     * @param ypos cubemap face in the positive y direction
     * @param yneg cubemap face in the negative y direction
     * @param zpos cubemap face in the positive z direction
     * @param zneg cubemap face in the negative z direction
     * @param mips specifies desired mipmap behaviour for the cubemap
     * @param usage bit field specifying how the cubemap is utilized
     *
     * @return allocation containing cubemap data
     *
     */
    static CARAPI CreateCubemapFromCubeFaces(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* xpos,
        /* [in] */ IBitmap* xneg,
        /* [in] */ IBitmap* ypos,
        /* [in] */ IBitmap* yneg,
        /* [in] */ IBitmap* zpos,
        /* [in] */ IBitmap* zneg,
        /* [in] */ AllocationMipmapControl mips,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates a non-mipmapped cubemap Allocation for use as a sampler input
     * from 6 {@link android.graphics.Bitmap} objects containing the cube
     * faces. Each face must be a square, have the same size as all other faces,
     * and have a width that is a power of 2.
     *
     * @param rs Context to which the allocation will belong.
     * @param xpos cubemap face in the positive x direction
     * @param xneg cubemap face in the negative x direction
     * @param ypos cubemap face in the positive y direction
     * @param yneg cubemap face in the negative y direction
     * @param zpos cubemap face in the positive z direction
     * @param zneg cubemap face in the negative z direction
     *
     * @return allocation containing cubemap data
     *
     */
    static CARAPI CreateCubemapFromCubeFaces(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IBitmap* xpos,
        /* [in] */ IBitmap* xneg,
        /* [in] */ IBitmap* ypos,
        /* [in] */ IBitmap* yneg,
        /* [in] */ IBitmap* zpos,
        /* [in] */ IBitmap* zneg,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation from the Bitmap referenced
     * by resource ID.
     *
     * @param rs Context to which the allocation will belong.
     * @param res application resources
     * @param id resource id to load the data from
     * @param mips specifies desired mipmap behaviour for the
     *             allocation
     * @param usage bit field specifying how the allocation is
     *              utilized
     *
     * @return Allocation containing resource data
     *
     */
    static CARAPI CreateFromBitmapResource(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IResources* res,
        /* [in] */ Int32 id,
        /* [in] */ AllocationMipmapControl mips,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates a non-mipmapped Allocation to use as a graphics texture from the
     * {@link android.graphics.Bitmap} referenced by resource ID.
     *
     * <p>With target API version 18 or greater, this allocation will be created
     * with {@link #USAGE_SCRIPT} and {@link #USAGE_GRAPHICS_TEXTURE}. With
     * target API version 17 or lower, this allocation will be created with
     * {@link #USAGE_GRAPHICS_TEXTURE}.</p>
     *
     * @param rs Context to which the allocation will belong.
     * @param res application resources
     * @param id resource id to load the data from
     *
     * @return Allocation containing resource data
     *
     */
    static CARAPI CreateFromBitmapResource(
        /* [in] */ IRenderScript* rs,
        /* [in] */ IResources* res,
        /* [in] */ Int32 id,
        /* [out] */ IAllocation** allocation);

    /**
     * Creates an Allocation containing string data encoded in UTF-8 format.
     *
     * @param rs Context to which the allocation will belong.
     * @param str string to create the allocation from
     * @param usage bit field specifying how the allocaiton is
     *              utilized
     *
     */
    static CARAPI CreateFromString(
        /* [in] */ IRenderScript* rs,
        /* [in] */ String str,
        /* [in] */ Int32 usage,
        /* [out] */ IAllocation** allocation);

protected:
    CARAPI_(void) UpdateFromNative();

private:
    CARAPI ValidateObjectIsPrimitiveArray(
        /* [in] */ IInterface* d,
        /* [in] */ Boolean checkType,
        /* [out] */ Element::DataType** type);

    CARAPI GetIDSafe(
        /* [out] */ Int64* id);

    CARAPI_(void) UpdateCacheInfo(
        /* [in] */ IType* t);

    CARAPI_(void) SetBitmap(
        /* [in] */ IBitmap* b);

    CARAPI ValidateIsInt64();

    CARAPI ValidateIsInt32();

    CARAPI ValidateIsInt16();

    CARAPI ValidateIsInt8();

    CARAPI ValidateIsFloat32();

    CARAPI ValidateIsFloat64();

    CARAPI ValidateIsObject();

    CARAPI ValidateBitmapFormat(
        /* [in] */ IBitmap* b);

    CARAPI ValidateBitmapSize(
        /* [in] */ IBitmap* b);

    CARAPI CopyFromUnchecked(
        /* [in] */ IInterface* array,
        /* [in] */ Element::DataType* dt,
        /* [in] */ Int32 arrayLen);

    CARAPI Data1DChecks(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ Int32 len,
        /* [in] */ Int32 dataSize);

    CARAPI Copy1DRangeFromUnchecked(
        /* [in] */ Int32 off,
        /* [in] */ Int32 count,
        /* [in] */ IInterface* array,
        /* [in] */ Element::DataType* dt,
        /* [in] */ Int32 arrayLen);

    CARAPI Validate2DRange(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI Copy2DRangeFromUnchecked(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h, Object array,
        /* [in] */ Element::DataType* dt,
        /* [in] */ Int32 arrayLen);

    CARAPI Validate3DRange(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 zoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 d);

    CARAPI Copy3DRangeFromUnchecked(
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 zoff,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 d,
        /* [in] */ IInterface* array,
        /* [in] */ Element::DataType* dt,
        /* [in] */ Int32 arrayLen);

    CARAPI CopyTo(
        /* [in] */ IInterface* array,
        /* [in] */ Element::DataType* dt,
        /* [in] */ Int32 arrayLen);

    static CARAPI_(void) SendBufferNotification(
        /* [in] */ Int64 id);

private:
    AutoPtr<Type> mType;
    AutoPtr<IBitmap> mBitmap;
    Int32 mUsage;
    AutoPtr<Allocation> mAdaptedAllocation;
    Int32 mSize;

    Boolean mConstrainedLOD;
    Boolean mConstrainedFace;
    Boolean mConstrainedY;
    Boolean mConstrainedZ;
    Boolean mReadAllowed;
    Boolean mWriteAllowed;
    Int32 mSelectedY;
    Int32 mSelectedZ;
    Int32 mSelectedLOD;
    TypeCubemapFace mSelectedFace;

    Int32 mCurrentDimX;
    Int32 mCurrentDimY;
    Int32 mCurrentDimZ;
    Int32 mCurrentCount;
    static HashMap<Int64, AutoPtr<Allocation> > sAllocationMap;
    static Object sAllocationMapLock;
    AutoPtr<IOnBufferAvailableListener> mBufferNotifier;

    // creation
    static AutoPtr<IBitmapFactoryOptions> sBitmapOptions;
};

} // namespace RenderScript
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_RENDERSCRIPT_ALLOCATION_H__
