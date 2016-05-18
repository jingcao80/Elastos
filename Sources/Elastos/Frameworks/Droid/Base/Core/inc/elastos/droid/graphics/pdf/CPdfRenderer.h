
#ifndef __ELASTOS_DROID_GRAPHICS_PDF_CPDFRENDERER_H__
#define __ELASTOS_DROID_GRAPHICS_PDF_CPDFRENDERER_H__

#include "_Elastos_Droid_Graphics_Pdf_CPdfRenderer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {
/**
 * <p>
 * This class enables rendering a PDF document. This class is not thread safe.
 * </p>
 * <p>
 * If you want to render a PDF, you create a renderer and for every page you want
 * to render, you open the page, render it, and close the page. After you are done
 * with rendering, you close the renderer. After the renderer is closed it should not
 * be used anymore. Note that the pages are rendered one by one, i.e. you can have
 * only a single page opened at any given time.
 * </p>
 * <p>
 * A typical use of the APIs to render a PDF looks like this:
 * </p>
 * <pre>
 * // create a new renderer
 * PdfRenderer renderer = new PdfRenderer(getSeekableFileDescriptor());
 *
 * // let us just render all pages
 * final Int32 pageCount = renderer.getPageCount();
 * for (Int32 i = 0; i < pageCount; i++) {
 *     Page page = renderer.openPage(i);
 *
 *     // say we render for showing on the screen
 *     page.render(mBitmap, null, null, Page.RENDER_MODE_FOR_DISPLAY);
 *
 *     // do stuff with the bitmap
 *
 *     // close the page
 *     page.close();
 * }
 *
 * // close the renderer
 * renderer.close();
 * </pre>
 *
 * <h3>Print preview and print output</h3>
 * <p>
 * If you are using this class to rasterize a PDF for printing or show a print
 * preview, it is recommended that you respect the following contract in order
 * to provide a consistent user experience when seeing a preview and printing,
 * i.e. the user sees a preview that is the same as the printout.
 * </p>
 * <ul>
 * <li>
 * Respect the property whether the document would like to be scaled for printing
 * as per {@link #shouldScaleForPrinting()}.
 * </li>
 * <li>
 * When scaling a document for printing the aspect ratio should be preserved.
 * </li>
 * <li>
 * Do not inset the content with any margins from the {@link android.print.PrintAttributes}
 * as the application is responsible to render it such that the margins are respected.
 * </li>
 * <li>
 * If document page size is greater than the printed media size the content should
 * be anchored to the upper left corner of the page for left-to-right locales and
 * top right corner for right-to-left locales.
 * </li>
 * </ul>
 *
 * @see #close()
 */
