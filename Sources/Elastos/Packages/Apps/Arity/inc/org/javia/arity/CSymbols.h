
#ifndef _ORG_JAVIA_ARITY_CSYMBOLS_H__
#define _ORG_JAVIA_ARITY_CSYMBOLS_H__

#include "_Org_Javia_Arity_CSymbols.h"
#include "org/javia/arity/Symbol.h"
#include "org/javia/arity/Compiler.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IHashTable;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IStack;

namespace Org {
namespace Javia {
namespace Arity {

CarClass(CSymbols)
    , public Object
    , public ISymbols
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CSymbols();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
       @param source the expression
       @return true if the expression is a definition (i.e. contains a '=').<p>
       These are definitions: "a=1+1";  "f(k)=2^k"<p>
       These are not definitions: "1+1"; "x+1"
     */
    static CARAPI_(Boolean) IsDefinition(
        /* [in] */ const String& source);

    /**
       Evaluates a simple expression (such as "1+1") and returns its value.
       @throws SyntaxException in these cases:
       <ul>
       <li> the expression is not well-formed
       <li> the expression is a definition (such as "a=1+1")
       <li> the expression is an implicit function (such as "x+1")
       </ul>
     */
    CARAPI Eval(
        /* [in] */ const String& expression,
        /* [out] */ Double* result);

    CARAPI EvalComplex(
        /* [in] */ const String& expression,
        /* [out] */ IComplex** complex);

    /**
       Compiles an expression in the context of this Symbols.
       Does not modify the symbols.
       <p>

       An expression is one of these cases (@see Symbols.isDefinition()):
       <ul>
       <li> constant value: 1+1
       <li> implicit function: x+1
       <li> constant definition: a=1+1
       <li> function definition with explicit arguments: f(a)=a+1
       <li> function definition with implicit arguments: f=x+1
       </ul>
       <p>

       @param source the expression; may contain '=' to denote a definition (with a name).

       @return the function together with its eventual name.<p>

       If this is not a definition (e.g. "1+1", "x^2"), the name is null.<p>

       If the expression is a constant (e.g. "1+1", "a=2"),
       the returned Function is an instance of {@link Constant}.

       @throws SyntaxException if there are errors compiling the expression.
    */
    CARAPI CompileWithName(
        /* [in] */ const String& source,
        /* [out] */ IFunctionAndName** fan);

    CARAPI Compile(
        /* [in] */ const String& source,
        /* [out] */ IFunction** func);

    /**
       Adds a new function symbol to the top-most frame of this Symbols.
       @param name the name of the function (e.g. "sin")
       @param function the function to which the name maps
    */
    CARAPI Define(
        /* [in] */ const String& name,
        /* [in] */ IFunction* function);

    /**
       Adds a new function symbol to the top-most frame of this Symbols.
       @param funAndName structure containing the function and its name
    */
    CARAPI Define(
        /* [in] */ IFunctionAndName* funAndName);


    /**
       Adds a new constant symbol to the top-most frame of this Symbols.
       @param name the name of the constant (e.g. "pi")
       @param value the value of the constant
    */
    CARAPI Define(
        /* [in] */ const String& name,
        /* [in] */ Double value);

    CARAPI Define(
        /* [in] */ const String& name,
        /* [in] */ IComplex* value);

    /**
       Pushes a new top frame.<p>

       All modifications (defining new symbols) happen in the top-most frame.
       When the frame is pop-ed the modifications that happened in it are reverted.
    */
    CARAPI PushFrame();

    /**
       Pops the top frame.<p>

       All the modifications done since this frame was pushed are reverted.
       @throws EmptyStackException if there were fewer <code>pushFrame</code> than <code>popFrame</code>.
    */
    CARAPI PopFrame();

    /**
     * Returns all the symbols that were added in the top frame.
     * (i.e. since the most recent pushFrame()).
     */
    CARAPI GetTopFrame(
        /* [out, callee] */ ArrayOf<ISymbol*>** topFrame);

    /**
     * Return all the defined symbols.
     */
    CARAPI GetAllSymbols(
        /* [out, callee] */ ArrayOf<ISymbol*>** symbols);

    /**
     * Return all the strings that are defined in this symbols.
     */
    CARAPI GetDictionary(
        /* [out, callee] */ ArrayOf<String>** dic);

    CARAPI_(void) AddArguments(
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(void) Add(
        /* [in] */ ISymbol* s);

    CARAPI_(AutoPtr<ISymbol>) Lookup(
        /* [in] */ const String& name,
        /* [in] */ Int32 arity);

    CARAPI_(AutoPtr<ISymbol>) LookupConst(
        /* [in] */ const String& name);

private:
    static AutoPtr<ArrayOf<ISymbol*> > sBuiltin;
    static const AutoPtr<Symbol> SHELL;
    static const AutoPtr<ArrayOf<String> > DEFINES;

    AutoPtr<Compiler> mCompiler;
    AutoPtr<IHashTable> mSymbols; //Hashtable<Symbol, Symbol>
    AutoPtr<IHashSet> mDelta;
    AutoPtr<IStack> mFrames;

    static StaticInitializer sInitializer;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

DEFINE_OBJECT_HASH_FUNC_FOR(Org::Javia::Arity::ISymbol)

#endif // _ORG_JAVIA_ARITY_CSYMBOLS_H__