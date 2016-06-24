
#ifndef __ELASTOS_DROID_CONTENT_RES_XMLBLOCK_H__
#define __ELASTOS_DROID_CONTENT_RES_XMLBLOCK_H__

#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/content/res/StringBlock.h"

using Elastos::Droid::Utility::IAttributeSet;

using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::ICloseable;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * Wrapper around a compiled XML file.
 *
 * {@hide}
 */
class XmlBlock
    : public Object
{
public:
    class Parser
        : public Object
        , public IXmlResourceParser
        , public IXmlPullParser
        , public IAttributeSet
        , public ICloseable
    {
    public:
        TO_STRING_IMPL("XmlBlock::Parser")

        CAR_INTERFACE_DECL()

        Parser(
            /* [in] */ Int64 parseState,
            /* [in] */ XmlBlock* block);

        virtual ~Parser();

        CARAPI SetFeature(
            /* [in] */ const String& name,
            /* [in] */ Boolean state);

        CARAPI GetFeature(
            /* [in] */ const String& name,
            /* [out] */ Boolean* value);

        CARAPI SetProperty(
            /* [in] */ const String& name,
            /* [in] */ IInterface* value);

        CARAPI GetProperty(
            /* [in] */ const String& name,
            /* [out] */ IInterface** value);

        CARAPI SetInput(
            /* [in]*/ IReader* reader);

        CARAPI SetInput(
            /* [in] */ IInputStream* pInputStream,
            /* [in] */ const String& inputEncoding);

        CARAPI DefineEntityReplacementText(
            /* [in] */ const String& entityName,
            /* [in] */ const String& replacementText);

        CARAPI GetNamespacePrefix(
            /* [in] */ Int32 pos,
            /* [out] */ String* prefix);

        CARAPI GetInputEncoding(
            /* [out] */ String* inputEncoding);

        CARAPI GetNamespace(
            /* [in] */ const String& prefix,
            /* [out] */ String* ns);

        CARAPI GetNamespaceCount(
            /* [in] */ Int32 depth,
            /* [out] */ Int32* count);

        CARAPI GetPositionDescription(
            /* [out] */ String* des);

        CARAPI GetNamespaceUri(
            /* [in] */ Int32 pos,
            /* [out] */ String* uri);

        CARAPI GetColumnNumber(
            /* [out] */ Int32* number);

        CARAPI GetDepth(
            /* [out] */ Int32* depth);

        CARAPI GetText(
            /* [out] */ String* text);

        CARAPI GetLineNumber(
            /* [out] */ Int32* number);

        CARAPI GetEventType(
            /* [out] */ Int32* type);

        CARAPI IsWhitespace(
            /* [out] */ Boolean* result);

        CARAPI GetPrefix(
            /* [out] */ String* prefix);

        CARAPI GetTextCharacters(
            /* [out] */ ArrayOf<Int32>* holderForStartAndLength,
            /* [out, callee] */ ArrayOf<Char32>** textChars);

        CARAPI GetNamespace(
            /* [out] */ String* ns);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetAttributeNamespace(
            /* [in] */ Int32 index,
            /* [out] */ String* attrNamespace);

        CARAPI GetAttributeName(
            /* [in] */ Int32 index,
            /* [out] */ String* attrName);

        CARAPI GetAttributePrefix(
            /* [in] */ Int32 index,
            /* [out] */ String* attrPrefix);

        CARAPI IsEmptyElementTag(
            /* [out] */ Boolean* result);

        CARAPI GetAttributeCount(
            /* [out] */ Int32* attrCount);

        CARAPI GetAttributeValue(
            /* [in] */ Int32 index,
            /* [out] */ String* attrValue);

        CARAPI GetAttributeType(
            /* [in] */ Int32 index,
            /* [out] */ String* attrType);

        CARAPI IsAttributeDefault(
            /* [in] */ Int32 index,
            /* [out] */ Boolean* result);

        CARAPI NextToken(
            /* [out] */ Int32* token);

        CARAPI GetAttributeValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& name,
            /* [out] */ String* attrValue);

        CARAPI Next(
            /* [out] */ Int32* event);

        CARAPI Require(
            /* [in] */ Int32 type,
            /* [in] */ const String& ns,
            /* [in] */ const String& name);

        CARAPI NextText(
            /* [out] */ String* text);

        CARAPI NextTag(
            /* [out] */ Int32* tag);

        CARAPI GetAttributeNameResource(
            /* [in] */ Int32 index,
            /* [out] */ Int32* resId);

        CARAPI GetAttributeListValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ ArrayOf<String>* options,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeBooleanValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ Boolean defaultValue,
            /* [out] */ Boolean* value);

        CARAPI GetAttributeResourceValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeIntValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeUnsignedIntValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeFloatValue(
            /* [in] */ const String& ns,
            /* [in] */ const String& attribute,
            /* [in] */ Float defaultValue,
            /* [out] */ Float* value);

        CARAPI GetAttributeListValue(
            /* [in] */ Int32 idx,
            /* [in] */ ArrayOf<String>* options,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeBooleanValue(
            /* [in] */ Int32 idx,
            /* [in] */ Boolean defaultValue,
            /* [out] */ Boolean* value);

        CARAPI GetAttributeResourceValue(
            /* [in] */ Int32 idx,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeIntValue(
            /* [in] */ Int32 idx,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeUnsignedIntValue(
            /* [in] */ Int32 idx,
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetAttributeFloatValue(
            /* [in] */ Int32 idx,
            /* [in] */ Float defaultValue,
            /* [out] */ Float* value);

        CARAPI GetIdAttribute(
            /* [out] */ String* attr);

        CARAPI GetClassAttribute(
            /* [out] */ String* attr);

        CARAPI GetIdAttributeResourceValue(
            /* [in] */ Int32 defaultValue,
            /* [out] */ Int32* value);

        CARAPI GetStyleAttribute(
            /* [out] */ Int32* value);

        CARAPI Close();

        CARAPI GetPooledString(
            /* [in] */ Int32 id,
            /* [out] */ ICharSequence** csq);

    public:
        Int64 mParseState;

    private:
        AutoPtr<XmlBlock> mHost;
        Boolean mStarted;
        Boolean mDecNextDepth;
        Int32 mDepth;
        Int32 mEventType;
    };

public:
    TO_STRING_IMPL("XmlBlock")

    XmlBlock(
        /* [in] */ const ArrayOf<Byte>& data);

    XmlBlock(
        /* [in] */ const ArrayOf<Byte>& data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size);

    /**
     * Create from an existing xml block native object.  This is
     * -extremely- dangerous -- only use it if you absolutely know what you
     *  are doing!  The given native object must exist for the entire lifetime
     *  of this newly creating XmlBlock.
     */
    XmlBlock(
        /* [in] */ IAssetManager* assets,
        /* [in] */ Int64 xmlBlock);

    ~XmlBlock();

    CARAPI_(void) Close();

    CARAPI_(AutoPtr<IXmlResourceParser>) NewParser();

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    static CARAPI_(Int32) NativeNext(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetName(
        /* [in] */ Int64 parser);

private:
    CARAPI_(void) DecOpenCountLocked();

    static CARAPI_(Int64) NativeCreate(
        /* [in] */ const ArrayOf<Byte>& data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size);

    static CARAPI_(Int64) NativeGetStringBlock(
        /* [in] */ Int64 xmlTree);

    static CARAPI_(Int64) NativeCreateParseState(
        /* [in] */ Int64 xmlTree);

    static CARAPI_(Int32) NativeGetNamespace(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetText(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetLineNumber(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetAttributeCount(
        /* [in] */ Int64 parsere);

    static CARAPI_(Int32) NativeGetAttributeNamespace(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetAttributeName(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetAttributeResource(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetAttributeDataType(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetAttributeData(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetAttributeStringValue(
        /* [in] */ Int64 parser,
        /* [in] */ Int32 idx);

    static CARAPI_(Int32) NativeGetIdAttribute(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetClassAttribute(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetStyleAttribute(
        /* [in] */ Int64 parser);

    static CARAPI_(Int32) NativeGetAttributeIndex(
        /* [in] */ Int64 parser,
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    static CARAPI_(void) NativeDestroyParseState(
        /* [in] */ Int64 parser);

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Int64 obj);

protected:
    AutoPtr<StringBlock> mStrings;

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IAssetManager> mAssets;
    Int64 mNative;
    Int32 mOpenCount;
    Object mSyncLock;
    Boolean mOpen;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Content::Res::XmlBlock, IInterface)

#endif // __ELASTOS_DROID_CONTENT_RES_XMLBLOCK_H__
