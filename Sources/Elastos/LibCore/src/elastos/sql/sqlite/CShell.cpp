
#include "CShell.h"
#include "StringBuffer.h"
#include "CPrintWriter.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CShell)

CAR_INTERFACE_IMPL_2(CShell, Object, IShell, ICallback)

CShell::CShell()
    : mEcho(FALSE)
    , mCount(0)
    , mMode(0)
    , mShowHeader(FALSE)
{}

CARAPI CShell::constructor(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IPrintWriter* err)
{
    mPw = pw;
    mErr = err;
    return NOERROR;
}

CARAPI CShell::constructor(
    /* [in] */ IPrintStream* ps,
    /* [in] */ IPrintStream* errs)
{
    FAIL_RETURN(CPrintWriter::New(IOutputStream::Probe(ps), (IPrintWriter**)&mPw));
    FAIL_RETURN(CPrintWriter::New(IOutputStream::Probe(errs), (IPrintWriter**)&mErr));
    return NOERROR;
}

void CShell::Clone(
    /* [out] */ CShell& s)
{
    s.mDb = mDb;
    s.mEcho = mEcho;
    s.mMode = mMode;
    s.mCount = 0;
    s.mShowHeader = mShowHeader;
    s.mTableName = mTableName;
    s.mSep = mSep;
    s.mColwidth = mColwidth;
}

ECode CShell::Columns(
    /* [in] */ ArrayOf<String> * args)
{
    mCols = NULL;
    if (args) {
        mCols = args->Clone();
    }
    return NOERROR;
}

ECode CShell::Types(
    /* [in] */ ArrayOf<String> * types)
{
    /* Empty body to satisfy SQLite.Callback Int32erface. */
    return NOERROR;
}

ECode CShell::Newrow(
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ Boolean * pValue)
{
    Int32 i = 0;
    String tname(NULL);
    switch (mMode) {
    case MODE_Line:
        {
            if (args == NULL || args->GetLength() == 0) {
                break;
            }
            if (mCount++ > 0) {
                mPw->Print(String(""));
            }
            for (i = 0; i < args->GetLength(); i++) {
                mPw->Print((*mCols)[i] + String(" = ") + ((*args)[i] == NULL ? String("NULL") : (*args)[i]));
            }
            break;
        }

    case MODE_Column:
        {
            String csep(NULL);
            if (mCount++ == 0) {
                mColwidth = ArrayOf<Int32>::Alloc(args->GetLength());
                for (i = 0; i < args->GetLength(); i++) {
                    Int32 w = 0 ;
                    w = ((*mCols)[i]).GetLength();
                    if (w < 10) {
                        w = 10;
                    }
                    (*mColwidth)[i] = w;
                    if (mShowHeader) {
                        mPw->Print(csep + (*mCols)[i]);
                        csep = String(" ");
                    }
                }
                if (mShowHeader) {
                    mPw->Print(String(""));
                }
            }
            if (args->GetLength() == 0) {
                break;
            }
            csep = String("");
            for (i = 0; i < args->GetLength(); i++) {
                mPw->Print(csep + ((*args)[i] == NULL ? "NULL" : (*args)[i]));
                csep = String(" ");
            }
            mPw->Print(String(""));
            break;
        }

    case MODE_Semi:
    case MODE_List:
        {
            if (mCount++ == 0 && mShowHeader) {
            for (i = 0; i < args->GetLength(); i++) {
                mPw->Print((*mCols)[i] +
                     (i == args->GetLength() - 1 ? "\n" : mSep));
            }
            }
            if (args->GetLength() == 0) {
            break;
            }
            for (i = 0; i < args->GetLength(); i++) {
            mPw->Print((*args)[i] == NULL ? String("NULL") : (*args)[i]);
            if (mMode == MODE_Semi) {
            mPw->Print(String(";"));
            } else if (i < args->GetLength() - 1) {
                mPw->Print(mSep);
            }
            }
            mPw->Print(String(""));
            break;
        }

    case MODE_Html:
        {
            if (mCount++ == 0 && mShowHeader) {
                mPw->Print(String("<TR>"));
                for (i = 0; i < args->GetLength(); i++) {
                    mPw->Print(String("<TH>") + HtmlQuote((*mCols)[i]) + String("</TH>"));
            }
            mPw->Print(String("</TR>"));
            }
            if (args->GetLength() == 0) {
            break;
            }
            mPw->Print(String("<TR>"));
            for (i = 0; i < args->GetLength(); i++) {
                mPw->Print(String("<TD>") + HtmlQuote((*args)[i]) + String("</TD>"));
            }
            mPw->Print(String("</TR>"));
            break;
        }
    case MODE_Insert:
        {
            if (args->GetLength() == 0) {
            break;
            }
            tname = mTableName;
            if (mDestTable != NULL) {
                tname = mDestTable;
            }
            mPw->Print(String("INSERT Int32O ") + tname + String(" VALUES("));
            for (i = 0; i < args->GetLength(); i++) {
                String tsep = i > 0 ? String(",") : String("");
            if ((*args)[i] == NULL) {
                mPw->Print(tsep + String("NULL"));
            }
            else if (IsNumeric((*args)[i])) {
                 mPw->Print(tsep + (*args)[i]);
            }
            else {
                mPw->Print(tsep + SqlQuote((*args)[i]));
            }
            }
            mPw->Print(String(");"));
            break;
        }

    case MODE_Insert2:
        {
            if (args->GetLength() == 0) {
            break;
            }
            tname = mTableName;
            if (mDestTable != NULL) {
                tname = mDestTable;
            }
            mPw->Print(String("INSERT Int32O ") + tname + String(" VALUES("));
            for (i = 0; i < args->GetLength(); i++) {
                String tsep = i > 0 ? String(",") : String("");
            mPw->Print(tsep + (*args)[i]);
            }
            mPw->Print(String(");"));
            break;
        }
    }
    *pValue = false;
    return NOERROR;
}

