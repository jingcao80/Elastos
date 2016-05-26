#ifndef __ELASTOS_UTILITY_PREFS_PREFERENCES_H__
#define __ELASTOS_UTILITY_PREFS_PREFERENCES_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Utility {
namespace Prefs {

/**
 * An instance of the class {@code Preferences} represents one node in a
 * preference tree, which provides a mechanism to store and access configuration
 * data in a hierarchical way. Two hierarchy trees are maintained, one for
 * system preferences shared by all users and the other for user preferences
 * specific to the user. {@code Preferences} hierarchy trees and data are stored
 * in an implementation-dependent back-end.
 * <p>
 * Every node has one name and one unique absolute path following the same
 * notational conventions as directories in a file system. The root node's
 * name is "", and other node name strings cannot contain the slash character
 * and cannot be empty. The root node's absolute path is "/", and all other
 * nodes' absolute paths are constructed in the standard way: &lt;parent's
 * absolute path&gt; + "/" + &lt;node's name&gt;. Since the set of nodes forms a
 * tree with the root node at its base, all absolute paths start with the slash
 * character. Every node has one relative path to each of its ancestors. The
 * relative path doesn't start with slash: it equals the node's absolute path
 * with leading substring removed corresponding to the ancestor's absolute path
 * and a slash.
 * <p>
 * Modification to preferences data may be asynchronous, which means that
 * preference update method calls may return immediately instead of blocking.
 * The {@code flush()} and {@code sync()} methods force the back-end to
 * synchronously perform all pending updates, but the implementation is
 * permitted to perform the modifications on the underlying back-end data
 * at any time between the moment the request is made and the moment the
 * {@code flush()} or {@code sync()} method returns. Please note that if the JVM
 * exits normally, the implementation must assure all modifications are
 * persisted implicitly.
 * <p>
 * When invoking a method that retrieves preferences, the user must provide
 * a default value. The default value is returned when the preferences cannot
 * be found or the back-end is unavailable. Some other methods will throw
 * {@code BackingStoreException} when the back-end is unavailable.
 * </p>
 * <p>
 * Preferences can be exported to and imported from an XML files. These
 * documents must have an XML DOCTYPE declaration:
 * <pre>{@code
 * <!DOCTYPE preferences SYSTEM "http://java.sun.com/dtd/preferences.dtd">
 * }</pre>
 * This system URI is not really accessed by network, it is only a
 * identification string. Visit the DTD location to see the actual format
 * permitted.
 * <p>
 * There must be a concrete {@code PreferencesFactory} type for every concrete
 * {@code Preferences} type developed. Every J2SE implementation must provide a
 * default implementation for every supported platform, and must also provide a
 * means of replacing the default implementation. This implementation uses the
 * system property {@code java.util.prefs.PreferencesFactory} to determine which
 * preferences implementation to use.
 * <p>
 * The methods of this class are thread-safe. If multiple JVMs are using the
 * same back-end concurrently, the back-end won't be corrupted, but no other
 * behavior guarantees are made.
 *
 * @see PreferencesFactory
 *
 * @since 1.4
 */
class Preferences
    : public Object
    , public IPreferences
{
public:
    CAR_INTERFACE_DECL()

    /**
     * @hide for testing only.
     */
    static CARAPI_(AutoPtr<IPreferencesFactory>) SetPreferencesFactory(
        /* [in] */ IPreferencesFactory* pf);

    /**
     * Gets the absolute path string of this preference node.
     *
     * @return the preference node's absolute path string.
     */
    virtual CARAPI GetAbsolutePath(
        /* [out] */ String* path) = 0;

    /**
     * Returns the names of all children of this node or an empty array if this
     * node has no children.
     *
     * @return the names of all children of this node.
     * @throws BackingStoreException
     *             if backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI GetChildrenNames(
        /* [out, callee] */ ArrayOf<String>** outarr) = 0/*throws BackingStoreException*/;

    /**
     * Removes all preferences of this node.
     *
     * @throws BackingStoreException
     *             if backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI Clear() = 0/*throws BackingStoreException;*/;

