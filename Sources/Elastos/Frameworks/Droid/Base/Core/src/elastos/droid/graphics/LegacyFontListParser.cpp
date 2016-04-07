
#include <elastos/droid/ext/frameworkdef.h>
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/graphics/LegacyFontListParser.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Droid::Utility::Xml;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {

//=============================================================
// LegacyFontListParser::Family
//=============================================================
LegacyFontListParser::Family::Family()
{
    CArrayList::New((IList**)&mNameset);
    CArrayList::New((IList**)&mFileset);
}

String LegacyFontListParser::Family::GetName()
{
    Boolean isEmpty;
    if (mNameset != NULL && (mNameset->IsEmpty(&isEmpty), !isEmpty)) {
        AutoPtr<IInterface> obj;
        mNameset->Get(0, (IInterface**)&obj);
        ICharSequence* csq = ICharSequence::Probe(obj);
        String name;
        csq->ToString(&name);
        return name;
    }
    return String(NULL);
}


//=============================================================
// LegacyFontListParser
//=============================================================
ECode LegacyFontListParser::Parse(
    /* [in] */ IInputStream* in,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    ECode ec = NOERROR;
    Int32 tag;
    AutoPtr<IXmlPullParser> parser;
    ec = Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_GOTO(ec, EXIT)
    ec = parser->SetInput(in, String(NULL));
    FAIL_GOTO(ec, EXIT)
    ec = parser->NextTag(&tag);
    FAIL_GOTO(ec, EXIT)
    ec = ReadFamilySet(parser, result);
    // } finally {
    //     in.close();
    // }
EXIT:
    in->Close();
    return ec;
}

ECode LegacyFontListParser::ReadFamilySet(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IList** result)
{
    *result = NULL;

    AutoPtr<IList> families;
    CArrayList::New((IList**)&families);
    parser->Require(IXmlPullParser::START_TAG, String(NULL), String("familyset"));

    Int32 next;
    FAIL_RETURN(parser->Next(&next));
    while (next != IXmlPullParser::END_TAG) {
        Int32 eventType;
        if (parser->GetEventType(&eventType), eventType != IXmlPullParser::START_TAG) {
            FAIL_RETURN(parser->Next(&next));
            continue;
        }
        String name;
        parser->GetName(&name);

        // Starts by looking for the entry tag
        if (name.Equals("family")) {
            AutoPtr<Family> family;
            FAIL_RETURN(ReadFamily(parser, (Family**)&family));
            families->Add((IObject*)family);
        }
        FAIL_RETURN(parser->Next(&next));
    }
    *result = families;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LegacyFontListParser::ReadFamily(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Family** result)
{
    *result = NULL;

    AutoPtr<Family> family = new Family();
    parser->Require(IXmlPullParser::START_TAG, String(NULL), String("family"));

    Int32 next;
    FAIL_RETURN(parser->Next(&next));
    while (next != IXmlPullParser::END_TAG) {
        Int32 eventType;
        if (parser->GetEventType(&eventType), eventType != IXmlPullParser::START_TAG) {
            FAIL_RETURN(parser->Next(&next));
            continue;
        }
        String name;
        parser->GetName(&name);
        if (name.Equals("nameset")) {
            AutoPtr<IList> nameset;
            FAIL_RETURN(ReadNameset(parser, (IList**)&nameset));
            family->mNameset = nameset;
        }
        else if (name.Equals("fileset")) {
            AutoPtr<IList> fileset;
            FAIL_RETURN(ReadFileset(parser, (IList**)&fileset));
            family->mFileset = fileset;
        }
        else {
            Skip(parser);
        }
        FAIL_RETURN(parser->Next(&next));
    }
    *result = family;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LegacyFontListParser::ReadNameset(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IList** result)
{
    *result = NULL;

    AutoPtr<IList> names;
    CArrayList::New((IList**)&names);
    parser->Require(IXmlPullParser::START_TAG, String(NULL), String("nameset"));

    Int32 next;
    FAIL_RETURN(parser->Next(&next));
    while (next != IXmlPullParser::END_TAG) {
        Int32 eventType;
        if (parser->GetEventType(&eventType), eventType != IXmlPullParser::START_TAG) {
            FAIL_RETURN(parser->Next(&next));
            continue;
        }
        String tagname;
        parser->GetName(&tagname);
        if (tagname.Equals("name")) {
            String name;
            FAIL_RETURN(ReadText(parser, &name));
            names->Add(CoreUtils::Convert(name));
        }
        else {
            Skip(parser);
        }
        FAIL_RETURN(parser->Next(&next));
    }
    *result = names;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LegacyFontListParser::ReadFileset(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IList** result)
{
    *result = NULL;

    AutoPtr<IList> files;
    CArrayList::New((IList**)&files);
    parser->Require(IXmlPullParser::START_TAG, String(NULL), String("fileset"));

    Int32 next;
    FAIL_RETURN(parser->Next(&next));
    while (next != IXmlPullParser::END_TAG) {
        Int32 eventType;
        if (parser->GetEventType(&eventType), eventType != IXmlPullParser::START_TAG) {
            FAIL_RETURN(parser->Next(&next));
            continue;
        }
        String name;
        parser->GetName(&name);
        if (name.Equals("file")) {
            String file;
            FAIL_RETURN(ReadText(parser, &file));
            files->Add(CoreUtils::Convert(file));
        }
        else {
            Skip(parser);
        }
        FAIL_RETURN(parser->Next(&next));
    }
    *result = files;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LegacyFontListParser::ReadText(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ String* text)
{
    *text = NULL;

    String result("");
    Int32 next;
    FAIL_RETURN(parser->Next(&next));
    if (next == IXmlPullParser::TEXT) {
        parser->GetText(&result);
        FAIL_RETURN(parser->NextTag(&next));
    }
    *text = result;
    return NOERROR;
}

ECode LegacyFontListParser::Skip(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 eventType;
    if (parser->GetEventType(&eventType), eventType != IXmlPullParser::START_TAG) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 depth = 1;
    while (depth != 0) {
        Int32 next;
        FAIL_RETURN(parser->Next(&next));
        switch (next) {
        case IXmlPullParser::END_TAG:
            depth--;
            break;
        case IXmlPullParser::START_TAG:
            depth++;
            break;
        }
    }
    return NOERROR;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