String CShell::HtmlQuote(
    /* [in] */ const String& str)
{
    if (str == NULL) {
        return String("NULL");
    }

    StringBuffer sb;
    Char32 c;
    AutoPtr<ArrayOf<Char32> > charArray = str.GetChars();
    for (Int32 i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '<') {
            sb.Append("&lt;");
        }
        else if (c == '>') {
            sb.Append("&gt;");
        }
        else if (c == '&') {
            sb.Append("&amp;");
        }
        else {
            Int32 x = c;
            if (x < 32 || x > 127) {
                sb.Append("&#");
                sb.AppendChar(c);
                sb.Append(";");

            } else {
                sb.AppendChar(c);
            }
        }
    }
    return sb.ToString();
}

Boolean CShell::IsNumeric(
    /* [in] */ const String& str)
{
    Double d;
    ECode ec = StringUtils::Parse(str, &d);
    if (FAILED(ec)) {
        return FALSE;
    }
    return TRUE;
}

String CShell::SqlQuoteDbl(
    /* [in] */ const String& str)
{
    if (str == NULL) {
        return String("NULL");
    }

    Char32 c;
    AutoPtr<ArrayOf<Char32> > charArray = str.GetChars();
    Int32 i = 0, single = 0, dbl = 0;
    for (i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '\'') {
            single++;
        }
        else if (c == '"') {
            dbl++;
        }
    }

    if (dbl == 0) {
        return String(String("\"") + str + String("\""));
    }

    StringBuffer sb("\"");
    for (i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '"') {
            sb += ("\"\"");
        }
        else {
            sb.AppendChar(c);
        }
    }
    return sb.ToString();
}

String CShell::SqlQuote(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return String("NULL");
    }

    AutoPtr<ArrayOf<Char32> > charArray = str.GetChars();
    Int32 i, single = 0, dbl = 0;
    Char32 ch;
    for (i = 0; i < charArray->GetLength(); i++) {
        ch = (*charArray)[i];
        if (ch == '\'') {
            single++;
        } else if (ch == '"') {
            dbl++;
        }
    }
    if (single == 0) {
        return String("'") + str + String("'");
    }
    if (dbl == 0) {
        return String("\"") + str + String("\"");
    }

    StringBuffer sb("'");
    for (i = 0; i < charArray->GetLength(); i++) {
        ch = (*charArray)[i];
        if (ch == '\'') {
            sb += ("''");
        }
        else {
            sb.AppendChar(ch);
        }
    }
    return sb.ToString();
}

void CShell::SetTableName(
    /* [out] */ String str)
{
    if (str == NULL) {
        mTableName = String("");
        return;
    }
    Boolean isflag = FALSE;
    mDb->Is3(&isflag);
    if (isflag) {
        mTableName = SqlQuoteDbl(str);
    } else {
        mTableName = SqlQuote(str);
    }
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
