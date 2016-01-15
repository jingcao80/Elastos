
#ifndef __ELASTOS_DROID_GRAPHICS_PDF_CPDFDOCUMENT_H__
#define __ELASTOS_DROID_GRAPHICS_PDF_CPDFDOCUMENT_H__

#include "_Elastos_Droid_Graphics_Pdf_CPdfDocument.h"
#include "elastos/droid/graphics/Canvas.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloseGuard;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {
/**
 * <p>
 * This class enables generating a PDF document from native Android content. You
 * create a new document and then for every page you want to add you start a page,
 * write content to the page, and finish the page. After you are done with all
 * pages, you write the document to an output stream and close the document.
 * After a document is closed you should not use it anymore. Note that pages are
 * created one by one, i.e. you can have only a single page to which you are
 * writing at any given time. This class is not thread safe.
 * </p>
 * <p>
 * A typical use of the APIs looks like this:
 * </p>
 * <pre>
 * // create a new document
 * PdfDocument document = new PdfDocument();
 *
 * // crate a page description
 * PageInfo pageInfo = new PageInfo.Builder(new Rect(0, 0, 100, 100), 1).create();
 *
 * // start a page
 * Page page = document.startPage(pageInfo);
 *
 * // draw something on the page
 * View content = getContentView();
 * content.draw(page.getCanvas());
 *
 * // finish the page
 * document.finishPage(page);
 * . . .
 * // add more pages
 * . . .
 * // write the document content
 * document.writeTo(getOutputStream());
 *
 * // close the document
 * document.close();
 * </pre>
 */
