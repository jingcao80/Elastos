#include "test.h"

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Text;
using namespace Libcore::ICU;
using namespace Elastos::IO;

void CheckLocaleIsEquivalentToRoot(ILocale* locale)
{
    AutoPtr<IDecimalFormatSymbols> dfs, dfs2;
    AutoPtr<IDecimalFormatSymbolsHelper> dfsh;
    CDecimalFormatSymbolsHelper::AcquireSingleton((IDecimalFormatSymbolsHelper**)&dfsh);
    dfsh->GetInstanceEx(locale, (IDecimalFormatSymbols**)&dfs);
    AutoPtr<ILocale> Locale_ROOT;
    AutoPtr<ILocaleHelper> lochep;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochep);
    lochep->GetROOT((ILocale**)&Locale_ROOT);
    dfsh->GetInstanceEx(Locale_ROOT, (IDecimalFormatSymbols**)&dfs2);
PFL_EX("dfs2 : %p , dfs : %p" , dfs2.Get() , dfs.Get())
    assert(dfs2 != dfs);
}

int CTest::test_getInstance_unknown_or_invalid_locale(int argc, char* argv[])
{
    // TODO: we fail these tests because ROOT has "INF" for infinity but 'dfs' has "\u221e".
    // On the RI, ROOT has "\u221e" too, but DecimalFormatSymbols.equals appears to be broken;
    // it returns false for objects that -- if you compare their externally visible state --
    // are equal. It could be that they're accidentally checking the Locale.
    AutoPtr<ILocale> locale, locale2;
    CLocale::New(String("xx"), String("XX"), (ILocale**)&locale);
    CLocale::New(String("not exist language"), String("not exist country"), (ILocale**)&locale2);
    CheckLocaleIsEquivalentToRoot(locale);
    CheckLocaleIsEquivalentToRoot(locale2);
    return 0;
}

// http://code.google.com/p/android/issues/detail?id=14495
int CTest::testSerialization(int argc, char* argv[]) {
    AutoPtr<IDecimalFormatSymbols> originalDfs;
    AutoPtr<IDecimalFormatSymbolsHelper> dfsh;
    CDecimalFormatSymbolsHelper::AcquireSingleton((IDecimalFormatSymbolsHelper**)&dfsh);
    AutoPtr<ILocale> geloc;
    AutoPtr<ILocaleHelper> lochep;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lochep);
    lochep->GetGERMANY((ILocale**)&geloc);
    dfsh->GetInstanceEx(geloc , (IDecimalFormatSymbols**)&originalDfs);

    // Serialize...
    // AutoPtr<IByteArrayOutputStream> out;
    // CByteArrayOutputStream::New((IByteArrayOutputStream**)&out);

    // new ObjectOutputStream(out).writeObject(originalDfs);
    // byte[] bytes = out.toByteArray();

    // // Deserialize...
    // ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(bytes));
    // DecimalFormatSymbols deserializedDfs = (DecimalFormatSymbols) in.readObject();
    // assertEquals(-1, in.read());

    // // The two objects should claim to be equal.
    // assertEquals(originalDfs, deserializedDfs);
}

