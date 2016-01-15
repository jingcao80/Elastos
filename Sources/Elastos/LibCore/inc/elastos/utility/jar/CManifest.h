
#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CMANIFEST_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CMANIFEST_H__

#include "_Elastos_Utility_Jar_CManifest.h"
#include "Object.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::ICloneable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteBuffer;
using Elastos::IO::Charset::ICharsetEncoder;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CManifest)
    , public Object
    , public IManifest
    , public ICloneable
{
public:
    class Chunk
        : public Object
    {
    public:
        Chunk(
            /* in */ Int32 start,
            /* in */ Int32 end)
            : mStart(start)
            , mEnd(end)
        {
        }

    public:
        Int32 mStart;
        Int32 mEnd;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static CARAPI Write(
        /* [in] */ IManifest* manifest,
        /* [in] */ IOutputStream* out);

    CManifest();

    ~CManifest();

    /**
     * Creates a new {@code Manifest} instance.
     */
    CARAPI constructor();

    /**
     * Creates a new {@code Manifest} instance using the attributes obtained
     * from the input stream.
     *
     * @param is
     *            {@code InputStream} to parse for attributes.
     * @throws IOException
     *             if an IO error occurs while creating this {@code Manifest}
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * Creates a new {@code Manifest} instance. The new instance will have the
     * same attributes as those found in the parameter {@code Manifest}.
     *
     * @param man
     *            {@code Manifest} instance to obtain attributes from.
     */
    CARAPI constructor(
        /* [in] */ IManifest* mani);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * manifestBytes,
        /* [in] */ Boolean readChunks);

    /**
     * Resets the both the main attributes as well as the entry attributes
     * associated with this {@code Manifest}.
     */
    CARAPI Clear();

    /**
     * Returns the {@code Attributes} associated with the parameter entry
     * {@code name}.
     *
     * @param name
     *            the name of the entry to obtain {@code Attributes} from.
     * @return the Attributes for the entry or {@code null} if the entry does
     *         not exist.
     */
    CARAPI GetAttributes(
        /* [in] */ const String& name,
        /* [out] */ IAttributes** attrib);

    /**
     * Returns a map containing the {@code Attributes} for each entry in the
     * {@code Manifest}.
     *
     * @return the map of entry attributes.
     */
    CARAPI GetEntries(
        /* [out] */ IMap** entries);

    /**
     * Returns the main {@code Attributes} of the {@code JarFile}.
     *
     * @return main {@code Attributes} associated with the source {@code
     *         JarFile}.
     */
    CARAPI GetMainAttributes(
        /* [out] */ IAttributes** attrib);

    /**
     * Creates a copy of this {@code Manifest}. The returned {@code Manifest}
     * will equal the {@code Manifest} from which it was cloned.
     *
     * @return a copy of this instance.
     */
    CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * Writes this {@code Manifest}'s name/attributes pairs to the given {@code OutputStream}.
     * The {@code MANIFEST_VERSION} or {@code SIGNATURE_VERSION} attribute must be set before
     * calling this method, or no attributes will be written.
     *
     * @throws IOException
     *             If an error occurs writing the {@code Manifest}.
     */
    CARAPI Write(
        /* [in] */ IOutputStream* os);

    /**
     * Merges name/attribute pairs read from the input stream {@code is} into this manifest.
     *
     * @param is
     *            The {@code InputStream} to read from.
     * @throws IOException
     *             If an error occurs reading the manifest.
     */
    CARAPI Read(
        /* [in] */ IInputStream* is);

    /**
     * Returns the hash code for this instance.
     *
     * @return this {@code Manifest}'s hashCode.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashcode);

    /**
     * Determines if the receiver is equal to the parameter object. Two {@code
     * Manifest}s are equal if they have identical main attributes as well as
     * identical entry attributes.
     *
     * @param o
     *            the object to compare against.
     * @return {@code true} if the manifests are equal, {@code false} otherwise
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI_(AutoPtr<CManifest::Chunk>) GetChunk(
        /* [in] */ const String& name);

    CARAPI RemoveChunks();

    CARAPI GetMainAttributesEnd(
        /* [out] */ Int32* mainAttrib);

private:
    CARAPI Read(
        /* [in] */ ArrayOf<Byte> * buf);

    static CARAPI WriteEntry(
        /* [in] */ IOutputStream* os,
        /* [in] */ IName* name,
        /* [in] */ const String& value,
        /* [in] */ ICharsetEncoder* encoder,
        /* [in] */ IByteBuffer* bBuf);

public:
    static const Int32 LINE_LENGTH_LIMIT = 72;
    //AutoPtr<HashMap<String, AutoPtr<IAttributes> > > mEntries;
    AutoPtr<IMap> mEntries;// HashMap<String, Attributes> entries;

private:
    static const AutoPtr<ArrayOf<Byte> > LINE_SEPARATOR;

    static const AutoPtr<ArrayOf<Byte> > VALUE_SEPARATOR;

    static const AutoPtr<IName> NAME_ATTRIBUTE;// = new Attributes.Name("Name");

    AutoPtr<IAttributes> mMainAttributes;

    AutoPtr<HashMap<String, AutoPtr<Chunk> > > mChunks;

    /**
     * The end of the main attributes section in the manifest is needed in
     * verification.
     */
    Int32 mMainEnd;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CMANIFEST_H__
