
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CSPARSERECTFARRAY_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CSPARSERECTFARRAY_H__

#include "_Elastos_Droid_View_InputMethod_CSparseRectFArray.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRectF;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

/**
 * An implementation of SparseArray specialized for {@link android.graphics.RectF}.
 * <p>
 * As this is a sparse array, it represents an array of {@link RectF} most of which are null. This
 * class could be in some other packages like android.graphics or android.util but currently
 * belong to android.view.inputmethod because this class is hidden and used only in input method
 * framework.
 * </p>
 * @hide
 */
CarClass(CSparseRectFArray)
    , public Object
    , public ISparseRectFArray
    , public IParcelable
{
public:
    /**
     * Builder for {@link SparseRectFArray}. This class is not designed to be thread-safe.
     * @hide
     */
    class SparseRectFArrayBuilder
        : public Object
        , public ISparseRectFArrayBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        SparseRectFArrayBuilder();

        /**
         * Puts the rectangle with an integer key.
         * @param key the key to be associated with the rectangle. It must be greater than all
         * existing keys that have been previously specified.
         * @param left left of the rectangle.
         * @param top top of the rectangle.
         * @param right right of the rectangle.
         * @param bottom bottom of the rectangle.
         * @param flags an arbitrary integer value to be associated with this rectangle.
         * @return the receiver object itself for chaining method calls.
         * @throws IllegalArgumentException If the index is not greater than all of existing keys.
         */
        CARAPI Append(
            /* [in] */ Int32 key,
            /* [in] */ Float left,
            /* [in] */ Float top,
            /* [in] */ Float right,
            /* [in] */ Float bottom,
            /* [in] */ Int32 flags);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        /**
         * @return {@link SparseRectFArray} using parameters in this {@link SparseRectFArray}.
         */
        CARAPI Build(
            /* [out] */ ISparseRectFArray** result);

        CARAPI Reset();

    private:
        /**
         * Throws {@link IllegalArgumentException} to make sure that this class is correctly used.
         * @param key key to be checked.
         */
        CARAPI_(void) CheckIndex(
            /* [in] */ Int32 key);

        /**
         * Extends the internal array if necessary.
         */
        CARAPI_(void) EnsureBufferSize();

    public:
        Int32 mCount;
        AutoPtr<ArrayOf<Int32> > mKeys;
        AutoPtr<ArrayOf<Float> > mCoordinates;
        AutoPtr<ArrayOf<Int32> > mFlagsArray;
        static const Int32 INITIAL_SIZE;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ SparseRectFArrayBuilder* builder);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Used to package this object into a {@link Parcel}.
     *
     * @param dest The {@link Parcel} to be written.
     * @param flags The flags used for parceling.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetHashCode(
        /* [out] */ Int32* hc);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ IRectF** result);

    CARAPI GetFlags(
        /* [in] */ Int32 index,
        /* [in] */ Int32 valueIfKeyNotFound,
        /* [out] */ Int32* result);

private:
    /**
     * The keys, in ascending order, of those {@link RectF} that are not null. For example,
     * {@code [null, null, null, Rect1, null, Rect2]} would be represented by {@code [3,5]}.
     * @see #mCoordinates
     */
    AutoPtr<ArrayOf<Int32> > mKeys;

    /**
     * Stores coordinates of the rectangles, in the order of
     * {@code rects[mKeys[0]].left}, {@code rects[mKeys[0]].top},
     * {@code rects[mKeys[0]].right}, {@code rects[mKeys[0]].bottom},
     * {@code rects[mKeys[1]].left}, {@code rects[mKeys[1]].top},
     * {@code rects[mKeys[1]].right}, {@code rects[mKeys[1]].bottom},
     * {@code rects[mKeys[2]].left}, {@code rects[mKeys[2]].top}, ....
     */
    AutoPtr<ArrayOf<Float> > mCoordinates;

    /**
     * Stores visibility information.
     */
    AutoPtr<ArrayOf<Int32> > mFlagsArray;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_VIEW_INPUTMETHOD_CSPARSERECTFARRAY_H__
