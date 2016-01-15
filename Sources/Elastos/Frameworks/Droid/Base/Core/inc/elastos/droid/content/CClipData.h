
#ifndef __ELASTOS_DROID_CONTENT_CCLIPDATA_H__
#define __ELASTOS_DROID_CONTENT_CCLIPDATA_H__

#include "_Elastos_Droid_Content_CClipData.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Graphics::IBitmap;

using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Representation of a clipped data on the clipboard.
 *
 * <p>ClippedData is a complex type containing one or Item instances,
 * each of which can hold one or more representations of an item of data.
 * For display to the user, it also has a label and iconic representation.</p>
 *
 * <p>A ClipData contains a {@link ClipDescription}, which describes
 * important meta-data about the clip.  In particular, its
 * {@link ClipDescription#getMimeType(int) getDescription().getMimeType(int)}
 * must return correct MIME type(s) describing the data in the clip.  For help
 * in correctly constructing a clip with the correct MIME type, use
 * {@link #newPlainText(CharSequence, CharSequence)},
 * {@link #newUri(ContentResolver, CharSequence, Uri)}, and
 * {@link #newIntent(CharSequence, Intent)}.
 *
 * <p>Each Item instance can be one of three main classes of data: a simple
 * CharSequence of text, a single Intent object, or a Uri.  See {@link Item}
 * for more details.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using the clipboard framework, read the
 * <a href="{@docRoot}guide/topics/clipboard/copy-paste.html">Copy and Paste</a>
 * developer guide.</p>
 * </div>
 *
 * <a name="ImplementingPaste"></a>
 * <h3>Implementing Paste or Drop</h3>
 *
 * <p>To implement a paste or drop of a ClippedData object into an application,
 * the application must correctly interpret the data for its use.  If the {@link Item}
 * it contains is simple text or an Intent, there is little to be done: text
 * can only be interpreted as text, and an Intent will typically be used for
 * creating shortcuts (such as placing icons on the home screen) or other
 * actions.
 *
 * <p>If all you want is the textual representation of the clipped data, you
 * can use the convenience method {@link Item#coerceToText Item.coerceToText}.
 * In this case there is generally no need to worry about the MIME types
 * reported by {@link ClipDescription#getMimeType(int) getDescription().getMimeType(int)},
 * since any clip item can always be converted to a string.
 *
 * <p>More complicated exchanges will be done through URIs, in particular
 * "content:" URIs.  A content URI allows the recipient of a ClippedData item
 * to interact closely with the ContentProvider holding the data in order to
 * negotiate the transfer of that data.  The clip must also be filled in with
 * the available MIME types; {@link #newUri(ContentResolver, CharSequence, Uri)}
 * will take care of correctly doing this.
 *
 * <p>For example, here is the paste function of a simple NotePad application.
 * When retrieving the data from the clipboard, it can do either two things:
 * if the clipboard contains a URI reference to an existing note, it copies
 * the entire structure of the note into a new note; otherwise, it simply
 * coerces the clip into text and uses that as the new note's contents.
 *
 * {@sample development/samples/NotePad/src/com/example/android/notepad/NoteEditor.java
 *      paste}
 *
 * <p>In many cases an application can paste various types of streams of data.  For
 * example, an e-mail application may want to allow the user to paste an image
 * or other binary data as an attachment.  This is accomplished through the
 * ContentResolver {@link ContentResolver#getStreamTypes(Uri, String)} and
 * {@link ContentResolver#openTypedAssetFileDescriptor(Uri, String, android.os.Bundle)}
 * methods.  These allow a client to discover the type(s) of data that a particular
 * content URI can make available as a stream and retrieve the stream of data.
 *
 * <p>For example, the implementation of {@link Item#coerceToText Item.coerceToText}
 * itself uses this to try to retrieve a URI clip as a stream of text:
 *
 * {@sample frameworks/base/core/java/android/content/ClipData.java coerceToText}
 *
 * <a name="ImplementingCopy"></a>
 * <h3>Implementing Copy or Drag</h3>
 *
 * <p>To be the source of a clip, the application must construct a ClippedData
 * object that any recipient can interpret best for their context.  If the clip
 * is to contain a simple text, Intent, or URI, this is easy: an {@link Item}
 * containing the appropriate data type can be constructed and used.
 *
 * <p>More complicated data types require the implementation of support in
 * a ContentProvider for describing and generating the data for the recipient.
 * A common scenario is one where an application places on the clipboard the
 * content: URI of an object that the user has copied, with the data at that
 * URI consisting of a complicated structure that only other applications with
 * direct knowledge of the structure can use.
 *
 * <p>For applications that do not have intrinsic knowledge of the data structure,
 * the content provider holding it can make the data available as an arbitrary
 * number of types of data streams.  This is done by implementing the
 * ContentProvider {@link ContentProvider#getStreamTypes(Uri, String)} and
 * {@link ContentProvider#openTypedAssetFile(Uri, String, android.os.Bundle)}
 * methods.
 *
 * <p>Going back to our simple NotePad application, this is the implementation
 * it may have to convert a single note URI (consisting of a title and the note
 * text) into a stream of plain text data.
 *
 * {@sample development/samples/NotePad/src/com/example/android/notepad/NotePadProvider.java
 *      stream}
 *
 * <p>The copy operation in our NotePad application is now just a simple matter
 * of making a clip containing the URI of the note being copied:
 *
 * {@sample development/samples/NotePad/src/com/example/android/notepad/NotesList.java
 *      copy}
 *
 * <p>Note if a paste operation needs this clip as text (for example to paste
 * into an editor), then {@link Item#coerceToText(Context)} will ask the content
 * provider for the clip URI as text and successfully paste the entire note.
 */
CarClass(CClipData)
    , public Object
    , public IClipData
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CClipData();

    virtual ~CClipData();

    /**
     * Create a new ClipData holding data of the type
     * {@link ClipDescription#MIMETYPE_TEXT_PLAIN}.
     *
     * @param label User-visible label for the clip data.
     * @param text The actual text in the clip.
     * @return Returns a new ClipData containing the specified data.
     */
    static CARAPI NewPlainText(
        /* [in] */ ICharSequence* label,
        /* [in] */ ICharSequence* text,
        /* [out] */ IClipData** data);

    /**
     * Create a new ClipData holding data of the type
     * {@link ClipDescription#MIMETYPE_TEXT_HTML}.
     *
     * @param label User-visible label for the clip data.
     * @param text The text of clip as plain text, for receivers that don't
     * handle HTML.  This is required.
     * @param htmlText The actual HTML text in the clip.
     * @return Returns a new ClipData containing the specified data.
     */
    static CARAPI NewHtmlText(
        /* [in] */ ICharSequence* label,
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& htmlText,
        /* [out] */ IClipData** data);

    /**
     * Create a new ClipData holding an Intent with MIME type
     * {@link ClipDescription#MIMETYPE_TEXT_INTENT}.
     *
     * @param label User-visible label for the clip data.
     * @param intent The actual Intent in the clip.
     * @return Returns a new ClipData containing the specified data.
     */
    static CARAPI NewIntent(
        /* [in] */ ICharSequence* label,
        /* [in] */ IIntent* intent,
        /* [out] */ IClipData** data);

    /**
     * Create a new ClipData holding a URI.  If the URI is a content: URI,
     * this will query the content provider for the MIME type of its data and
     * use that as the MIME type.  Otherwise, it will use the MIME type
     * {@link ClipDescription#MIMETYPE_TEXT_URILIST}.
     *
     * @param resolver ContentResolver used to get information about the URI.
     * @param label User-visible label for the clip data.
     * @param uri The URI in the clip.
     * @return Returns a new ClipData containing the specified data.
     */
    static CARAPI NewUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ ICharSequence* label,
        /* [in] */ IUri* uri,
        /* [out] */ IClipData** data);

    /**
     * Create a new ClipData holding an URI with MIME type
     * {@link ClipDescription#MIMETYPE_TEXT_URILIST}.
     * Unlike {@link #newUri(ContentResolver, CharSequence, Uri)}, nothing
     * is inferred about the URI -- if it is a content: URI holding a bitmap,
     * the reported type will still be uri-list.  Use this with care!
     *
     * @param label User-visible label for the clip data.
     * @param uri The URI in the clip.
     * @return Returns a new ClipData containing the specified data.
     */
    static CARAPI NewRawUri(
        /* [in] */ ICharSequence* label,
        /* [in] */ IUri* uri,
        /* [out] */ IClipData** data);

    /**
    * Return the {@link ClipDescription} associated with this data, describing
    * what it contains.
    */
    CARAPI GetDescription(
        /* [out] */ IClipDescription** description);

    /**
     * Add a new Item to the overall ClipData container.
     */
    CARAPI AddItem(
        /* [in] */ IClipDataItem* item);

    /** @hide */
    CARAPI GetIcon(
        /* [out] */ IBitmap** icon);

    /**
     * Return the number of items in the clip data.
     */
    CARAPI GetItemCount(
        /* [out] */ Int32* itemCount);

    /**
     * Return a single item inside of the clip data.  The index can range
     * from 0 to {@link #getItemCount()}-1.
     */
    CARAPI GetItemAt(
        /* [in] */ Int32 index,
        /* [out] */ IClipDataItem** item);

    /**
     * Prepare this {@link ClipData} to leave an app process.
     *
     * @hide
     */
    CARAPI PrepareToLeaveProcess();

    /** @hide */
    CARAPI FixUris(
        /* [in] */ Int32 contentUserHint);

    /**
     * Only fixing the data field of the intents
     * @hide
     */
    CARAPI FixUrisLight(
        /* [in] */ Int32 contentUserHint);

    CARAPI ToString(
        /* [out] */ String* str);

    /** @hide */
    CARAPI ToShortString(
        /* [in] */ IStringBuilder* sb);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Create a new clip.
     *
     * @param label Label to show to the user describing this clip.
     * @param mimeTypes An array of MIME types this data is available as.
     * @param item The contents of the first item in the clip.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* label,
        /* [in] */ ArrayOf<String>* mimeTypes,
        /* [in] */ IClipDataItem* item);

    /**
     * Create a new clip.
     *
     * @param description The ClipDescription describing the clip contents.
     * @param item The contents of the first item in the clip.
     */
    CARAPI constructor(
        /* [in] */ IClipDescription* description,
        /* [in] */ IClipDataItem* item);

    /**
     * Create a new clip that is a copy of another clip.  This does a deep-copy
     * of all items in the clip.
     *
     * @param other The existing ClipData that is to be copied.
     */
    CARAPI constructor(
        /* [in] */ IClipData* other);

    CARAPI constructor();

public:
    static const AutoPtr< ArrayOf<String> > MIMETYPES_TEXT_PLAIN;
    static const AutoPtr< ArrayOf<String> > MIMETYPES_TEXT_HTML;
    static const AutoPtr< ArrayOf<String> > MIMETYPES_TEXT_URILIST;
    static const AutoPtr< ArrayOf<String> > MIMETYPES_TEXT_INTENT;

    AutoPtr<IClipDescription> mClipDescription;
    AutoPtr<IBitmap> mIcon;
    List< AutoPtr<IClipDataItem> > mItems;

private:
    static const String TAG;
};

} // Content
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_CONTENT_CCLIPDATA_H__