CarClass(CPdfDocument)
    , public Object
    , public IPdfDocument
{
public:
    /**
     * This class represents meta-data that describes a PDF {@link Page}.
     */
    class PageInfo
        : public Object
        , public IPdfDocumentPageInfo
    {
    public:
        /**
         * Builder for creating a {@link PageInfo}.
         */
        class Builder
            : public Object
            , public IPdfDocumentPageInfoBuilder
        {
        public:
            CAR_INTERFACE_DECL();

            /**
             * Creates a new builder with the mandatory page info attributes.
             *
             * @param pageWidth The page width in PostScript (1/72th of an inch).
             * @param pageHeight The page height in PostScript (1/72th of an inch).
             * @param pageNumber The page number.
             */
            Builder(
                /* [in] */ Int32 pageWidth,
                /* [in] */ Int32 pageHeight,
                /* [in] */ Int32 pageNumber);

            /**
             * Sets the content rectangle in PostScript point (1/72th of an inch).
             * This is the area that contains the page content and is relative to
             * the page top left.
             *
             * @param contentRect The content rectangle. Must fit in the page.
             */
            CARAPI SetContentRect(
                /* [in] */ IRect* contentRect);

            /**
             * Creates a new {@link PageInfo}.
             *
             * @return The new instance.
             */
            CARAPI Create(
                /* [out] */ IPdfDocumentPageInfo** info);

        private:
            AutoPtr<PageInfo> mPageInfo;
        };

    public:
        CAR_INTERFACE_DECL();

        /**
         * Gets the page width in PostScript points (1/72th of an inch).
         *
         * @return The page width.
         */
        CARAPI GetPageWidth(
            /* [out] */ Int32* width);

        /**
         * Gets the page height in PostScript points (1/72th of an inch).
         *
         * @return The page height.
         */
        CARAPI GetPageHeight(
            /* [out] */ Int32* height);

        /**
         * Get the content rectangle in PostScript points (1/72th of an inch).
         * This is the area that contains the page content and is relative to
         * the page top left.
         *
         * @return The content rectangle.
         */
        CARAPI GetContentRect(
            /* [out] */ IRect** rect);

        /**
         * Gets the page number.
         *
         * @return The page number.
         */
        CARAPI GetPageNumber(
            /* [out] */ Int32* number);

    private:
        /**
         * Creates a new instance.
         */
        PageInfo();

    private:
        friend class CPdfDocument;
        Int32 mPageWidth;
        Int32 mPageHeight;
        AutoPtr<IRect> mContentRect;
        Int32 mPageNumber;
    };

    /**
     * This class represents a PDF document page. It has associated
     * a canvas on which you can draw content and is acquired by a
     * call to {@link #getCanvas()}. It also has associated a
     * {@link PageInfo} instance that describes its attributes. Also
     * a page has
     */
    class Page
        : public Object
        , public IPdfDocumentPage
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * Gets the {@link Canvas} of the page.
         *
         * <p>
         * <strong>Note: </strong> There are some draw operations that are not yet
         * supported by the canvas returned by this method. More specifically:
         * <ul>
         * <li>Inverse path clipping performed via {@link Canvas#clipPath(android.graphics.Path,
         *     android.graphics.Region.Op) Canvas.clipPath(android.graphics.Path,
         *     android.graphics.Region.Op)} for {@link
         *     android.graphics.Region.Op#REVERSE_DIFFERENCE
         *     Region.Op#REVERSE_DIFFERENCE} operations.</li>
         * <li>{@link Canvas#drawVertices(android.graphics.Canvas.VertexMode, Int32,
         *     float[], Int32, float[], Int32, Int32[], Int32, short[], Int32, Int32,
         *     android.graphics.Paint) Canvas.drawVertices(
         *     android.graphics.Canvas.VertexMode, Int32, float[], Int32, float[],
         *     Int32, Int32[], Int32, short[], Int32, Int32, android.graphics.Paint)}</li>
         * <li>Color filters set via {@link Paint#setColorFilter(
         *     android.graphics.ColorFilter)}</li>
         * <li>Mask filters set via {@link Paint#setMaskFilter(
         *     android.graphics.MaskFilter)}</li>
         * <li>Some XFER modes such as
         *     {@link android.graphics.PorterDuff.Mode#SRC_ATOP PorterDuff.Mode SRC},
         *     {@link android.graphics.PorterDuff.Mode#DST_ATOP PorterDuff.DST_ATOP},
         *     {@link android.graphics.PorterDuff.Mode#XOR PorterDuff.XOR},
         *     {@link android.graphics.PorterDuff.Mode#ADD PorterDuff.ADD}</li>
         * </ul>
         *
         * @return The canvas if the page is not finished, NULL otherwise.
         *
         * @see PdfDocument#finishPage(Page)
         */
        CARAPI GetCanvas(
            /* [out] */ ICanvas** canvas);

        /**
         * Gets the {@link PageInfo} with meta-data for the page.
         *
         * @return The page info.
         *
         * @see PdfDocument#finishPage(Page)
         */
        CARAPI GetInfo(
            /* [out] */ IPdfDocumentPageInfo** info);

        CARAPI IsFinished(
            /* [out] */ Boolean* finished);

    private:
        /**
         * Creates a new instance.
         *
         * @param canvas The canvas of the page.
         * @param pageInfo The info with meta-data.
         */
        Page(
            /* [in] */ ICanvas* canvas,
            /* [in] */ PageInfo* pageInfo);

        CARAPI_(void) Finish();

    private:
        friend class CPdfDocument;
        AutoPtr<PageInfo> mPageInfo;
        AutoPtr<ICanvas> mCanvas;
    };

private:
    class PdfCanvas
        : public Canvas
    {
    public:
        PdfCanvas(
            /* [in] */ Int64 nativeCanvas);

        // @Override
        CARAPI SetBitmap(
            /* [in] */ IBitmap* bitmap);
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    /**
     * Creates a new instance.
     */
    CPdfDocument();

    ~CPdfDocument();

    CARAPI constructor();

    /**
     * Starts a page using the provided {@link PageInfo}. After the page
     * is created you can draw arbitrary content on the page's canvas which
     * you can get by calling {@link Page#getCanvas()}. After you are done
     * drawing the content you should finish the page by calling
     * {@link #finishPage(Page)}. After the page is finished you should
     * no longer access the page or its canvas.
     * <p>
     * <strong>Note:</strong> Do not call this method after {@link #close()}.
     * Also do not call this method if the last page returned by this method
     * is not finished by calling {@link #finishPage(Page)}.
     * </p>
     *
     * @param pageInfo The page info. Cannot be NULL.
     * @return A blank page.
     *
     * @see #finishPage(Page)
     */
    CARAPI StartPage(
        /* [in] */ IPdfDocumentPageInfo* pageInfo,
        /* [out] */ IPdfDocumentPage** page);

    /**
     * Finishes a started page. You should always finish the last started page.
     * <p>
     * <strong>Note:</strong> Do not call this method after {@link #close()}.
     * You should not finish the same page more than once.
     * </p>
     *
     * @param page The page. Cannot be NULL.
     *
     * @see #startPage(PageInfo)
     */
    CARAPI FinishPage(
        /* [in] */ IPdfDocumentPage* page);

    /**
     * Writes the document to an output stream. You can call this method
     * multiple times.
     * <p>
     * <strong>Note:</strong> Do not call this method after {@link #close()}.
     * Also do not call this method if a page returned by {@link #startPage(
     * PageInfo)} is not finished by calling {@link #finishPage(Page)}.
     * </p>
     *
     * @param out The output stream. Cannot be NULL.
     *
     * @throws IOException If an error occurs while writing.
     */
    CARAPI WriteTo(
        /* [in] */ IOutputStream* out) /*throws IOException*/;

    /**
     * Gets the pages of the document.
     *
     * @return The pages or an empty list.
     */
    CARAPI GetPages(
        /* [out] */ IList** list);

    /**
     * Closes this document. This method should be called after you
     * are done working with the document. After this call the document
     * is considered closed and none of its methods should be called.
     * <p>
     * <strong>Note:</strong> Do not call this method if the page
     * returned by {@link #startPage(PageInfo)} is not finished by
     * calling {@link #finishPage(Page)}.
     * </p>
     */
    CARAPI Close();

private:
    CARAPI Dispose();

    /**
     * Throws an exception if the document is already closed.
     */
    CARAPI ThrowIfClosed();

    /**
     * Throws an exception if the last started page is not finished.
     */
    CARAPI ThrowIfCurrentPageNotFinished();

    CARAPI_(Int64) NativeCreateDocument();

    CARAPI_(void) NativeClose(
        /* [in] */ Int64 nativeDocument);

    CARAPI_(void) NativeFinishPage(
        /* [in] */ Int64 nativeDocument);

    CARAPI_(void) NativeWriteTo(
        /* [in] */ Int64 nativeDocument,
        /* [in] */ IOutputStream* out,
        /* [in] */ ArrayOf<Byte>* chunk);

    static CARAPI_(Int64) NativeStartPage(
        /* [in] */ Int64 nativeDocument,
        /* [in] */ Int32 pageWidth,
        /* [in] */ Int32 pageHeight,
        /* [in] */ Int32 contentLeft,
        /* [in] */ Int32 contentTop,
        /* [in] */ Int32 contentRight,
        /* [in] */ Int32 contentBottom);

private:
    // TODO: We need a constructor that will take an OutputStream to
    // support online data serialization as opposed to the current
    // on demand one. The current approach is fine until Skia starts
    // to support online PDF generation at which point we need to
    // handle this.
    AutoPtr<ArrayOf<Byte> > mChunk;

    AutoPtr<ICloseGuard> mCloseGuard;

    AutoPtr<IList/*<IPdfDocumentPageInfo*>*/> mPages;

    Int64 mNativeDocument;

    AutoPtr<IPdfDocumentPage> mCurrentPage;
};

} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_PDF_CPDFDOCUMENT_H__
