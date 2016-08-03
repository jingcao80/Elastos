
#ifndef __ELASTOS_DROID_INCALLUI_CVIDEOCALLFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_CVIDEOCALLFRAGMENT_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_InCallUI_CVideoCallFragment.h"
#include "elastos/droid/incallui/BaseFragment.h"

using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::View::ISurfaceTextureListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::ITextureView;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IOnGlobalLayoutListener;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CVideoCallFragment)
    , public BaseFragment
    , public IUi
    , public IVideoCallUi
{
private:
    /**
     * Inner-class representing a {@link TextureView} and its associated {@link SurfaceTexture} and
     * {@link Surface}.  Used to manage the lifecycle of these objects across device orientation
     * changes.
     */
    class VideoCallSurface
        : public Object
        , public ISurfaceTextureListener
        , public IViewOnClickListener
    {
    public:
        /**
         * Creates an instance of a {@link VideoCallSurface}.
         *
         * @param surfaceId The surface ID of the surface.
         * @param textureView The {@link TextureView} for the surface.
         */
        VideoCallSurface(
            /* [in] */ Int32 surfaceId,
            /* [in] */ ITextureView* textureView,
            /* [in] */ CVideoCallFragment* host);

        /**
         * Creates an instance of a {@link VideoCallSurface}.
         *
         * @param surfaceId The surface ID of the surface.
         * @param textureView The {@link TextureView} for the surface.
         * @param width The width of the surface.
         * @param height The height of the surface.
         */
        VideoCallSurface(
            /* [in] */ Int32 surfaceId,
            /* [in] */ ITextureView* textureView,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ CVideoCallFragment* host);

        CAR_INTERFACE_DECL()

        /**
         * Recreates a {@link VideoCallSurface} after a device orientation change.  Re-applies the
         * saved {@link SurfaceTexture} to the
         *
         * @param view The {@link TextureView}.
         */
        CARAPI_(void) RecreateView(
            /* [in] */ ITextureView* view);

        /**
         * Handles {@link SurfaceTexture} callback to indicate that a {@link SurfaceTexture} has
         * been successfully created.
         *
         * @param surfaceTexture The {@link SurfaceTexture} which has been created.
         * @param width The width of the {@link SurfaceTexture}.
         * @param height The height of the {@link SurfaceTexture}.
         */
        // @Override
        CARAPI OnSurfaceTextureAvailable(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Handles a change in the {@link SurfaceTexture}'s size.
         *
         * @param surfaceTexture The {@link SurfaceTexture}.
         * @param width The new width.
         * @param height The new height.
         */
        // @Override
        CARAPI OnSurfaceTextureSizeChanged(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Handles {@link SurfaceTexture} destruct callback, indicating that it has been destroyed.
         *
         * @param surfaceTexture The {@link SurfaceTexture}.
         * @return {@code True} if the {@link TextureView} can release the {@link SurfaceTexture}.
         */
        // @Override
        CARAPI OnSurfaceTextureDestroyed(
            /* [in] */ ISurfaceTexture* surfaceTexture,
            /* [out] */ Boolean* result);

        /**
         * Handles {@link SurfaceTexture} update callback.
         * @param surface
         */
        // @Override
        CARAPI OnSurfaceTextureUpdated(
            /* [in] */ ISurfaceTexture* surface);

        /**
         * Retrieves the current {@link TextureView}.
         *
         * @return The {@link TextureView}.
         */
        CARAPI_(AutoPtr<ITextureView>) GetTextureView();

        /**
         * Called by the user presenter to indicate that the surface is no longer required due to a
         * change in video state.  Releases and clears out the saved surface and surface textures.
         */
        CARAPI_(void) SetDoneWithSurface();

        /**
         * Retrieves the saved surface instance.
         *
         * @return The surface.
         */
        CARAPI_(AutoPtr<ISurface>) GetSurface();

        /**
         * Sets the dimensions of the surface.
         *
         * @param width The width of the surface, in pixels.
         * @param height The height of the surface, in pixels.
         */
        CARAPI_(void) SetSurfaceDimensions(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Handles a user clicking the surface, which is the trigger to toggle the full screen
         * Video UI.
         *
         * @param view The view receiving the click.
         */
        // @Override
        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        /**
         * Creates the {@link Surface}, adjusting the {@link SurfaceTexture} buffer size.
         */
        CARAPI_(Boolean) CreateSurface();

    private:
        Int32 mSurfaceId;
        AutoPtr<ITextureView> mTextureView;
        AutoPtr<ISurfaceTexture> mSavedSurfaceTexture;
        AutoPtr<ISurface> mSavedSurface;

        CVideoCallFragment* mHost;
    };

    class MyGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        MyGlobalLayoutListener(
            /* [in] */ IView* view,
            /* [in] */ CVideoCallFragment* host)
            : mView(view)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnGlobalLayout();

    private:
        AutoPtr<IView> mView;
        CVideoCallFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CVideoCallFragment();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Handles creation of the activity and initialization of the presenter.
     *
     * @param savedInstanceState The saved instance state.
     */
    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Handles creation of the fragment view.
     *
     * @param inflater The inflater.
     * @param container The view group containing the fragment.
     * @param savedInstanceState The saved instance state.
     * @return
     */
    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    /**
     * After creation of the fragment view, retrieves the required views.
     *
     * @param view The fragment view.
     * @param savedInstanceState The saved instance state.
     */
    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Creates the presenter for the {@link VideoCallFragment}.
     * @return The presenter instance.
     */
    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    /**
     * @return The user interface for the presenter, which is this fragment.
     */
    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    /**
     * Toggles visibility of the video UI.
     *
     * @param show {@code True} if the video surfaces should be shown.
     */
    // @Override
    CARAPI ShowVideoUi(
        /* [in] */ Boolean show);

    /**
     * Cleans up the video telephony surfaces.  Used when the presenter indicates a change to an
     * audio-only state.  Since the surfaces are static, it is important to ensure they are cleaned
     * up promptly.
     */
    // @Override
    CARAPI CleanupSurfaces();

    // @Override
    CARAPI IsActivityRestart(
        /* [out] */ Boolean* result);

    /**
     * @return {@code True} if the display video surface has been created.
     */
    // @Override
    CARAPI IsDisplayVideoSurfaceCreated(
        /* [out] */ Boolean* result);

    /**
     * @return {@code True} if the preview video surface has been created.
     */
    // @Override
    CARAPI IsPreviewVideoSurfaceCreated(
        /* [out] */ Boolean* result);

    /**
     * {@link android.view.Surface} on which incoming video for a video call is displayed.
     * {@code Null} until the video views {@link android.view.ViewStub} is inflated.
     */
    // @Override
    CARAPI GetDisplayVideoSurface(
        /* [out] */ ISurface** surface);

    /**
     * {@link android.view.Surface} on which a preview of the outgoing video for a video call is
     * displayed.  {@code Null} until the video views {@link android.view.ViewStub} is inflated.
     */
    // @Override
    CARAPI GetPreviewVideoSurface(
        /* [out] */ ISurface** surface);

    /**
     * Changes the dimensions of the preview surface.  Called when the dimensions change due to a
     * device orientation change.
     *
     * @param width The new width.
     * @param height The new height.
     */
    // @Override
    CARAPI SetPreviewSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    /**
     * Centers the display view vertically for portrait orientation, and horizontally for
     * lanscape orientations.  The view is centered within the available space not occupied by
     * the call card.
     *
     * @param displayVideo The video view to center.
     */
    CARAPI_(void) CenterDisplayView(
        /* [in] */ IView* displayVideo);

    /**
     * Inflates the {@link ViewStub} containing the incoming and outgoing surfaces, if necessary,
     * and creates {@link VideoCallSurface} instances to track the surfaces.
     */
    CARAPI_(void) InflateVideoCallViews();

    /**
     * Resizes a surface so that it has the same size as the full screen and so that it is
     * centered vertically below the call card.
     *
     * @param textureView The {@link TextureView} to resize and position.
     * @param size The size of the screen.
     */
    CARAPI_(void) SetSurfaceSizeAndTranslation(
        /* [in] */ ITextureView* textureView,
        /* [in] */ IPoint* size);

    /**
     * Determines the size of the device screen.
     *
     * @return {@link Point} specifying the width and height of the screen.
     */
    CARAPI_(AutoPtr<IPoint>) GetScreenSize();

private:
    /**
     * Used to indicate that the surface dimensions are not set.
     */
    static const Int32 DIMENSIONS_NOT_SET = -1;

    // Static storage used to retain the video surfaces across Activity restart.
    // TextureViews are not parcelable, so it is not possible to store them in the saved state.
    static Boolean sVideoSurfacesInUse;
    static AutoPtr<VideoCallSurface> sPreviewSurface;
    static AutoPtr<VideoCallSurface> sDisplaySurface;

    /**
     * {@link ViewStub} holding the video call surfaces.  This is the parent for the
     * {@link VideoCallFragment}.  Used to ensure that the video surfaces are only inflated when
     * required.
     */
    AutoPtr<IViewStub> mVideoViewsStub;

    /**
     * Inflated view containing the video call surfaces represented by the {@link ViewStub}.
     */
    AutoPtr<IView> mVideoViews;

    /**
     * {@code True} when the entering the activity again after a restart due to orientation change.
     */
    Boolean mIsActivityRestart;

    /**
     * {@code True} when the layout of the activity has been completed.
     */
    Boolean mIsLayoutComplete;

    /**
     * {@code True} if in landscape mode.
     */
    Boolean mIsLandscape;

    /**
     * The width of the surface.
     */
    Int32 mWidth;

    /**
     * The height of the surface.
     */
    Int32 mHeight;

    friend class MyGlobalLayoutListener;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CVIDEOCALLFRAGMENT_H__
