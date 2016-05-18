
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHOLDER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHOLDER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.View.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class RequestHolder
    : public Object
    , public IRequestHolder
{
public:
    class Builder
        : public Object
        , public IRequestHolderBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();

        virtual ~Builder() {}

        CARAPI constructor();

        /**
         * Construct a new {@link Builder} to generate {@link RequestHolder} objects.
         *
         * @param requestId the ID to set in {@link RequestHolder} objects.
         * @param subsequenceId the sequence ID to set in {@link RequestHolder} objects.
         * @param request the original {@link CaptureRequest} to set in {@link RequestHolder}
         *                objects.
         * @param repeating {@code true} if the request is repeating.
         */
        CARAPI constructor(
            /* [in] */ Int32 requestId,
            /* [in] */ Int32 subsequenceId,
            /* [in] */ ICaptureRequest* request,
            /* [in] */ Boolean repeating,
            /* [in] */ ICollection* jpegSurfaceIds);

        /**
         * Build a new {@link RequestHolder} using with parameters generated from this
         *      {@link Builder}.
         *
         * @param frameNumber the {@code framenumber} to generate in the {@link RequestHolder}.
         * @return a {@link RequestHolder} constructed with the {@link Builder}'s parameters.
         */
        CARAPI Build(
            /* [in] */ Int64 frameNumber,
            /* [out] */ IRequestHolder** holder);

    private:
        /**
         * Returns true if the given surface requires jpeg buffers.
         *
         * @param s a {@link android.view.Surface} to check.
         * @return true if the surface requires a jpeg buffer.
         */
        CARAPI JpegType(
            /* [in] */ ISurface* s,
            /* [out] */ Boolean* result);

        /**
         * Returns true if the given surface requires non-jpeg buffer types.
         *
         * <p>
         * "Jpeg buffer" refers to the buffers returned in the jpeg
         * {@link android.hardware.Camera.PictureCallback}.  Non-jpeg buffers are created using a tee
         * of the preview stream drawn to the surface
         * set via {@link android.hardware.Camera#setPreviewDisplay(android.view.SurfaceHolder)} or
         * equivalent methods.
         * </p>
         * @param s a {@link android.view.Surface} to check.
         * @return true if the surface requires a non-jpeg buffer type.
         */
        CARAPI PreviewType(
            /* [in] */ ISurface* s,
            /* [out] */ Boolean* result);

        /**
         * Returns the number of surfaces targeted by the request that require jpeg buffers.
         */
        CARAPI NumJpegTargets(
            /* [in] */ ICaptureRequest* request,
            /* [out] */ Int32* num);

        /**
         * Returns the number of surfaces targeted by the request that require non-jpeg buffers.
         */
        CARAPI NumPreviewTargets(
            /* [in] */ ICaptureRequest* request,
            /* [out] */ Int32* num);

    private:
        static const String TAG;

        Int32 mRequestId;
        Int32 mSubsequenceId;
        AutoPtr<ICaptureRequest> mRequest;
        Boolean mRepeating;
        Int32 mNumJpegTargets;
        Int32 mNumPreviewTargets;
        AutoPtr<ICollection> mJpegSurfaceIds;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~RequestHolder() {}

    /**
     * Return the request id for the contained {@link CaptureRequest}.
     */
    CARAPI GetRequestId(
        /* [out] */ Int32* value);

    /**
     * Returns true if the contained request is repeating.
     */
    CARAPI IsRepeating(
        /* [out] */ Boolean* value);

    /**
     * Return the subsequence id for this request.
     */
    CARAPI GetSubsequeceId(
        /* [out] */ Int32* value);

    /**
     * Returns the frame number for this request.
     */
    CARAPI GetFrameNumber(
        /* [out] */ Int64* value);

    /**
     * Returns the contained request.
     */
    CARAPI GetRequest(
        /* [out] */ ICaptureRequest** outcr);

    /**
     * Returns a read-only collection of the surfaces targeted by the contained request.
     */
    CARAPI GetHolderTargets(
        /* [out] */ ICollection** outcol);

    /**
     * Returns true if any of the surfaces targeted by the contained request require jpeg buffers.
     */
    CARAPI HasJpegTargets(
        /* [out] */ Boolean* value);

    /**
     * Returns true if any of the surfaces targeted by the contained request require a
     * non-jpeg buffer type.
     */
    CARAPI HasPreviewTargets(
        /* [out] */ Boolean* value);

    /**
     * Return the number of jpeg-type surfaces targeted by this request.
     */
    CARAPI NumJpegTargets(
        /* [out] */ Int32* value);

    /**
     * Return the number of non-jpeg-type surfaces targeted by this request.
     */
    CARAPI NumPreviewTargets(
        /* [out] */ Int32* value);

    /**
     * Mark this request as failed.
     */
    CARAPI FailRequest();

    /**
     * Return {@code true} if this request failed.
     */
    CARAPI RequestFailed(
        /* [out] */ Boolean* value);

private:
    friend class RequestHolderBuilder;

    RequestHolder(
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 subsequenceId,
        /* [in] */ ICaptureRequest* request,
        /* [in] */ Boolean repeating,
        /* [in] */ Int64 frameNumber,
        /* [in] */ Int32 numJpegTargets,
        /* [in] */ Int32 numPreviewTargets);

private:
    static const String TAG;

    Boolean mRepeating;
    AutoPtr<ICaptureRequest> mRequest;
    Int32 mRequestId;
    Int32 mSubsequeceId;
    Int64 mFrameNumber;
    Int32 mNumJpegTargets;
    Int32 mNumPreviewTargets;
    /*volatile*/ Boolean mFailed;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHOLDER_H__