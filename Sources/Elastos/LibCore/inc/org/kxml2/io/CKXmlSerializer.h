
#ifndef __ORG_KXML2_IO_CKXMLSERIALIZER_H__
#define __ORG_KXML2_IO_CKXMLSERIALIZER_H__

#include "_Org_Kxml2_IO_CKXmlSerializer.h"
#include <elastos/core/Object.h>

using Elastos::Core::IBoolean;
using Elastos::IO::IWriter;
using Elastos::IO::IOutputStream;
using Elastos::IO::IBufferedWriter;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Org {
namespace Kxml2 {
namespace IO {

CarClass(CKXmlSerializer)
    , public Object
    , public IXmlSerializer
{
public:
    CAR_INTERFACE_DECL()
    CAR_OBJECT_DECL()

    CKXmlSerializer();

    ~CKXmlSerializer();

    CARAPI WriteDocDecl(
        /* [in] */ const String& dd);

    CARAPI EndDocument();

    CARAPI WriteEntityRef(
        /* [in] */ const String& text);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* value);

    CARAPI GetPrefix(
        /* [in] */ const String& ns,
        /* [in] */ Boolean generatePrefix,
        /* [out] */ String* prefix);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** value);

    CARAPI WriteIgnorableWhitespace(
        /* [in] */ const String& text);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean state);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    CARAPI SetPrefix(
        /* [in] */ const String& prefix,
        /* [in] */ const String& ns);

    CARAPI SetOutput(
        /* [in] */ IWriter* writer);

    CARAPI SetOutput(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& encoding);

    CARAPI StartDocument(
        /* [in] */ const String& encoding,
        /* [in] */ Boolean standalone);

    CARAPI WriteStartTag(
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    CARAPI WriteAttribute(
        /* [in] */ const String& ns,
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI Flush();

    CARAPI WriteEndTag(
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetDepth(
        /* [out] */ Int32* depth);

    CARAPI WriteText(
        /* [in] */ const String& text);

    CARAPI WriteText(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 start,
        /* [in] */ Int32 len);

    CARAPI WriteCdSect(
        /* [in] */ const String& text);

    CARAPI WriteComment(
        /* [in] */ const String& text);

    CARAPI WriteProcessingInstruction(
        /* [in] */ const String& text);

private:
    CARAPI Check(
        /* [in] */ Boolean close);

    CARAPI WriteEscaped(
        /* [in] */ const String& s,
        /* [in] */ Int32 quot);

    // BEGIN android-added
    static CARAPI ReportInvalidCharacter(
        /* [in] */ Char32 ch);
    // END android-added

    CARAPI GetPrefix(
        /* [in] */ const String& ns,
        /* [in] */ Boolean includeDefault,
        /* [in] */ Boolean create,
        /* [out] */ String* prefix);

private:
    //    static final String UNDEFINED = ":";

    // BEGIN android-added
    /** size (in characters) for the write buffer */
    static const Int32 WRITE_BUFFER_SIZE = 500;
    // END android-added

    // BEGIN android-changed
    // (Guarantee that the writer is always buffered.)
    AutoPtr<IBufferedWriter> mWriter;
    // END android-changed

    AutoPtr<ArrayOf<String> > mElementStack;
    //nsp/prefix/name
    AutoPtr<ArrayOf<Int32> > mNspCounts;
    AutoPtr<ArrayOf<String> > mNspStack;
    //prefix/nsp; both empty are ""
    AutoPtr<ArrayOf<Boolean> > mIndent;

    String mEncoding;

    Int32 mAuto;
    Int32 mDepth;
    Boolean mPending;
    Boolean mUnicode;
};

} //namespace IO
} //namespace Kxml2
} //namespace Org

#endif //__ORG_KXML2_IO_CKXMLSERIALIZER_H__
