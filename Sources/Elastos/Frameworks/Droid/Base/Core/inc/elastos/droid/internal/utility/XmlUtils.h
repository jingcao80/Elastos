#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_XMLUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_XMLUTILS_H__

#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class ECO_PUBLIC XmlUtils
{
public:
    static CARAPI SkipCurrentTag(
        /* [in] */ IXmlPullParser* parser);

    static CARAPI_(Int32) ConvertValueToList(
        /* [in] */ ICharSequence* value,
        /* [in] */ const ArrayOf<String>& options,
        /* [in] */ Int32 defaultValue);

    static CARAPI_(Boolean) ConvertValueToBoolean(
        /* [in] */ const String& value,
        /* [in] */ Boolean defaultValue);

    static CARAPI_(Boolean) ConvertValueToBoolean(
        /* [in] */ ICharSequence* value,
        /* [in] */ Boolean defaultValue);

    static CARAPI_(Int32) ConvertValueToInt32(
        /* [in] */ ICharSequence* charSeq,
        /* [in] */ Int32 defaultValue);

    static CARAPI_(Int32) ConvertValueToUnsignedInt(
        /* [in] */ const String& value,
        /* [in] */ Int32 defaultValue);

    static CARAPI_(Int32) ParseUnsignedIntAttribute(
        /* [in] */ const String& charSeq);

    /**
     * Flatten a Map into an output stream as XML.  The map can later be
     * read back with readMapXml().
     *
     * @param val The map to be flattened.
     * @param out Where to write the XML data.
     *
     * @see #writeMapXml(Map, String, XmlSerializer)
     * @see #writeListXml
     * @see #writeValueXml
     * @see #readMapXml
     */
    static CARAPI WriteMapXml(
        /* [in] */ IMap* val,
        /* [in] */ IOutputStream* out);

    /**
     * Flatten a List into an output stream as XML.  The list can later be
     * read back with readListXml().
     *
     * @param val The list to be flattened.
     * @param out Where to write the XML data.
     *
     * @see #writeListXml(List, String, XmlSerializer)
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readListXml
     */
    static CARAPI WriteListXml(
        /* [in] */ IList* val,
        /* [in] */ IOutputStream* out);

    /**
     * Flatten a Map into an XmlSerializer.  The map can later be read back
     * with readThisMapXml().
     *
     * @param val The map to be flattened.
     * @param name Name attribute to include with this list's tag, or NULL for
     *             none.
     * @param out XmlSerializer to write the map into.
     *
     * @see #writeMapXml(Map, OutputStream)
     * @see #writeListXml
     * @see #writeValueXml
     * @see #readMapXml
     */
    static CARAPI WriteMapXml(
        /* [in] */ IMap* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten a Map into an XmlSerializer.  The map can later be read back
     * with readThisMapXml().
     *
     * @param val The map to be flattened.
     * @param name Name attribute to include with this list's tag, or null for
     *             none.
     * @param out XmlSerializer to write the map into.
     * @param callback Method to call when an Object type is not recognized.
     *
     * @see #writeMapXml(Map, OutputStream)
     * @see #writeListXml
     * @see #writeValueXml
     * @see #readMapXml
     *
     * @hide
     */
    static CARAPI WriteMapXml(
        /* [in] */ IMap* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out,
        /* [in] */ IXmlUtilsWriteMapCallback* callback);

    /**
     * Flatten a Map into an XmlSerializer.  The map can later be read back
     * with readThisMapXml(). This method presumes that the start tag and
     * name attribute have already been written and does not write an end tag.
     *
     * @param val The map to be flattened.
     * @param out XmlSerializer to write the map into.
     *
     * @see #writeMapXml(Map, OutputStream)
     * @see #writeListXml
     * @see #writeValueXml
     * @see #readMapXml
     *
     * @hide
     */
    static CARAPI WriteMapXml(
        /* [in] */ IMap* val,
        /* [in] */ IXmlSerializer* out,
        /* [in] */ IXmlUtilsWriteMapCallback* callback);

    /**
     * Flatten a List into an XmlSerializer.  The list can later be read back
     * with readThisListXml().
     *
     * @param val The list to be flattened.
     * @param name Name attribute to include with this list's tag, or NULL for
     *             none.
     * @param out XmlSerializer to write the list into.
     *
     * @see #writeListXml(List, OutputStream)
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readListXml
     */
    static CARAPI WriteListXml(
        /* [in] */ IList* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    static CARAPI WriteSetXml(
        /* [in] */ ISet* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten a byte[] into an XmlSerializer.  The list can later be read back
     * with readThisByteArrayXml().
     *
     * @param val The byte array to be flattened.
     * @param name Name attribute to include with this array's tag, or NULL for
     *             none.
     * @param out XmlSerializer to write the array into.
     *
     * @see #writeMapXml
     * @see #writeValueXml
     */
    static CARAPI WriteByteArrayXml(
        /* [in] */ ArrayOf<Byte>* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten an Int32[] into an XmlSerializer.  The list can later be read back
     * with readThisIntArrayXml().
     *
     * @param val The Int32 array to be flattened.
     * @param name Name attribute to include with this array's tag, or NULL for
     *             none.
     * @param out XmlSerializer to write the array into.
     *
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readThisIntArrayXml
     */
    static CARAPI WriteInt32ArrayXml(
        /* [in] */ ArrayOf<Int32>* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten a Int64[] into an XmlSerializer.  The list can later be read back
     * with readThisInt64ArrayXml().
     *
     * @param val The Int64 array to be flattened.
     * @param name Name attribute to include with this array's tag, or null for
     *             none.
     * @param out XmlSerializer to write the array into.
     *
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readThisIntArrayXml
     */
    static CARAPI WriteInt64ArrayXml(
        /* [in] */ ArrayOf<Int64>* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten a double[] into an XmlSerializer.  The list can later be read back
     * with readThisDoubleArrayXml().
     *
     * @param val The double array to be flattened.
     * @param name Name attribute to include with this array's tag, or null for
     *             none.
     * @param out XmlSerializer to write the array into.
     *
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readThisIntArrayXml
     */
    static CARAPI WriteDoubleArrayXml(
        /* [in] */ ArrayOf<Double>* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten a String[] into an XmlSerializer.  The list can later be read back
     * with readThisStringArrayXml().
     *
     * @param val The Int64 array to be flattened.
     * @param name Name attribute to include with this array's tag, or null for
     *             none.
     * @param out XmlSerializer to write the array into.
     *
     * @see #writeMapXml
     * @see #writeValueXml
     * @see #readThisIntArrayXml
     */
    static CARAPI WriteStringArrayXml(
        /* [in] */ ArrayOf<String>* val,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Flatten an object's value into an XmlSerializer.  The value can later
     * be read back with readThisValueXml().
     *
     * Currently supported value types are: NULL, String, Integer, Int64,
     * Float, Double Boolean, Map, List.
     *
     * @param v The object to be flattened.
     * @param name Name attribute to include with this value's tag, or NULL
     *             for none.
     * @param out XmlSerializer to write the object into.
     *
     * @see #writeMapXml
     * @see #writeListXml
     * @see #readValueXml
     */
    static CARAPI WriteValueXml(
        /* [in] */ IInterface* v,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out);

    /**
     * Read a HashMap from an InputStream containing XML.  The stream can
     * previously have been written by writeMapXml().
     *
     * @param in The InputStream from which to read.
     *
     * @return HashMap The resulting map.
     *
     * @see #readListXml
     * @see #readValueXml
     * @see #readThisMapXml
     * #see #writeMapXml
     */
    static CARAPI ReadMapXml(
        /* [in] */ IInputStream* in,
        /* [out] */ IHashMap** map);

    /**
     * Read an ArrayList from an InputStream containing XML.  The stream can
     * previously have been written by writeListXml().
     *
     * @param in The InputStream from which to read.
     *
     * @return ArrayList The resulting list.
     *
     * @see #readMapXml
     * @see #readValueXml
     * @see #readThisListXml
     * @see #writeListXml
     */
    static CARAPI ReadListXml(
        /* [in] */ IInputStream* in,
        /* [out] */ IArrayList** list);

    /**
     * Read a HashSet from an InputStream containing XML. The stream can
     * previously have been written by writeSetXml().
     *
     * @param in The InputStream from which to read.
     *
     * @return HashSet The resulting set.
     *
     * @throws XmlPullParserException
     * @throws java.io.IOException
     *
     * @see #readValueXml
     * @see #readThisSetXml
     * @see #writeSetXml
     */
    static CARAPI ReadSetXml(
        /* [in] */ IInputStream* in,
        /* [out] */ IHashSet** set);

    /**
     * Read a HashMap object from an XmlPullParser.  The XML data could
     * previously have been generated by writeMapXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the map.
     *
     * @param parser The XmlPullParser from which to read the map data.
     * @param endTag Name of the tag that will end the map, usually "map".
     * @param name An array of one string, used to return the name attribute
     *             of the map's tag.
     *
     * @return HashMap The newly generated map.
     *
     * @see #readMapXml
     */
    static CARAPI ReadThisMapXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ IHashMap** map);

    /**
     * Read a HashMap object from an XmlPullParser.  The XML data could
     * previously have been generated by writeMapXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the map.
     *
     * @param parser The XmlPullParser from which to read the map data.
     * @param endTag Name of the tag that will end the map, usually "map".
     * @param name An array of one string, used to return the name attribute
     *             of the map's tag.
     *
     * @return HashMap The newly generated map.
     *
     * @see #readMapXml
     * @hide
     */
    static CARAPI ReadThisMapXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [in] */ IXmlUtilsReadMapCallback* callback,
        /* [out] */ IHashMap** map);

    /**
     * Read an ArrayList object from an XmlPullParser.  The XML data could
     * previously have been generated by writeListXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "list".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return HashMap The newly generated list.
     *
     * @see #readListXml
     */
    static CARAPI ReadThisListXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ IArrayList** list);

    /**
     * Read a HashSet object from an XmlPullParser. The XML data could previously
     * have been generated by writeSetXml(). The XmlPullParser must be positioned
     * <em>after</em> the tag that begins the set.
     *
     * @param parser The XmlPullParser from which to read the set data.
     * @param endTag Name of the tag that will end the set, usually "set".
     * @param name An array of one string, used to return the name attribute
     *             of the set's tag.
     *
     * @return HashSet The newly generated set.
     *
     * @throws XmlPullParserException
     * @throws java.io.IOException
     *
     * @see #readSetXml
     */
    static CARAPI ReadThisSetXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ IHashSet** set);

    /**
     * Read an Int32[] object from an XmlPullParser.  The XML data could
     * previously have been generated by writeIntArrayXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "list".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return Returns a newly generated Int32[].
     *
     * @see #readListXml
     */
    static CARAPI ReadThisInt32ArrayXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ ArrayOf<Int32>** array);

    /**
     * Read a Int64[] object from an XmlPullParser.  The XML data could
     * previously have been generated by writeInt64ArrayXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "list".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return Returns a newly generated Int64[].
     *
     * @see #readListXml
     */
    static CARAPI ReadThisInt64ArrayXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ ArrayOf<Int64>** array);

    /**
     * Read a double[] object from an XmlPullParser.  The XML data could
     * previously have been generated by writeDoubleArrayXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "double-array".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return Returns a newly generated double[].
     *
     * @see #readListXml
     */
    static CARAPI ReadThisDoubleArrayXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ ArrayOf<Double>** array);

    /**
     * Read a String[] object from an XmlPullParser.  The XML data could
     * previously have been generated by writeStringArrayXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "string-array".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return Returns a newly generated String[].
     *
     * @see #readListXml
     */
    static CARAPI ReadThisStringArrayXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ ArrayOf<String>** array);

    /**
     * Read a flattened object from an XmlPullParser.  The XML data could
     * previously have been written with writeMapXml(), writeListXml(), or
     * writeValueXml().  The XmlPullParser must be positioned <em>at</em> the
     * tag that defines the value.
     *
     * @param parser The XmlPullParser from which to read the object.
     * @param name An array of one string, used to return the name attribute
     *             of the value's tag.
     *
     * @return Object The newly generated value object.
     *
     * @see #readMapXml
     * @see #readListXml
     * @see #writeValueXml
     */
    static CARAPI ReadValueXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ ArrayOf<String>* name,
        /* [out] */ IInterface** value);

    static CARAPI BeginDocument(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& firstElementName);

    static CARAPI NextElement(
        /* [in] */ IXmlPullParser* parser);

    static CARAPI_(Boolean) NextElementWithin(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 outerDepth);

    static CARAPI ReadInt32Attribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* result);

    static CARAPI ReadInt32Attribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ Int32* result);

    static CARAPI WriteInt32Attribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    static CARAPI ReadInt64Attribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name, Int64 defaultValue,
        /* [out] */ Int64* result);

    static CARAPI ReadInt64Attribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ Int64* result);

    static CARAPI WriteInt64Attribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ Int64 value);

    static CARAPI ReadFloatAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ Float* result);

    static CARAPI WriteFloatAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ Float value);

    static CARAPI ReadBooleanAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    static CARAPI ReadBooleanAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    static CARAPI WriteBooleanAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    static CARAPI ReadUriAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ IUri** result);

    static CARAPI WriteUriAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ IUri* value);

    static CARAPI ReadStringAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ String* result);

    static CARAPI WriteStringAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    static CARAPI ReadByteArrayAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<Byte>** array);

    static CARAPI WriteByteArrayAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* value);

    static CARAPI ReadBitmapAttribute(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ const String& name,
        /* [out] */ IBitmap** result);

    static CARAPI WriteBitmapAttribute(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ const String& name,
        /* [in] */ IBitmap* value);

