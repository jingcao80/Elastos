
#ifndef __ELASTOS_DROID_GRAPHICS_PDF_PDFEDITOR_H__
#define __ELASTOS_DROID_GRAPHICS_PDF_PDFEDITOR_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Pdf {

/**
 * Class for editing PDF files.
 *
 * @hide
 */
class PdfEditor
    : public Object
    , public IPdfEditor
{
public:
    CAR_INTERFACE_DECL();

    PdfEditor();

    /**
     * Creates a new instance.
     * <p>
     * <strong>Note:</strong> The provided file descriptor must be <strong>seekable</strong>,
     * i.e. its data being randomly accessed, e.g. pointing to a file. After finishing
     * with this class you must call {@link #close()}.
     * </p>
     * <p>
     * <strong>Note:</strong> This class takes ownership of the passed in file descriptor
     * and is responsible for closing it when the editor is closed.
     * </p>
     *
     * @param input Seekable file descriptor to read from.
     *
     * @see #close()
     */
    CARAPI constructor(
        /* [in] */ /*@NonNull*/ IParcelFileDescriptor* input) /*throws IOException*/;

    ~PdfEditor();

    /**
     * Gets the number of pages in the document.
     *
     * @return The page count.
     */
    CARAPI GetPageCount(
        /* [out] */ Int32* count);

    /**
     * Removes the page with a given index.
     *
     * @param pageIndex The page to remove.
     */
    CARAPI RemovePage(
        /* [in] */ Int32 pageIndex);

    /**
     * Writes the PDF file to the provided destination.
     * <p>
     * <strong>Note:</strong> This method takes ownership of the passed in file
     * descriptor and is responsible for closing it when writing completes.
     * </p>
     * @param output The destination.
     */
    CARAPI Write(
        /* [in] */ IParcelFileDescriptor* output) /*throws IOException*/;

    /**
     * Closes this editor. You should not use this instance
     * after this method is called.
     */
    CARAPI Close();

private:
    CARAPI_(void) DoClose();

    CARAPI ThrowIfClosed();

    CARAPI ThrowIfPageNotInDocument(
        /* [in] */ Int32 pageIndex);

    static CARAPI NativeOpen(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 size,
        /* [out] */ Int64* resVal);

    static CARAPI_(void) NativeClose(
        /* [in] */ Int64 documentPtr);

    static CARAPI_(Int32) NativeGetPageCount(
        /* [in] */ Int64 documentPtr);

    static CARAPI_(Int32) NativeRemovePage(
        /* [in] */ Int64 documentPtr,
        /* [in] */ Int32 pageIndex);

    static CARAPI NativeWrite(
        /* [in] */ Int64 documentPtr,
        /* [in] */ Int32 fd);

private:
    AutoPtr<ICloseGuard> mCloseGuard;
    Int64 mNativeDocument;
    Int32 mPageCount;
    AutoPtr<IParcelFileDescriptor> mInput;
};

} // namespace Pdf
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_PDF_PDFEDITOR_H__
