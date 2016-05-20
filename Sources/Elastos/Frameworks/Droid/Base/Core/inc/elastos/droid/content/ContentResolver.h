
#ifndef __ELASTOS_DROID_CONTENT_CONTENTRESOLVER_H__
#define __ELASTOS_DROID_CONTENT_CONTENTRESOLVER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/database/CrossProcessCursorWrapper.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICrossProcessCursorWrapper;
using Elastos::Droid::Database::CrossProcessCursorWrapper;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ParcelFileDescriptor;

using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::IRandom;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * This class provides applications access to the content model.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using a ContentResolver with content providers, read the
 * <a href="{@docRoot}guide/topics/providers/content-providers.html">Content Providers</a>
 * developer guide.</p>
 */
class ContentResolver
    : public Object
    , public IContentResolver
{
private:
    class CursorWrapperInner
        : public CrossProcessCursorWrapper
    {
    public:
        TO_STRING_IMPL("ContentResolver::CursorWrapperInner")

        CursorWrapperInner(
            /* [in] */ IIContentProvider* icp,
            /* [in] */ ContentResolver* contentResolver);

        ~CursorWrapperInner();

        CARAPI constructor(
            /* [in] */ ICursor* cursor);

        CARAPI Close();

        CARAPI Finalize();

    private:
        AutoPtr<IIContentProvider> mContentProvider;
        AutoPtr<ICloseGuard> mCloseGuard;
        Boolean mProviderReleased;
        ContentResolver* mContentResolver;
    };

    class ParcelFileDescriptorInner
        : public ParcelFileDescriptor
    {
    public:
        ParcelFileDescriptorInner();

        CARAPI constructor(
            /* [in] */ IParcelFileDescriptor* pfd,
            /* [in] */ IIContentProvider* icp,
            /* [in] */ ContentResolver* contentResolver);

        CARAPI ReleaseResources();

    private:
        AutoPtr<IIContentProvider> mContentProvider;
        Boolean mProviderReleased;
        ContentResolver* mContentResolver;
    };

public:
    CAR_INTERFACE_DECL()

    ContentResolver();

    virtual ~ContentResolver();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /** @hide */
    static CARAPI_(String) SyncErrorToString(
        /* [in] */ Int32 error);

    /** @hide */
    static CARAPI_(Int32) SyncErrorStringToInt(
        /* [in] */ const String& error);

    /** @hide */
    virtual CARAPI ReleaseProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* result) = 0;

    /** @hide */
    virtual CARAPI ReleaseUnstableProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* result) = 0;

    /** @hide */
    virtual CARAPI UnstableProviderDied(
        /* [in] */ IIContentProvider* icp) = 0;

    /** @hide */
    CARAPI AppNotRespondingViaProvider(
        /* [in] */ IIContentProvider* icp);

    /**
     * Return the MIME type of the given content URL.
     *
     * @param url A Uri identifying content (either a list or specific type),
     * using the content:// scheme.
     * @return A MIME type for the content, or null if the URL is invalid or the type is unknown
     */
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    /**
     * Query for the possible MIME types for the representations the given
     * content URL can be returned when opened as as stream with
     * {@link #openTypedAssetFileDescriptor}.  Note that the types here are
     * not necessarily a superset of the type returned by {@link #getType} --
     * many content providers can not return a raw stream for the structured
     * data that they contain.
     *
     * @param url A Uri identifying content (either a list or specific type),
     * using the content:// scheme.
     * @param mimeTypeFilter The desired MIME type.  This may be a pattern,
     * such as * \/ *, to query for all available MIME types that match the
     * pattern.
     * @return Returns an array of MIME type strings for all available
     * data streams that match the given mimeTypeFilter.  If there are none,
     * null is returned.
     */
    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    /**
     * <p>
     * Query the given URI, returning a {@link Cursor} over the result set.
     * </p>
     * <p>
     * For best performance, the caller should follow these guidelines:
     * <ul>
     * <li>Provide an explicit projection, to prevent
     * reading data from storage that aren't going to be used.</li>
     * <li>Use question mark parameter markers such as 'phone=?' instead of
     * explicit values in the {@code selection} parameter, so that queries
     * that differ only by those values will be recognized as the same
     * for caching purposes.</li>
     * </ul>
     * </p>
     *
     * @param uri The URI, using the content:// scheme, for the content to
     *         retrieve.
     * @param projection A list of which columns to return. Passing null will
     *         return all columns, which is inefficient.
     * @param selection A filter declaring which rows to return, formatted as an
     *         SQL WHERE clause (excluding the WHERE itself). Passing null will
     *         return all rows for the given URI.
     * @param selectionArgs You may include ?s in selection, which will be
     *         replaced by the values from selectionArgs, in the order that they
     *         appear in the selection. The values will be bound as Strings.
     * @param sortOrder How to order the rows, formatted as an SQL ORDER BY
     *         clause (excluding the ORDER BY itself). Passing null will use the
     *         default sort order, which may be unordered.
     * @return A Cursor object, which is positioned before the first entry, or null
     * @see Cursor
     */
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    /**
     * <p>
     * Query the given URI, returning a {@link Cursor} over the result set.
     * </p>
     * <p>
     * For best performance, the caller should follow these guidelines:
     * <ul>
     * <li>Provide an explicit projection, to prevent
     * reading data from storage that aren't going to be used.</li>
     * <li>Use question mark parameter markers such as 'phone=?' instead of
     * explicit values in the {@code selection} parameter, so that queries
     * that differ only by those values will be recognized as the same
     * for caching purposes.</li>
     * </ul>
     * </p>
     *
     * @param uri The URI, using the content:// scheme, for the content to
     *         retrieve.
     * @param projection A list of which columns to return. Passing null will
     *         return all columns, which is inefficient.
     * @param selection A filter declaring which rows to return, formatted as an
     *         SQL WHERE clause (excluding the WHERE itself). Passing null will
     *         return all rows for the given URI.
     * @param selectionArgs You may include ?s in selection, which will be
     *         replaced by the values from selectionArgs, in the order that they
     *         appear in the selection. The values will be bound as Strings.
     * @param sortOrder How to order the rows, formatted as an SQL ORDER BY
     *         clause (excluding the ORDER BY itself). Passing null will use the
     *         default sort order, which may be unordered.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * If the operation is canceled, then {@link OperationCanceledException} will be thrown
     * when the query is executed.
     * @return A Cursor object, which is positioned before the first entry, or null
     * @see Cursor
     */
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    /**
     * Transform the given <var>url</var> to a canonical representation of
     * its referenced resource, which can be used across devices, persisted,
     * backed up and restored, etc.  The returned Uri is still a fully capable
     * Uri for use with its content provider, allowing you to do all of the
     * same content provider operations as with the original Uri --
     * {@link #query}, {@link #openInputStream(android.net.Uri)}, etc.  The
     * only difference in behavior between the original and new Uris is that
     * the content provider may need to do some additional work at each call
     * using it to resolve it to the correct resource, especially if the
     * canonical Uri has been moved to a different environment.
     *
     * <p>If you are moving a canonical Uri between environments, you should
     * perform another call to {@link #canonicalize} with that original Uri to
     * re-canonicalize it for the current environment.  Alternatively, you may
     * want to use {@link #uncanonicalize} to transform it to a non-canonical
     * Uri that works only in the current environment but potentially more
     * efficiently than the canonical representation.</p>
     *
     * @param url The {@link Uri} that is to be transformed to a canonical
     * representation.  Like all resolver calls, the input can be either
     * a non-canonical or canonical Uri.
     *
     * @return Returns the official canonical representation of <var>url</var>,
     * or null if the content provider does not support a canonical representation
     * of the given Uri.  Many providers may not support canonicalization of some
     * or all of their Uris.
     *
     * @see #uncanonicalize
     */
    CARAPI Canonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /**
     * Given a canonical Uri previously generated by {@link #canonicalize}, convert
     * it to its local non-canonical form.  This can be useful in some cases where
     * you know that you will only be using the Uri in the current environment and
     * want to avoid any possible overhead when using it with the content
     * provider or want to verify that the referenced data exists at all in the
     * new environment.
     *
     * @param url The canonical {@link Uri} that is to be convered back to its
     * non-canonical form.
     *
     * @return Returns the non-canonical representation of <var>url</var>.  This will
     * return null if data identified by the canonical Uri can not be found in
     * the current environment; callers must always check for null and deal with
     * that by appropriately falling back to an alternative.
     *
     * @see #canonicalize
     */
    CARAPI Uncanonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /**
     * Open a stream on to the content associated with a content URI.  If there
     * is no data associated with the URI, FileNotFoundException is thrown.
     *
     * <h5>Accepts the following URI schemes:</h5>
     * <ul>
     * <li>content ({@link #SCHEME_CONTENT})</li>
     * <li>android.resource ({@link #SCHEME_ANDROID_RESOURCE})</li>
     * <li>file ({@link #SCHEME_FILE})</li>
     * </ul>
     *
     * <p>See {@link #openAssetFileDescriptor(Uri, String)} for more information
     * on these schemes.
     *
     * @param uri The desired URI.
     * @return InputStream
     * @throws FileNotFoundException if the provided URI could not be opened.
     * @see #openAssetFileDescriptor(Uri, String)
     */
    CARAPI OpenInputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IInputStream** inStream);

    /**
     * Synonym for {@link #openOutputStream(Uri, String)
     * openOutputStream(uri, "w")}.
     * @throws FileNotFoundException if the provided URI could not be opened.
     */
    CARAPI OpenOutputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IOutputStream** ouStream);

    /**
     * Open a stream on to the content associated with a content URI.  If there
     * is no data associated with the URI, FileNotFoundException is thrown.
     *
     * <h5>Accepts the following URI schemes:</h5>
     * <ul>
     * <li>content ({@link #SCHEME_CONTENT})</li>
     * <li>file ({@link #SCHEME_FILE})</li>
     * </ul>
     *
     * <p>See {@link #openAssetFileDescriptor(Uri, String)} for more information
     * on these schemes.
     *
     * @param uri The desired URI.
     * @param mode May be "w", "wa", "rw", or "rwt".
     * @return OutputStream
     * @throws FileNotFoundException if the provided URI could not be opened.
     * @see #openAssetFileDescriptor(Uri, String)
     */
    CARAPI OpenOutputStream(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IOutputStream** outStream);

    /**
     * Open a raw file descriptor to access data under a URI.  This
     * is like {@link #openAssetFileDescriptor(Uri, String)}, but uses the
     * underlying {@link ContentProvider#openFile}
     * ContentProvider.openFile()} method, so will <em>not</em> work with
     * providers that return sub-sections of files.  If at all possible,
     * you should use {@link #openAssetFileDescriptor(Uri, String)}.  You
     * will receive a FileNotFoundException exception if the provider returns a
     * sub-section of a file.
     *
     * <h5>Accepts the following URI schemes:</h5>
     * <ul>
     * <li>content ({@link #SCHEME_CONTENT})</li>
     * <li>file ({@link #SCHEME_FILE})</li>
     * </ul>
     *
     * <p>See {@link #openAssetFileDescriptor(Uri, String)} for more information
     * on these schemes.
     *
     * @param uri The desired URI to open.
     * @param mode The file mode to use, as per {@link ContentProvider#openFile
     * ContentProvider.openFile}.
     * @return Returns a new ParcelFileDescriptor pointing to the file.  You
     * own this descriptor and are responsible for closing it when done.
     * @throws FileNotFoundException Throws FileNotFoundException of no
     * file exists under the URI or the mode is invalid.
     * @see #openAssetFileDescriptor(Uri, String)
     */
    CARAPI OpenFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    /**
     * Open a raw file descriptor to access data under a URI.  This
     * interacts with the underlying {@link ContentProvider#openAssetFile}
     * method of the provider associated with the given URI, to retrieve any file stored there.
     *
     * <h5>Accepts the following URI schemes:</h5>
     * <ul>
     * <li>content ({@link #SCHEME_CONTENT})</li>
     * <li>android.resource ({@link #SCHEME_ANDROID_RESOURCE})</li>
     * <li>file ({@link #SCHEME_FILE})</li>
     * </ul>
     * <h5>The android.resource ({@link #SCHEME_ANDROID_RESOURCE}) Scheme</h5>
     * <p>
     * A Uri object can be used to reference a resource in an APK file.  The
     * Uri should be one of the following formats:
     * <ul>
     * <li><code>android.resource://package_name/id_number</code><br/>
     * <code>package_name</code> is your package name as listed in your AndroidManifest.xml.
     * For example <code>com.example.myapp</code><br/>
     * <code>id_number</code> is the int form of the ID.<br/>
     * The easiest way to construct this form is
     * <pre>Uri uri = Uri.parse("android.resource://com.example.myapp/" + R.raw.my_resource");</pre>
     * </li>
     * <li><code>android.resource://package_name/type/name</code><br/>
     * <code>package_name</code> is your package name as listed in your AndroidManifest.xml.
     * For example <code>com.example.myapp</code><br/>
     * <code>type</code> is the const String& form of the resource type.  For example, <code>raw</code>
     * or <code>drawable</code>.
     * <code>name</code> is the const String& form of the resource name.  That is, whatever the file
     * name was in your res directory, without the type extension.
     * The easiest way to construct this form is
     * <pre>Uri uri = Uri.parse("android.resource://com.example.myapp/raw/my_resource");</pre>
     * </li>
     * </ul>
     *
     * <p>Note that if this function is called for read-only input (mode is "r")
     * on a content: URI, it will instead call {@link #openTypedAssetFileDescriptor}
     * for you with a MIME type of "* \ / *".  This allows such callers to benefit
     * from any built-in data conversion that a provider implements.
     *
     * @param uri The desired URI to open.
     * @param mode The file mode to use, as per {@link ContentProvider#openAssetFile
     * ContentProvider.openAssetFile}.
     * @return Returns a new ParcelFileDescriptor pointing to the file.  You
     * own this descriptor and are responsible for closing it when done.
     * @throws FileNotFoundException Throws FileNotFoundException of no
     * file exists under the URI or the mode is invalid.
     */
    CARAPI OpenAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    /**
     * Open a raw file descriptor to access (potentially type transformed)
     * data from a "content:" URI.  This interacts with the underlying
     * {@link ContentProvider#openTypedAssetFile} method of the provider
     * associated with the given URI, to retrieve retrieve any appropriate
     * data stream for the data stored there.
     *
     * <p>Unlike {@link #openAssetFileDescriptor}, this function only works
     * with "content:" URIs, because content providers are the only facility
     * with an associated MIME type to ensure that the returned data stream
     * is of the desired type.
     *
     * <p>All text/ * streams are encoded in UTF-8.
     *
     * @param uri The desired URI to open.
     * @param mimeType The desired MIME type of the returned data.  This can
     * be a pattern such as * \ / *, which will allow the content provider to
     * select a type, though there is no way for you to determine what type
     * it is returning.
     * @param opts Additional provider-dependent options.
     * @return Returns a new ParcelFileDescriptor from which you can read the
     * data stream from the provider.  Note that this may be a pipe, meaning
     * you can't seek in it.  The only seek you should do is if the
     * AssetFileDescriptor contains an offset, to move to that offset before
     * reading.  You own this descriptor and are responsible for closing it when done.
     * @throws FileNotFoundException Throws FileNotFoundException of no
     * data of the desired type exists under the URI.
     */
    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    /**
     * Resolves an android.resource URI to a {@link Resources} and a resource id.
     *
     * @hide
     */
    CARAPI GetResourceId(
        /* [in] */ IUri* uri,
        /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult);

    /**
     * Inserts a row into a table at the given URL.
     *
     * If the content provider supports transactions the insertion will be atomic.
     *
     * @param url The URL of the table to insert into.
     * @param values The initial values for the newly inserted row. The key is the column name for
     *               the field. Passing an empty ContentValues will create an empty row.
     * @return the URL of the newly created row.
     */
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedUri);

    /**
     * Applies each of the {@link ContentProviderOperation} objects and returns an array
     * of their results. Passes through OperationApplicationException, which may be thrown
     * by the call to {@link ContentProviderOperation#apply}.
     * If all the applications succeed then a {@link ContentProviderResult} array with the
     * same number of elements as the operations will be returned. It is implementation-specific
     * how many, if any, operations will have been successfully applied if a call to
     * apply results in a {@link OperationApplicationException}.
     * @param authority the authority of the ContentProvider to which this batch should be applied
     * @param operations the operations to apply
     * @return the results of the applications
     * @throws OperationApplicationException thrown if an application fails.
     * See {@link ContentProviderOperation#apply} for more information.
     * @throws RemoteException thrown if a RemoteException is encountered while attempting
     *   to communicate with a remote provider.
     */
    CARAPI ApplyBatch(
        /* [in] */ const String& authority,
        /* [in] */ IArrayList* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults);

    /**
     * Inserts multiple rows into a table at the given URL.
     *
     * This function make no guarantees about the atomicity of the insertions.
     *
     * @param url The URL of the table to insert into.
     * @param values The initial values for the newly inserted rows. The key is the column name for
     *               the field. Passing null will create an empty row.
     * @return the number of newly created rows.
     */
    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* values,
        /* [out] */ Int32* number);

    /**
     * Deletes row(s) specified by a content URI.
     *
     * If the content provider supports transactions, the deletion will be atomic.
     *
     * @param url The URL of the row to delete.
     * @param where A filter to apply to rows before deleting, formatted as an SQL WHERE clause
                    (excluding the WHERE itself).
     * @return The number of rows deleted.
     */
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    /**
     * Update row(s) in a content URI.
     *
     * If the content provider supports transactions the update will be atomic.
     *
     * @param uri The URI to modify.
     * @param values The new field values. The key is the column name for the field.
                     A null value will remove an existing field value.
     * @param where A filter to apply to rows before updating, formatted as an SQL WHERE clause
                    (excluding the WHERE itself).
     * @return the number of rows updated.
     * @throws NullPointerException if uri or values are null
     */
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    /**
     * Call a provider-defined method.  This can be used to implement
     * read or write interfaces which are cheaper than using a Cursor and/or
     * do not fit into the traditional table model.
     *
     * @param method provider-defined method name to call.  Opaque to
     *   framework, but must be non-null.
     * @param arg provider-defined const String& argument.  May be null.
     * @param extras provider-defined Bundle argument.  May be null.
     * @return a result Bundle, possibly null.  Will be null if the ContentProvider
     *   does not implement call.
     * @throws NullPointerException if uri or method is null
     * @throws IllegalArgumentException if uri is not known
     */
    CARAPI Call(
        /* [in] */ IUri* uri,
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    /**
     * Returns the content provider for the given content URI.
     *
     * @param uri The URI to a content provider
     * @return The ContentProvider for the given URI, or null if no content provider is found.
     * @hide
     */
    CARAPI AcquireProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * Returns the content provider for the given content URI if the process
     * already has a reference on it.
     *
     * @param uri The URI to a content provider
     * @return The ContentProvider for the given URI, or null if no content provider is found.
     * @hide
     */
    CARAPI AcquireExistingProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * @hide
     */
    CARAPI AcquireProvider(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * Returns the content provider for the given content URI.
     *
     * @param uri The URI to a content provider
     * @return The ContentProvider for the given URI, or null if no content provider is found.
     * @hide
     */
    CARAPI AcquireUnstableProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * @hide
     */
    CARAPI AcquireUnstableProvider(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    /**
     * Returns a {@link ContentProviderClient} that is associated with the {@link ContentProvider}
     * that services the content at uri, starting the provider if necessary. Returns
     * null if there is no provider associated wih the uri. The caller must indicate that they are
     * done with the provider by calling {@link ContentProviderClient#release} which will allow
     * the system to release the provider it it determines that there is no other reason for
     * keeping it active.
     * @param uri specifies which provider should be acquired
     * @return a {@link ContentProviderClient} that is associated with the {@link ContentProvider}
     * that services the content at uri or null if there isn't one.
     */
    CARAPI AcquireContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    /**
     * Returns a {@link ContentProviderClient} that is associated with the {@link ContentProvider}
     * with the authority of name, starting the provider if necessary. Returns
     * null if there is no provider associated wih the uri. The caller must indicate that they are
     * done with the provider by calling {@link ContentProviderClient#release} which will allow
     * the system to release the provider it it determines that there is no other reason for
     * keeping it active.
     * @param name specifies which provider should be acquired
     * @return a {@link ContentProviderClient} that is associated with the {@link ContentProvider}
     * with the authority of name or null if there isn't one.
     */
    CARAPI AcquireContentProviderClient(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    /**
     * Like {@link #acquireContentProviderClient(Uri)}, but for use when you do
     * not trust the stability of the target content provider.  This turns off
     * the mechanism in the platform clean up processes that are dependent on
     * a content provider if that content provider's process goes away.  Normally
     * you can safely assume that once you have acquired a provider, you can freely
     * use it as needed and it won't disappear, even if your process is in the
     * background.  If using this method, you need to take care to deal with any
     * failures when communicating with the provider, and be sure to close it
     * so that it can be re-opened later.  In particular, catching a
     * {@link android.os.DeadObjectException} from the calls there will let you
     * know that the content provider has gone away; at that point the current
     * ContentProviderClient object is invalid, and you should release it.  You
     * can acquire a new one if you would like to try to restart the provider
     * and perform new operations on it.
     */
    CARAPI AcquireUnstableContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    /**
     * Like {@link #acquireContentProviderClient(String)}, but for use when you do
     * not trust the stability of the target content provider.  This turns off
     * the mechanism in the platform clean up processes that are dependent on
     * a content provider if that content provider's process goes away.  Normally
     * you can safely assume that once you have acquired a provider, you can freely
     * use it as needed and it won't disappear, even if your process is in the
     * background.  If using this method, you need to take care to deal with any
     * failures when communicating with the provider, and be sure to close it
     * so that it can be re-opened later.  In particular, catching a
     * {@link android.os.DeadObjectException} from the calls there will let you
     * know that the content provider has gone away; at that point the current
     * ContentProviderClient object is invalid, and you should release it.  You
     * can acquire a new one if you would like to try to restart the provider
     * and perform new operations on it.
     */
    CARAPI AcquireUnstableContentProviderClient(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    /**
     * Register an observer class that gets callbacks when data identified by a
     * given content URI changes.
     *
     * @param uri The URI to watch for changes. This can be a specific row URI, or a base URI
     * for a whole class of content.
     * @param notifyForDescendents If <code>true</code> changes to URIs beginning with <code>uri</code>
     * will also cause notifications to be sent. If <code>false</code> only changes to the exact URI
     * specified by <em>uri</em> will cause notifications to be sent. If true, than any URI values
     * at or below the specified URI will also trigger a match.
     * @param observer The object that receives callbacks when changes occur.
     * @see #unregisterContentObserver
     */
    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer);

    /** @hide - designated user version */
    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Int32 userHandle);

    /**
     * Unregisters a change observer.
     *
     * @param observer The previously registered observer that is no longer needed.
     * @see #registerContentObserver
     */
    CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    /**
     * Notify registered observers that a row was updated and attempt to sync changes
     * to the network.
     * To register, call {@link #registerContentObserver(android.net.Uri , boolean, android.database.ContentObserver) registerContentObserver()}.
     * By default, CursorAdapter objects will get this notification.
     *
     * @param uri The uri of the content that was changed.
     * @param observer The observer that originated the change, may be <code>null</null>.
     * The observer that originated the change will only receive the notification if it
     * has requested to receive self-change notifications by implementing
     * {@link ContentObserver#deliverSelfNotifications()} to return true.
     */
    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer);

    /**
     * Notify registered observers that a row was updated.
     * To register, call {@link #registerContentObserver(android.net.Uri , boolean, android.database.ContentObserver) registerContentObserver()}.
     * By default, CursorAdapter objects will get this notification.
     * If syncToNetwork is true, this will attempt to schedule a local sync using the sync
     * adapter that's registered for the authority of the provided uri. No account will be
     * passed to the sync adapter, so all matching accounts will be synchronized.
     *
     * @param uri The uri of the content that was changed.
     * @param observer The observer that originated the change, may be <code>null</null>.
     * The observer that originated the change will only receive the notification if it
     * has requested to receive self-change notifications by implementing
     * {@link ContentObserver#deliverSelfNotifications()} to return true.
     * @param syncToNetwork If true, attempt to sync the change to the network.
     * @see #requestSync(android.accounts.Account, String, android.os.Bundle)
     */
    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork);

    /**
     * Notify registered observers within the designated user(s) that a row was updated.
     *
     * @hide
     */
    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork,
        /* [in] */ Int32 userHandle);

    /**
     * Take a persistable URI permission grant that has been offered. Once
     * taken, the permission grant will be remembered across device reboots.
     * Only URI permissions granted with
     * {@link Intent#FLAG_GRANT_PERSISTABLE_URI_PERMISSION} can be persisted. If
     * the grant has already been persisted, taking it again will touch
     * {@link UriPermission#getPersistedTime()}.
     *
     * @see #getPersistedUriPermissions()
     */
    CARAPI TakePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    /**
     * Relinquish a persisted URI permission grant. The URI must have been
     * previously made persistent with
     * {@link #takePersistableUriPermission(Uri, int)}. Any non-persistent
     * grants to the calling package will remain intact.
     *
     * @see #getPersistedUriPermissions()
     */
    CARAPI ReleasePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    /**
     * Return list of all URI permission grants that have been persisted by the
     * calling app. That is, the returned permissions have been granted
     * <em>to</em> the calling app. Only persistable grants taken with
     * {@link #takePersistableUriPermission(Uri, int)} are returned.
     *
     * @see #takePersistableUriPermission(Uri, int)
     * @see #releasePersistableUriPermission(Uri, int)
     */
    CARAPI GetPersistedUriPermissions(
        /* [out] */ IList** perms);

    /**
     * Return list of all persisted URI permission grants that are hosted by the
     * calling app. That is, the returned permissions have been granted
     * <em>from</em> the calling app. Only grants taken with
     * {@link #takePersistableUriPermission(Uri, int)} are returned.
     */
    CARAPI GetOutgoingPersistedUriPermissions(
        /* [out] */ IList** perms);


    /**
     * Start an asynchronous sync operation. If you want to monitor the progress
     * of the sync you may register a SyncObserver. Only values of the following
     * types may be used in the extras bundle:
     * <ul>
     * <li>Integer</li>
     * <li>Long</li>
     * <li>Boolean</li>
     * <li>Float</li>
     * <li>Double</li>
     * <li>String</li>
     * </ul>
     *
     * @param uri the uri of the provider to sync or null to sync all providers.
     * @param extras any extras to pass to the SyncAdapter.
     * @deprecated instead use
     * {@link #requestSync(android.accounts.Account, String, android.os.Bundle)}
     */
    CARAPI StartSync(
        /* [in] */ IUri* uri,
        /* [in] */ IBundle* extras);

    /**
     * Start an asynchronous sync operation. If you want to monitor the progress
     * of the sync you may register a SyncObserver. Only values of the following
     * types may be used in the extras bundle:
     * <ul>
     * <li>Integer</li>
     * <li>Long</li>
     * <li>Boolean</li>
     * <li>Float</li>
     * <li>Double</li>
     * <li>String</li>
     * </ul>
     *
     * @param account which account should be synced
     * @param authority which authority should be synced
     * @param extras any extras to pass to the SyncAdapter.
     */
    static CARAPI RequestSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    /**
     * @see #requestSync(Account, String, Bundle)
     * @hide
     */
    static CARAPI RequestSyncAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [in] */ IBundle* extras);

    /**
     * Register a sync with the SyncManager. These requests are built using the
     * {@link SyncRequest.Builder}.
     */
    static CARAPI RequestSync(
        /* [in] */ ISyncRequest* request);

    /**
     * Check that only values of the following types are in the Bundle:
     * <ul>
     * <li>Integer</li>
     * <li>Long</li>
     * <li>Boolean</li>
     * <li>Float</li>
     * <li>Double</li>
     * <li>String</li>
     * <li>Account</li>
     * <li>null</li>
     * </ul>
     * @param extras the Bundle to check
     */
    static CARAPI ValidateSyncExtrasBundle(
        /* [in] */ IBundle* extras);

    /**
     * Cancel any active or pending syncs that match the Uri. If the uri is null then
     * all syncs will be canceled.
     *
     * @param uri the uri of the provider to sync or null to sync all providers.
     * @deprecated instead use {@link #cancelSync(android.accounts.Account, String)}
     */
    CARAPI CancelSync(
        /* [in] */ IUri* uri);

    /**
     * Cancel any active or pending syncs that match account and authority. The account and
     * authority can each independently be set to null, which means that syncs with any account
     * or authority, respectively, will match.
     *
     * @param account filters the syncs that match by this account
     * @param authority filters the syncs that match by this authority
     */
    static CARAPI CancelSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority);

    /**
     * @see #cancelSync(Account, String)
     * @hide
     */
    static CARAPI CancelSyncAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId);

    /**
     * Get information about the SyncAdapters that are known to the system.
     * @return an array of SyncAdapters that have registered with the system
     */
    static CARAPI GetSyncAdapterTypes(
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** types);

    /**
     * @see #getSyncAdapterTypes()
     * @hide
     */
    static CARAPI GetSyncAdapterTypesAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** types);


    /**
     * Check if the provider should be synced when a network tickle is received
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_SETTINGS}.
     *
     * @param account the account whose setting we are querying
     * @param authority the provider whose setting we are querying
     * @return true if the provider should be synced when a network tickle is received
     */
    static CARAPI GetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSynced);

    /**
     * @see #getSyncAutomatically(Account, String)
     * @hide
     */
    static CARAPI GetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* isSynced);

    /**
     * Set whether or not the provider is synced when it receives a network tickle.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     *
     * @param account the account whose setting we are querying
     * @param authority the provider whose behavior is being controlled
     * @param sync true if the provider should be synced when tickles are received for it
     */
    static CARAPI SetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Boolean sync);

    /**
     * @see #setSyncAutomatically(Account, String, boolean)
     * @hide
     */
    static CARAPI SetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean sync);

    /**
     * Specifies that a sync should be requested with the specified the account, authority,
     * and extras at the given frequency. If there is already another periodic sync scheduled
     * with the account, authority and extras then a new periodic sync won't be added, instead
     * the frequency of the previous one will be updated.
     * <p>
     * These periodic syncs honor the "syncAutomatically" and "masterSyncAutomatically" settings.
     * Although these sync are scheduled at the specified frequency, it may take longer for it to
     * actually be started if other syncs are ahead of it in the sync operation queue. This means
     * that the actual start time may drift.
     * <p>
     * Periodic syncs are not allowed to have any of {@link #SYNC_EXTRAS_DO_NOT_RETRY},
     * {@link #SYNC_EXTRAS_IGNORE_BACKOFF}, {@link #SYNC_EXTRAS_IGNORE_SETTINGS},
     * {@link #SYNC_EXTRAS_INITIALIZE}, {@link #SYNC_EXTRAS_FORCE},
     * {@link #SYNC_EXTRAS_EXPEDITED}, {@link #SYNC_EXTRAS_MANUAL} set to true.
     * If any are supplied then an {@link IllegalArgumentException} will be thrown.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     *
     * @param account the account to specify in the sync
     * @param authority the provider to specify in the sync request
     * @param extras extra parameters to go along with the sync request
     * @param pollFrequency how frequently the sync should be performed, in seconds.
     * @throws IllegalArgumentException if an illegal extra was set or if any of the parameters
     * are null.
     */
    static CARAPI AddPeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 pollFrequency);

    /**
     * {@hide}
     * Helper function to throw an <code>IllegalArgumentException</code> if any illegal
     * extras were set for a periodic sync.
     *
     * @param extras bundle to validate.
     */
    static CARAPI InvalidPeriodicExtras(
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);


    /**
     * Remove a periodic sync. Has no affect if account, authority and extras don't match
     * an existing periodic sync.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     *
     * @param account the account of the periodic sync to remove
     * @param authority the provider of the periodic sync to remove
     * @param extras the extras of the periodic sync to remove
     */
    static CARAPI RemovePeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    /**
     * Remove the specified sync. This will cancel any pending or active syncs. If the request is
     * for a periodic sync, this call will remove any future occurrences.
     * <p>
     *     If a periodic sync is specified, the caller must hold the permission
     *     {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     *</p>
     * It is possible to cancel a sync using a SyncRequest object that is not the same object
     * with which you requested the sync. Do so by building a SyncRequest with the same
     * adapter, frequency, <b>and</b> extras bundle.
     *
     * @param request SyncRequest object containing information about sync to cancel.
     */
    static CARAPI CancelSync(
        /* [in] */ ISyncRequest* request);


    /**
     * Get the list of information about the periodic syncs for the given account and authority.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_SETTINGS}.
     *
     * @param account the account whose periodic syncs we are querying
     * @param authority the provider whose periodic syncs we are querying
     * @return a list of PeriodicSync objects. This list may be empty but will never be null.
     */
    static CARAPI GetPeriodicSyncs(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ IList** periodicSyncs);

    /**
     * Check if this account/provider is syncable.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_SETTINGS}.
     * @return >0 if it is syncable, 0 if not, and <0 if the state isn't known yet.
     */
    static CARAPI GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Int32* isSyncable);

    /**
     * @see #getIsSyncable(Account, String)
     * @hide
     */
    static CARAPI GetIsSyncableAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* isSyncable);

    /**
     * Set whether this account/provider is syncable.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     * @param syncable >0 denotes syncable, 0 means not syncable, <0 means unknown
     */
    static CARAPI SetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 syncable);

    /**
     * Gets the master auto-sync setting that applies to all the providers and accounts.
     * If this is false then the per-provider auto-sync setting is ignored.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_SETTINGS}.
     *
     * @return the master auto-sync setting that applies to all the providers and accounts
     */
    static CARAPI GetMasterSyncAutomatically(
        /* [out] */ Boolean* result);

    /**
     * @see #getMasterSyncAutomatically()
     * @hide
     */
    static CARAPI GetMasterSyncAutomaticallyAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /**
     * Sets the master auto-sync setting that applies to all the providers and accounts.
     * If this is false then the per-provider auto-sync setting is ignored.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#WRITE_SYNC_SETTINGS}.
     *
     * @param sync the master auto-sync setting that applies to all the providers and accounts
     */
    static CARAPI SetMasterSyncAutomatically(
        /* [in] */ Boolean sync);

    /**
     * @see #setMasterSyncAutomatically(boolean)
     * @hide
     */
    static CARAPI SetMasterSyncAutomaticallyAsUser(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean sync);

    /**
     * Returns true if there is currently a sync operation for the given
     * account or authority in the pending list, or actively being processed.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_STATS}.
     * @param account the account whose setting we are querying
     * @param authority the provider whose behavior is being queried
     * @return true if a sync is active for the given account or authority.
     */
    static CARAPI IsSyncActive(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSyncActive);

    /**
     * If a sync is active returns the information about it, otherwise returns null.
     * <p>
     * This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_STATS}.
     * <p>
     * @return the SyncInfo for the currently active sync or null if one is not active.
     * @deprecated
     * Since multiple concurrent syncs are now supported you should use
     * {@link #getCurrentSyncs()} to get the accurate list of current syncs.
     * This method returns the first item from the list of current syncs
     * or null if there are none.
     */
    static CARAPI GetCurrentSync(
        /* [out] */ ISyncInfo** syncInfo);

    /**
     * Returns a list with information about all the active syncs. This list will be empty
     * if there are no active syncs.
     * <p>
     * This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_STATS}.
     * <p>
     * @return a List of SyncInfo objects for the currently active syncs.
     */
    static CARAPI GetCurrentSyncsAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ IList** syncInfoList);

    /**
     * @see #getCurrentSyncs()
     * @hide
     */
    static CARAPI GetCurrentSyncs(
        /* [out] */ IList** syncInfoList);

    /**
     * Returns the status that matches the authority.
     * @param account the account whose setting we are querying
     * @param authority the provider whose behavior is being queried
     * @return the SyncStatusInfo for the authority, or null if none exists
     * @hide
     */
    static CARAPI GetSyncStatus(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ ISyncStatusInfo** syncStatusInfo);

    /**
     * @see #getSyncStatus(Account, String)
     * @hide
     */
    static CARAPI GetSyncStatusAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [out] */ ISyncStatusInfo** syncStatusInfo);

    /**
     * Return true if the pending status is true of any matching authorities.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#READ_SYNC_STATS}.
     * @param account the account whose setting we are querying
     * @param authority the provider whose behavior is being queried
     * @return true if there is a pending sync with the matching account and authority
     */
    static CARAPI IsSyncPending(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [out] */ Boolean* isSyncPending);

    /**
     * @see #requestSync(Account, String, Bundle)
     * @hide
     */
    static CARAPI IsSyncPendingAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* isSyncPending);


    /**
     * Request notifications when the different aspects of the SyncManager change. The
     * different items that can be requested are:
     * <ul>
     * <li> {@link #SYNC_OBSERVER_TYPE_PENDING}
     * <li> {@link #SYNC_OBSERVER_TYPE_ACTIVE}
     * <li> {@link #SYNC_OBSERVER_TYPE_SETTINGS}
     * </ul>
     * The caller can set one or more of the status types in the mask for any
     * given listener registration.
     * @param mask the status change types that will cause the callback to be invoked
     * @param callback observer to be invoked when the status changes
     * @return a handle that can be used to remove the listener at a later time
     */
    static CARAPI AddStatusChangeListener(
        /* [in] */ Int32 mask,
        /* [in] */ ISyncStatusObserver* observerCallback,
        /* [out] */ IInterface** item);

    /**
     * Remove a previously registered status change listener.
     * @param handle the handle that was returned by {@link #addStatusChangeListener}
     */
    static CARAPI RemoveStatusChangeListener(
        /* [in] */ IInterface* handle);

    /** @hide */
    static CARAPI GetContentService(
        /* [out] */ IIContentService** contentService);

    /** @hide */
    CARAPI GetPackageName(
        /* [out] */ String* package);

    /** @hide */
    CARAPI ResolveUserId(
        /* [in] */ IUri* uri,
        /* [out] */ Int32* id);