CarClass(CPdfRenderer)
    , public Object
    , public IPdfRenderer
    /*implements AutoCloseable*/
{
public:
    /**
     * This class represents a PDF document page for rendering.
     */
    class Page
        : public Object
        , public IPdfRendererPage //implements AutoCloseable
    {
    public:
        CAR_INTERFACE_DECL()

        ~Page();

        /**
         * Gets the page index.
         *
         * @return The index.
         */
        CARAPI GetIndex(
            /* [out] */ Int32* index);

        /**
         * Gets the page width in points (1/72").
         *
         * @return The width in points.
         */
        CARAPI GetWidth(
            /* [out] */ Int32* width);

        /**
         * Gets the page height in points (1/72").
         *
         * @return The height in points.
         */
        CARAPI GetHeight(
            /* [out] */ Int32* height);

        /**
         * Renders a page to a bitmap.
         * <p>
         * You may optionally specify a rectangular clip in the bitmap bounds. No rendering
         * outside the clip will be performed, hence it is your responsibility to initialize
         * the bitmap outside the clip.
         * </p>
         * <p>
         * You may optionally specify a matrix to transform the content from page coordinates
         * which are in points (1/72") to bitmap coordinates which are in pixels. If this
         * matrix is not provided this method will apply a transformation that will fit the
         * whole page to the destination clip if provided or the destination bitmap if no
         * clip is provided.
         * </p>
         * <p>
         * The clip and transformation are useful for implementing tile rendering where the
         * destination bitmap contains a portion of the image, for example when zooming.
         * Another useful application is for printing where the size of the bitmap holding
         * the page is too large and a client can render the page in stripes.
         * </p>
         * <p>
         * <strong>Note: </strong> The destination bitmap format must be
         * {@link Config#ARGB_8888 ARGB}.
         * </p>
         * <p>
         * <strong>Note: </strong> The optional transformation matrix must be affine as per
         * {@link android.graphics.Matrix#isAffine() Matrix.isAffine()}. Hence, you can specify
         * rotation, scaling, translation but not a perspective transformation.
         * </p>
         *
         * @param destination Destination bitmap to which to render.
         * @param destClip Optional clip in the bitmap bounds.
         * @param transform Optional transformation to apply when rendering.
         * @param renderMode The render mode.
         *
         * @see #RENDER_MODE_FOR_DISPLAY
         * @see #RENDER_MODE_FOR_PRINT
         */
        CARAPI Render(
            /* [in] */ /*@NonNull*/ IBitmap* destination,
            /* [in] */ /*@Nullable*/ IRect* destClip,
            /* [in] */ /*@Nullable*/ IMatrix* transform,
            /* [in] */ /*@RenderMode*/ Int32 renderMode);

        /**
         * Closes this page.
         *
         * @see android.graphics.pdf.PdfRenderer#openPage(Int32)
         */
        // @Override
        CARAPI Close();

    private:
        Page();

        CARAPI constructor(
            /* [in] */ Int32 index,
            /* [in] */ CPdfRenderer* host);

        CARAPI_(void) DoClose();

        CARAPI ThrowIfClosed();

    public:
        static const Int32 RENDER_MODE_FOR_DISPLAY;
        static const Int32 RENDER_MODE_FOR_PRINT;

    private:
        friend class CPdfRenderer;
        AutoPtr<ICloseGuard> mCloseGuard;
        Int32 mIndex;
        Int32 mWidth;
        Int32 mHeight;
        Int64 mNativePage;
        CPdfRenderer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @hide */
    // @IntDef({
    //     Page.RENDER_MODE_FOR_DISPLAY,
    //     Page.RENDER_MODE_FOR_PRINT
    // })
    // @Retention(RetentionPolicy.SOURCE)
    // public @interface RenderMode {}

    CPdfRenderer();

    ~CPdfRenderer();

    /**
     * Creates a new instance.
     * <p>
     * <strong>Note:</strong> The provided file descriptor must be <strong>seekable</strong>,
     * i.e. its data being randomly accessed, e.g. pointing to a file.
     * </p>
     * <p>
     * <strong>Note:</strong> This class takes ownership of the passed in file descriptor
     * and is responsible for closing it when the renderer is closed.
     * </p>
     *
     * @param input Seekable file descriptor to read from.
     */
    CARAPI constructor(
        /* [in] */ /*@NonNull*/ IParcelFileDescriptor* input) /*throws IOException*/;

    /**
     * Closes this renderer. You should not use this instance
     * after this method is called.
     */
    CARAPI Close();

    /**
     * Gets the number of pages in the document.
     *
     * @return The page count.
     */
    CARAPI GetPageCount(
        /* [out] */ Int32* count);

    /**
     * Gets whether the document prefers to be scaled for printing.
     * You should take this info account if the document is rendered
     * for printing and the target media size differs from the page
     * size.
     *
     * @return If to scale the document.
     */
    CARAPI ShouldScaleForPrinting(
        /* [out] */ Boolean* state);

    /**
     * Opens a page for rendering.
     *
     * @param index The page index.
     * @return A page that can be rendered.
     *
     * @see android.graphics.pdf.PdfRenderer.Page#close() PdfRenderer.Page.close()
     */
    CARAPI OpenPage(
        /* [in] */ Int32 index,
        /* [out] */ IPdfRendererPage** page);

private:
    CARAPI_(void) DoClose();

    CARAPI ThrowIfClosed();

    CARAPI ThrowIfPageOpened();

    CARAPI ThrowIfPageNotInDocument(
        /* [in] */ Int32 pageIndex);

    static CARAPI NativeCreate(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 size,
        /* [out] */ Int64* result);

    static CARAPI_(void) NativeClose(
        /* [in] */ Int64 documentPtr);

    static CARAPI_(Int32) NativeGetPageCount(
        /* [in] */ Int64 documentPtr);

    static CARAPI_(Boolean) NativeScaleForPrinting(
        /* [in] */ Int64 documentPtr);

    static CARAPI_(void) NativeRenderPage(
        /* [in] */ Int64 documentPtr,
        /* [in] */ Int64 pagePtr,
        /* [in] */ Int64 destPtr,
        /* [in] */ Int32 destLeft,
        /* [in] */ Int32 destTop,
        /* [in] */ Int32 destRight,
        /* [in] */ Int32 destBottom,
        /* [in] */ Int64 matrixPtr,
        /* [in] */ Int32 renderMode);

    static CARAPI NativeOpenPageAndGetSize(
        /* [in] */ Int64 documentPtr,
        /* [in] */ Int32 pageIndex,
        /* [in] */ IPoint* outSize,
        /* [out] */ Int64* result);

    static CARAPI_(void) NativeClosePage(
        /* [in] */ Int64 pagePtr);

private:
    AutoPtr<ICloseGuard> mCloseGuard;
    AutoPtr<IPoint> mTempPoint;
    Int64 mNativeDocument;
    Int32 mPageCount;
    AutoPtr<IParcelFileDescriptor> mInput;
    AutoPtr<IPdfRendererPage> mCurrentPage;
};


} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_PDF_CPDFRENDERER_H__