    /**
     * Exports all of the preferences of this node to a XML document using the
     * given output stream.
     * <p>
     * This XML document uses the UTF-8 encoding and is written according to the
     * DTD in its DOCTYPE declaration, which is the following:
     *
     * <pre>
     * &lt;!DOCTYPE preferences SYSTEM &quot;http://java.sun.com/dtd/preferences.dtd&quot;&gt;
     * </pre>
     *
     * <i>Please note that (unlike the methods of this class that don't concern
     * serialization), this call is not thread-safe.</i>
     * </p>
     *
     * @param ostream
     *            the output stream to write the XML-formatted data to.
     * @throws IOException
     *             if an error occurs while exporting.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI ExportNode(
        /* [in] */ IOutputStream* ostream) = 0 /*throws IOException, BackingStoreException*/;

    /**
     * Exports all of the preferences of this node and all its descendants to a
     * XML document using the given output stream.
     * <p>
     * This XML document uses the UTF-8 encoding and is written according to the
     * DTD in its DOCTYPE declaration, which is the following:
     *
     * <pre>
     * &lt;!DOCTYPE preferences SYSTEM &quot;http://java.sun.com/dtd/preferences.dtd&quot;&gt;
     * </pre>
     *
     * <i>Please note that (unlike the methods of this class that don't concern
     * serialization), this call is not thread-safe.</i>
     * </p>
     *
     * @param ostream
     *            the output stream to write the XML-formatted data to.
     * @throws IOException
     *             if an error occurs while exporting.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI ExportSubtree(
        /* [in] */ IOutputStream* ostream) = 0 /*throws IOException, BackingStoreException*/;

    /**
     * Forces all pending updates to this node and its descendants to be
     * persisted in the backing store.
     * <p>
     * If this node has been removed, the invocation of this method only flushes
     * this node, not its descendants.
     * </p>
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI Flush() = 0/*throws BackingStoreException*/;

    /**
     * Gets the {@code String} value mapped to the given key or its default
     * value if no value is mapped or no backing store is available.
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key or no backing store is available.
     * @return the preference value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI Get(
        /* [in] */ const String& key,
        /* [in] */ const String& deflt,
        /* [out] */ String* value) = 0;

    /**
     * Gets the {@code boolean} value mapped to the given key or its default
     * value if no value is mapped, if the backing store is unavailable, or if
     * the value is invalid.
     * <p>
     * The only valid values are the {@code String} "true", which represents
     * {@code true} and "false", which represents {@code false}, ignoring case.
     * </p>
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable,
     *            or if the value is invalid.
     * @return the boolean value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean deflt,
        /* [out] */ Boolean* value) = 0;

    /**
     * Gets the {@code byte} array value mapped to the given key or its default
     * value if no value is mapped, if the backing store is unavailable, or if
     * the value is an invalid string.
     * <p>
     * To be valid, the value string must be Base64-encoded binary data. The
     * Base64 encoding is as defined in <a
     * href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>, section 6.8.
     * </p>
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable,
     *            or if the value is invalid.
     * @return the byte array value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetByteArray(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* deflt,
        /* [out] */ ArrayOf<Byte>** outarr) = 0;

    /**
     * Gets the {@code double} value mapped to the given key or its default
     * value if no value is mapped, if the backing store is unavailable, or if
     * the value is an invalid string.
     * <p>
     * To be valid, the value string must be a string that can be converted to a
     * {@code double} by {@link Double#parseDouble(String)
     * Double.parseDouble(String)}.
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable, or if the
     *            value is invalid.
     * @return the double value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetDouble(
        /* [in] */ const String& key,
        /* [in] */ Double deflt,
        /* [out] */ Double* value) = 0;

    /**
     * Gets the {@code float} value mapped to the given key or its default value
     * if no value is mapped, if the backing store is unavailable, or if the
     * value is an invalid string.
     * <p>
     * To be valid, the value string must be a string that can be converted to a
     * {@code float} by {@link Float#parseFloat(String)
     * Float.parseFloat(String)}.
     * </p>
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable, or if the
     *            value is invalid.
     * @return the float value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetFloat(
        /* [in] */ const String& key,
        /* [in] */ Float deflt,
        /* [out] */ Float* value) = 0;

    /**
     * Gets the {@code Int32} value mapped to the given key or its default value
     * if no value is mapped, if the backing store is unavailable, or if the
     * value is an invalid string.
     * <p>
     * To be valid, the value string must be a string that can be converted to
     * an {@code Int32} by {@link Integer#parseInt(String)
     * Integer.parseInt(String)}.
     * </p>
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable,
     *            or if the value is invalid.
     * @return the integer value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 deflt,
        /* [out] */ Int32* value) = 0;

    /**
     * Gets the {@code long} value mapped to the given key or its default value
     * if no value is mapped, if the backing store is unavailable, or if the
     * value is an invalid string.
     * <p>
     * To be valid, the value string must be a string that can be converted to a
     * {@code long} by {@link Long#parseLong(String) Long.parseLong(String)}.
     * </p>
     * <p>
     * Some implementations may store default values in backing stores. In this
     * case, if there is no value mapped to the given key, the stored default
     * value is returned.
     * </p>
     *
     * @param key
     *            the preference key.
     * @param deflt
     *            the default value, which will be returned if no value is
     *            mapped to the given key, if the backing store is unavailable,
     *            or if the value is invalid.
     * @return the long value mapped to the given key.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws NullPointerException
     *             if the parameter {@code key} is {@code null}.
     */
    virtual CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 deflt,
        /* [out] */ Int64* value) = 0;

    /**
     * Imports all the preferences from an XML document using the given input
     * stream.
     * <p>
     * This XML document uses the UTF-8 encoding and must be written according
     * to the DTD in its DOCTYPE declaration, which must be the following:
     *
     * <pre>
     * &lt;!DOCTYPE preferences SYSTEM &quot;http://java.sun.com/dtd/preferences.dtd&quot;&gt;
     * </pre>
     *
     * <i>Please note that (unlike the methods of this class that don't concern
     * serialization), this call is not thread-safe.</i>
     * </p>
     *
     * @param istream
     *            the input stream to read the data from.
     * @throws InvalidPreferencesFormatException
     *             if the data read from the given input stream is not from a
     *             valid XML document.
     * @throws IOException
     *             if an error occurs while importing.
     */
    static CARAPI ImportPreferences(
        /* [in] */ IInputStream* istream)/* throws InvalidPreferencesFormatException, IOException*/;

    /**
     * Returns whether this is a user preference node.
     *
     * @return {@code true}, if this is a user preference node, {@code false} if
     *         this is a system preference node.
     */
    virtual CARAPI IsUserNode(
        /* [out] */ Boolean* isUserNode) = 0;

    /**
     * Returns all preference keys stored in this node or an empty array if no
     * key was found.
     *
     * @return the list of all preference keys of this node.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI GetKeys(
        /* [out, callee] */ ArrayOf<String>** outarr) = 0/* throws BackingStoreException*/;

    /**
     * Returns the name of this node.
     *
     * @return the name of this node.
     */
    virtual CARAPI GetName(
        /* [out] */ String* name) = 0;

    /**
     * Returns the preference node with the given path name. The path name can
     * be relative or absolute. The requested node and its ancestors will
     * be created if they do not exist.
     * <p>
     * The path is treated as relative to this node if it doesn't start with a
     * slash, otherwise it will be treated as an absolute path.
     * </p>
     *
     * @param path
     *            the path name of the requested preference node.
     * @return the requested preference node.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws IllegalArgumentException
     *             if the path name is invalid.
     * @throws NullPointerException
     *             if the given path is {@code null}.
     */
    virtual CARAPI GetNode(
        /* [in] */ const String& path,
        /* [out] */ IPreferences** pfs) = 0;

    /**
     * Returns whether the preference node with the given path name exists. The
     * path is treated as relative to this node if it doesn't start with a slash,
     * otherwise it is treated as an absolute path.
     * <p>
     * Please note that if this node has been removed, an invocation of this
     * node will throw an {@code IllegalStateException} unless the given path is
     * an empty string, which will return {@code false}.
     * </p>
     *
     * @param path
     *            the path name of the preference node to query.
     * @return {@code true}, if the queried preference node exists, {@code false}
     *         otherwise.
     * @throws IllegalStateException
     *             if this node has been removed and the path is not an empty
     *             string.
     * @throws IllegalArgumentException
     *             if the path name is invalid.
     * @throws NullPointerException
     *             if the given path is {@code null}.
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     */
    virtual CARAPI IsNodeExists(
        /* [in] */ const String& path,
        /* [out] */ Boolean* exists) = 0/* throws BackingStoreException*/;

    /**
     * Returns the parent preference node of this node or {@code null} if this
     * node is the root node.
     *
     * @return the parent preference node of this node.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI GetParent(
        /* [out] */ IPreferences** pfs) = 0;

    /**
     * Adds a new preference to this node using the given key and value or
     * updates the value if a preference with the given key already exists.
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key or value is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH} or the value's length is bigger than {@code
     *             MAX_VALUE_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value) = 0;

    /**
     * Adds a new preference with a {@code boolean} value to this node using the
     * given key and value or updates the value if a preference with the given
     * key already exists.
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference {@code boolean} value for the given key.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean value) = 0;

    /**
     * Adds a new preference to this node using the given key and the string
     * form of the given value or updates the value if a preference with the
     * given key already exists.
     * <p>
     * The string form of the value is the Base64-encoded binary data of the
     * given byte array. The Base64 encoding is as defined in <a
     * href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>, section 6.8.
     * </p>
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key or value is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH} or value's length is bigger than three
     *             quarters of {@code MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutByteArray(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value) = 0;

    /**
     * Adds a new preference to this node using the given key and {@code double}
     * value or updates the value if a preference with the
     * given key already exists.
     * <p>
     * The value is stored in its string form, which is the result of invoking
     * {@link Double#toString(double) Double.toString(double)}.
     * </p>
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutDouble(
        /* [in] */ const String& key,
        /* [in] */ Double value) = 0;

    /**
     * Adds a new preference to this node using the given key and {@code float}
     * value or updates the value if a preference with the
     * given key already exists.
     * <p>
     * The value is stored in its string form, which is the result of invoking
     * {@link Float#toString(float) Float.toString(float)}.
     * </p>
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutFloat(
        /* [in] */ const String& key,
        /* [in] */ Float value) = 0;

    /**
     * Adds a new preference to this node using the given key and {@code Int32}
     * value or updates the value if a preference with the
     * given key already exists.
     * <p>
     * The value is stored in its string form, which is the result of invoking
     * {@link Integer#toString(Int32) Integer.toString(Int32)}.
     * </p>
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 value) = 0;

    /**
     * Adds a new preference to this node using the given key and {@code long}
     * value or updates the value if a preference with the
     * given key already exists.
     * <p>
     * The value is stored in its string form, which is the result of invoking
     * {@link Long#toString(long) Long.toString(long)}.
     * </p>
     *
     * @param key
     *            the preference key to be added or updated.
     * @param value
     *            the preference value for the given key.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalArgumentException
     *             if the given key's length is bigger than {@code
     *             MAX_KEY_LENGTH}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI PutInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 value) = 0;

    /**
     * Removes the preference mapped to the given key from this node.
     *
     * @param key
     *            the key of the preference to be removed.
     * @throws NullPointerException
     *             if the given key is {@code null}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI Remove(
        /* [in] */ const String& key) = 0;

    /**
     * Removes this preference node with all its descendants. The removal won't
     * necessarily be persisted until the method {@code flush()} is invoked.
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     * @throws UnsupportedOperationException
     *             if this is a root node.
     */
    virtual CARAPI RemoveNode() = 0 /*throws BackingStoreException*/;

    /**
     * Registers a {@code NodeChangeListener} instance for this node, which will
     * handle {@code NodeChangeEvent}s. {@code NodeChangeEvent}s will be fired
     * when a child node has been added to or removed from this node.
     *
     * @param ncl
     *            the listener to be registered.
     * @throws NullPointerException
     *             if the given listener is {@code null}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI AddNodeChangeListener(
        /* [in] */ INodeChangeListener* ncl) = 0;

    /**
     * Registers a {@code PreferenceChangeListener} instance for this node,
     * which will handle {@code PreferenceChangeEvent}s. {@code
     * PreferenceChangeEvent}s will be fired when a preference has been added
     * to, removed from, or updated for this node.
     *
     * @param pcl
     *            the listener to be registered.
     * @throws NullPointerException
     *             if the given listener is {@code null}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI AddPreferenceChangeListener(
        /* [in] */ IPreferenceChangeListener* pcl) = 0;

    /**
     * Removes the given {@code NodeChangeListener} instance from this node.
     *
     * @param ncl
     *            the listener to be removed.
     * @throws IllegalArgumentException
     *             if the given listener is {@code null}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI RemoveNodeChangeListener(
        /* [in] */ INodeChangeListener* ncl) = 0;

    /**
     * Removes the given {@code PreferenceChangeListener} instance from this
     * node.
     *
     * @param pcl
     *            the listener to be removed.
     * @throws IllegalArgumentException
     *             if the given listener is {@code null}.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI RemovePreferenceChangeListener(
        /* [in] */ IPreferenceChangeListener* pcl) = 0;

    /**
     * Synchronizes the data of this preference node and its descendants with
     * the back-end preference store. Any changes found in the back-end data
     * should be reflected in this node and its descendants, and at the same
     * time any local changes to this node and descendants should be persisted.
     *
     * @throws BackingStoreException
     *             if the backing store is unavailable or causes an operation
     *             failure.
     * @throws IllegalStateException
     *             if this node has been removed.
     */
    virtual CARAPI Sync() = 0/* throws BackingStoreException*/;

    /**
     * <strong>Legacy code; do not use.</strong> On Android, the Preference nodes
     * corresponding to the "system" and "user" preferences are stored in sections
     * of the file system that are inaccessible to apps. Further, allowing apps to set
     * "system wide" preferences is contrary to android's security model.
     *
     * Returns the system preference node for the package of the given class.
     * The absolute path of the returned node is one slash followed by the given
     * class's full package name, replacing each period character ('.') with
     * a slash. For example, the absolute path of the preference associated with
     * the class Object would be "/java/lang". As a special case, the unnamed
     * package is associated with a preference node "/&lt;unnamed&gt;". This
     * method will create the node and its ancestors as needed. Any nodes created
     * by this method won't necessarily be persisted until the method {@code
     * flush()} is invoked.
     *
     * @param c
     *            the given class.
     * @return the system preference node for the package of the given class.
     * @throws NullPointerException
     *             if the given class is {@code null}.
     */
    static CARAPI SystemNodeForPackage(
        /* [in] */ IInterface* c,
        /* [out] */ IPreferences** outpre);

    /**
     * <strong>Legacy code; do not use.</strong> On Android, the Preference nodes
     * corresponding to the "system" and "user" preferences are stored in sections
     * of the file system that are inaccessible to apps. Further, allowing apps to set
     * "system wide" preferences is contrary to android's security model.
     *
     * Returns the root node of the system preference hierarchy.
     *
     * @return the system preference hierarchy root node.
     */
    static CARAPI SystemRoot(
        /* [out] */ IPreferences** outpre);

    /**
     *
     * <strong>Legacy code; do not use.</strong> On Android, the Preference nodes
     * corresponding to the "system" and "user" preferences are stored in sections
     * of the file system that are inaccessible to apps. Further, allowing apps to set
     * "system wide" preferences is contrary to android's security model.
     *
     * <p>
     * Returns the user preference node for the package of the given class.
     * The absolute path of the returned node is one slash followed by the given
     * class's full package name, replacing each period character ('.') with
     * a slash. For example, the absolute path of the preference associated with
     * the class Object would be "/java/lang". As a special case, the unnamed
     * package is associated with a preference node "/&lt;unnamed&gt;". This
     * method will create the node and its ancestors as needed. Any nodes created
     * by this method won't necessarily be persisted until the method {@code
     * flush()} is invoked.
     *
     * @return the user preference node for the package of the given class.
     * @throws NullPointerException
     *             if the given class is {@code null}.
     */
    static CARAPI UserNodeForPackage(
        /* [in] */ IInterface* c,
        /* [out] */ IPreferences** outpre);

    /**
     * <strong>Legacy code; do not use.</strong> On Android, the Preference nodes
     * corresponding to the "system" and "user" preferences are stored in sections
     * of the file system that are inaccessible to apps. Further, allowing apps to set
     * "system wide" preferences is contrary to android's security model.
     *
     * Returns the root node of the user preference hierarchy.
     *
     * @return the user preference hierarchy root node.
     */
    static CARAPI UserRoot(
        /* [out] */ IPreferences** outpre);

    /**
     * Returns a string representation of this node. The format is "User/System
     * Preference Node: " followed by this node's absolute path.
     *
     * @return the string representation of this node.
     */
    // @Override
    // public abstract String toString();
    virtual CARAPI ToString(
        /* [out] */ String* value) = 0;

protected:
    /**
     * Default constructor, for use by subclasses only.
     */
    Preferences();

    virtual ~Preferences();

    static CARAPI_(AutoPtr<IPreferencesFactory>) FindPreferencesFactory();

private:
    //parse node's absolute path from class instance
    static CARAPI_(String) GetNodeName(
        /* [in] */ IInterface* c);

public:
    /**
     * Maximum size in characters allowed for a preferences key.
     */
    static const Int32 MAX_KEY_LENGTH;

    /**
     * Maximum size in characters allowed for a preferences name.
     */
    static const Int32 MAX_NAME_LENGTH;

    /**
     * Maximum size in characters allowed for a preferences value.
     */
    static const Int32 MAX_VALUE_LENGTH;

private:
    // factory used to get user/system prefs root
    static AutoPtr<IPreferencesFactory> mFactory;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_PREFERENCES_H__