protected:
    /** @hide */
    virtual CARAPI AcquireProvider(
        /* [in] */ IContext* c,
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider) = 0;

    /** Providing a default implementation of this, to avoid having to change
     * a lot of other things, but implementations of ContentResolver should
     * implement it. @hide */
    virtual CARAPI AcquireExistingProvider(
        /* [in] */ IContext* c,
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider) = 0;

    /** @hide */
    virtual CARAPI AcquireUnstableProvider(
        /* [in] */ IContext* c,
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider) = 0;

private:
    /**
     * Returns sampling percentage for a given duration.
     *
     * Always returns at least 1%.
     */
    CARAPI_(Int32) SamplePercentForDuration(
        /* [in] */ Int64 durationMillis);

    CARAPI MaybeLogQueryToEventLog(
        /* [in] */ Int64 durationMillis,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder);

    CARAPI MaybeLogUpdateToEventLog(
        /* [in] */ Int64 durationMillis,
        /* [in] */ IUri* uri,
        /* [in] */ const String& operation,
        /* [in] */ const String& selection);

public:
    /** @hide */
    static AutoPtr<IIntent> ACTION_SYNC_CONN_STATUS_CHANGED;

private:
    static const AutoPtr<ArrayOf<String> > SYNC_ERROR_NAMES;

    // Always log queries which take 500ms+; shorter queries are
    // sampled accordingly.
    static const Boolean ENABLE_CONTENT_SAMPLE;// = false;
    static const Int32 SLOW_THRESHOLD_MILLIS;// = 500;
    AutoPtr<IRandom> mRandom;  // guarded by itself

    static AutoPtr<IIContentService> sContentService;
    IContext* mContext;
    String mPackageName;
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_CONTENTRESOLVER_H__