private:
    /**
     * Flatten an object's value into an XmlSerializer.  The value can later
     * be read back with readThisValueXml().
     *
     * Currently supported value types are: null, String, Integer, Int64,
     * Float, Double Boolean, Map, List.
     *
     * @param v The object to be flattened.
     * @param name Name attribute to include with this value's tag, or null
     *             for none.
     * @param out XmlSerializer to write the object into.
     * @param callback Handler for Object types not recognized.
     *
     * @see #writeMapXml
     * @see #writeListXml
     * @see #readValueXml
     */
    ECO_LOCAL static CARAPI WriteValueXml(
        /* [in] */ IInterface* v,
        /* [in] */ const String& name,
        /* [in] */ IXmlSerializer* out,
        /* [in] */ IXmlUtilsWriteMapCallback* callback);

    /**
     * Read an ArrayList object from an XmlPullParser.  The XML data could
     * previously have been generated by writeListXml().  The XmlPullParser
     * must be positioned <em>after</em> the tag that begins the list.
     *
     * @param parser The XmlPullParser from which to read the list data.
     * @param endTag Name of the tag that will end the list, usually "list".
     * @param name An array of one string, used to return the name attribute
     *             of the list's tag.
     *
     * @return HashMap The newly generated list.
     *
     * @see #readListXml
     */
    ECO_LOCAL static CARAPI ReadThisListXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [in] */ IXmlUtilsReadMapCallback* callback,
        /* [out] */ IArrayList** list);

    /**
     * Read a HashSet object from an XmlPullParser. The XML data could previously
     * have been generated by writeSetXml(). The XmlPullParser must be positioned
     * <em>after</em> the tag that begins the set.
     *
     * @param parser The XmlPullParser from which to read the set data.
     * @param endTag Name of the tag that will end the set, usually "set".
     * @param name An array of one string, used to return the name attribute
     *             of the set's tag.
     *
     * @return HashSet The newly generated set.
     *
     * @throws XmlPullParserException
     * @throws java.io.IOException
     *
     * @see #readSetXml
     * @hide
     */
    ECO_LOCAL static CARAPI ReadThisSetXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& endTag,
        /* [in] */ ArrayOf<String>* name,
        /* [in] */ IXmlUtilsReadMapCallback* callback,
        /* [out] */ IHashSet** set);

    ECO_LOCAL static CARAPI ReadThisValueXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ ArrayOf<String>* name,
        /* [in] */ IXmlUtilsReadMapCallback* callback,
        /* [out] */ IInterface** ret);

    ECO_LOCAL static CARAPI ReadThisPrimitiveValueXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& tagName,
        /* [out] */ IInterface** ret);

private:
    static const String TAG;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_UTILITY_XMLUTILS_H__
