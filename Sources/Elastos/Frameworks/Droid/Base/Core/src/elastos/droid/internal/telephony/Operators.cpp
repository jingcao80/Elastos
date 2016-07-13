
#include "elastos/droid/internal/telephony/Operators.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

String Operators::sStored;
String Operators::sStoredOperators;

// Initialize list of Operator codes
// this will be taken care of when garbage collection starts.
AutoPtr<HashMap<String, String> > Operators::InitList()
{
    AutoPtr<HashMap<String, String> > init = new HashMap<String, String>();
    //taken from spnOveride.java

    AutoPtr<IFileReader> spnReader;

    AutoPtr<IFile> dir = Environment::GetRootDirectory();
    AutoPtr<IFile> spnFile;
    CFile::New(dir, String("etc/selective-spn-conf.xml"), (IFile**)&spnFile);

    // try {
    if (FAILED(CFileReader::New(spnFile, (IFileReader**)&spnReader))) {
        Logger::W("Operatorcheck", "Can not open %s/etc/selective-spn-conf.xml", TO_CSTR(dir));
        return init;
    }
    // } catch (FileNotFoundException e) {
    //     Logger::W("Operatorcheck", "Can not open " +
    //            Environment.getRootDirectory() + "/etc/selective-spn-conf.xml");
    //     return init;
    // }

    ECode ec = NOERROR;
    do {
        AutoPtr<IXmlPullParser> parser;
        ec= Xml::NewPullParser((IXmlPullParser**)&parser);
        if (FAILED(ec)) {
            break;
        }
        ec = parser->SetInput(IReader::Probe(spnReader));
        if (FAILED(ec)) {
            break;
        }

        ec = XmlUtils::BeginDocument(parser, String("spnOverrides"));
        if (FAILED(ec)) {
            break;
        }

        while (TRUE && SUCCEEDED(ec)) {
            ec = XmlUtils::NextElement(parser);
            if (FAILED(ec)) {
                break;
            }

            String name;
            ec = parser->GetName(&name);
            if (FAILED(ec)) {
                break;
            }

            if (!name.Equals("spnOverride")) {
                break;
            }

            String numeric;
            ec = parser->GetAttributeValue(String(NULL), String("numeric"), &numeric);
            if (FAILED(ec)) {
                break;
            }

            String data;
            ec = parser->GetAttributeValue(String(NULL), String("spn"), &data);
            if (FAILED(ec)) {
                break;
            }

            (*init)[numeric] = data;
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::W("Operatorcheck", "Exception in spn-conf parser ");
    }
    return init;
}

String Operators::OperatorReplace(
    /* [in] */ const String& response)
{
    // sanity checking if the value is actually not equal to the range apn
    // numerics
    // if it is NULL, check your ril class.
    if(response == NULL || (5 != response.GetLength() && response.GetLength() != 6)){
        return response;
    }
    // this will check if the stored value is equal to other.
    // this uses a technique called last known of good value.
    // along with sanity checking
    if(sStoredOperators != NULL && sStored != NULL && sStored.Equals(response)){
        return sStoredOperators;
    }
    sStored = response;
    // try {
    // this will find out if it a number then it will catch it based
    // on invalid chars.
    Int32 v = 0;
    if (FAILED(StringUtils::Parse(response, &v))) {
        // not a number, pass it along to stored operator until the next
        // round.
        sStoredOperators = response;
        return sStoredOperators;
    }
    // }  catch(NumberFormatException E){
    //     // not a number, pass it along to stored operator until the next
    //     // round.
    //     sStoredOperators = response;
    //     return sStoredOperators;
    // }
    // this code will be taking care of when garbage collection start
    AutoPtr<Operators> init = new Operators();
    AutoPtr<HashMap<String, String> > operators = init->InitList();
    HashMap<String, String>::Iterator ator = operators->Find(response);
    if (ator != operators->End()) {
        sStoredOperators = ator->mSecond;
    }
    else {
        sStoredOperators = response;
    }
    return sStoredOperators;
}

// unoptimized version of operatorreplace for responseOperatorInfos
// this will provide a little more flexiblilty  in a loop like sisuation
// same numbers of checks like before
// this is for the search network functionality
String Operators::UnOptimizedOperatorReplace(
    /* [in] */ const String& response)
{
    // sanity checking if the value is actually not equal to the range apn
    // numerics
    // if it is NULL, check your ril class.
    if(response == NULL || (5 != response.GetLength() && response.GetLength() != 6)){
        return response;
    }

    // try {
    // this will find out if it a number then it will catch it based
    // on invalid chars.
    Int32 v = 0;
    if (FAILED(StringUtils::Parse(response, &v))) {
        // an illegal char is found i.e a word
        return response;
    }
    // }  catch(NumberFormatException E){
    //     // an illegal char is found i.e a word
    //     return response;
    // }

    if (mUnOptOperators == NULL){
        mUnOptOperators = InitList();
    }

    HashMap<String, String>::Iterator ator = mUnOptOperators->Find(response);
    if (ator != mUnOptOperators->End()) {
        return ator->mSecond;
    }

    return response;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
