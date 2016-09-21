/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         cssyyparse
#define yylex           cssyylex
#define yyerror         cssyyerror
#define yylval          cssyylval
#define yychar          cssyychar
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "css/CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *  Copyright (C) 2012 Intel Corporation. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSPropertyNames.h"
#include "HTMLNames.h"
#include "core/css/CSSKeyframeRule.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/parser/BisonCSSParser.h"
#include "core/css/CSSParserMode.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSSelector.h"
#include "core/css/CSSSelectorList.h"
#include "core/css/MediaList.h"
#include "core/css/MediaQueryExp.h"
#include "core/css/StyleRule.h"
#include "core/css/StyleSheetContents.h"
#include "core/dom/Document.h"
#include "wtf/FastMalloc.h"
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

#if YYDEBUG > 0
#define YYPRINT(File,Type,Value) if (isCSSTokenAString(Type)) YYFPRINTF(File, "%s", String((Value).string).utf8().data())
#endif



/* Line 268 of yacc.c  */
#line 142 "/home/whm/kt_work/Android/CM12_0-Android501/out/target/product/victara/obj/GYP/shared_intermediates/blink/core/CSSGrammar.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_EOF = 0,
     LOWEST_PREC = 258,
     UNIMPORTANT_TOK = 259,
     WHITESPACE = 260,
     SGML_CD = 261,
     INCLUDES = 262,
     DASHMATCH = 263,
     BEGINSWITH = 264,
     ENDSWITH = 265,
     CONTAINS = 266,
     STRING = 267,
     IDENT = 268,
     NTH = 269,
     HEX = 270,
     IDSEL = 271,
     IMPORT_SYM = 272,
     PAGE_SYM = 273,
     MEDIA_SYM = 274,
     SUPPORTS_SYM = 275,
     FONT_FACE_SYM = 276,
     CHARSET_SYM = 277,
     NAMESPACE_SYM = 278,
     VIEWPORT_RULE_SYM = 279,
     INTERNAL_DECLS_SYM = 280,
     INTERNAL_MEDIALIST_SYM = 281,
     INTERNAL_RULE_SYM = 282,
     INTERNAL_SELECTOR_SYM = 283,
     INTERNAL_VALUE_SYM = 284,
     INTERNAL_KEYFRAME_RULE_SYM = 285,
     INTERNAL_KEYFRAME_KEY_LIST_SYM = 286,
     INTERNAL_SUPPORTS_CONDITION_SYM = 287,
     KEYFRAMES_SYM = 288,
     WEBKIT_KEYFRAMES_SYM = 289,
     TOPLEFTCORNER_SYM = 290,
     TOPLEFT_SYM = 291,
     TOPCENTER_SYM = 292,
     TOPRIGHT_SYM = 293,
     TOPRIGHTCORNER_SYM = 294,
     BOTTOMLEFTCORNER_SYM = 295,
     BOTTOMLEFT_SYM = 296,
     BOTTOMCENTER_SYM = 297,
     BOTTOMRIGHT_SYM = 298,
     BOTTOMRIGHTCORNER_SYM = 299,
     LEFTTOP_SYM = 300,
     LEFTMIDDLE_SYM = 301,
     LEFTBOTTOM_SYM = 302,
     RIGHTTOP_SYM = 303,
     RIGHTMIDDLE_SYM = 304,
     RIGHTBOTTOM_SYM = 305,
     ATKEYWORD = 306,
     IMPORTANT_SYM = 307,
     MEDIA_ONLY = 308,
     MEDIA_NOT = 309,
     MEDIA_AND = 310,
     MEDIA_OR = 311,
     SUPPORTS_NOT = 312,
     SUPPORTS_AND = 313,
     SUPPORTS_OR = 314,
     REMS = 315,
     CHS = 316,
     QEMS = 317,
     EMS = 318,
     EXS = 319,
     PXS = 320,
     CMS = 321,
     MMS = 322,
     INS = 323,
     PTS = 324,
     PCS = 325,
     DEGS = 326,
     RADS = 327,
     GRADS = 328,
     TURNS = 329,
     MSECS = 330,
     SECS = 331,
     HERTZ = 332,
     KHERTZ = 333,
     DIMEN = 334,
     INVALIDDIMEN = 335,
     PERCENTAGE = 336,
     FLOATTOKEN = 337,
     INTEGER = 338,
     VW = 339,
     VH = 340,
     VMIN = 341,
     VMAX = 342,
     DPPX = 343,
     DPI = 344,
     DPCM = 345,
     FR = 346,
     URI = 347,
     FUNCTION = 348,
     ANYFUNCTION = 349,
     CUEFUNCTION = 350,
     NOTFUNCTION = 351,
     DISTRIBUTEDFUNCTION = 352,
     CALCFUNCTION = 353,
     HOSTFUNCTION = 354,
     HOSTCONTEXTFUNCTION = 355,
     UNICODERANGE = 356
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 68 "css/CSSGrammar.y"

    bool boolean;
    char character;
    int integer;
    double number;
    CSSParserString string;

    StyleRuleBase* rule;
    // The content of the three below HeapVectors are guaranteed to be kept alive by
    // the corresponding m_parsedRules, m_floatingMediaQueryExpList, and m_parsedKeyFrames
    // lists in BisonCSSParser.h.
    WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase> >* ruleList;
    WillBeHeapVector<OwnPtrWillBeMember<MediaQueryExp> >* mediaQueryExpList;
    WillBeHeapVector<RefPtrWillBeMember<StyleKeyframe> >* keyframeRuleList;
    CSSParserSelector* selector;
    Vector<OwnPtr<CSSParserSelector> >* selectorList;
    CSSSelector::MarginBoxType marginBox;
    CSSSelector::Relation relation;
    MediaQuerySet* mediaList;
    MediaQuery* mediaQuery;
    MediaQuery::Restrictor mediaQueryRestrictor;
    MediaQueryExp* mediaQueryExp;
    CSSParserValue value;
    CSSParserValueList* valueList;
    StyleKeyframe* keyframe;
    float val;
    CSSPropertyID id;
    CSSParserLocation location;



/* Line 293 of yacc.c  */
#line 312 "/home/whm/kt_work/Android/CM12_0-Android501/out/target/product/victara/obj/GYP/shared_intermediates/blink/core/CSSGrammar.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 343 of yacc.c  */
#line 98 "css/CSSGrammar.y"


static inline int cssyyerror(void*, const char*)
{
    return 1;
}

#if YYDEBUG > 0
static inline bool isCSSTokenAString(int yytype)
{
    switch (yytype) {
    case IDENT:
    case STRING:
    case NTH:
    case HEX:
    case IDSEL:
    case DIMEN:
    case INVALIDDIMEN:
    case URI:
    case FUNCTION:
    case ANYFUNCTION:
    case HOSTFUNCTION:
    case HOSTCONTEXTFUNCTION:
    case NOTFUNCTION:
    case CALCFUNCTION:
    case UNICODERANGE:
        return true;
    default:
        return false;
    }
}
#endif

inline static CSSParserValue makeOperatorValue(int value)
{
    CSSParserValue v;
    v.id = CSSValueInvalid;
    v.isInt = false;
    v.unit = CSSParserValue::Operator;
    v.iValue = value;
    return v;
}

inline static CSSParserValue makeIdentValue(CSSParserString string)
{
    CSSParserValue v;
    v.id = cssValueKeywordID(string);
    v.isInt = false;
    v.unit = CSSPrimitiveValue::CSS_IDENT;
    v.string = string;
    return v;
}



/* Line 343 of yacc.c  */
#line 380 "/home/whm/kt_work/Android/CM12_0-Android501/out/target/product/victara/obj/GYP/shared_intermediates/blink/core/CSSGrammar.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  35
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2129

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  122
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  139
/* YYNRULES -- Number of rules.  */
#define YYNRULES  372
/* YYNRULES -- Number of states.  */
#define YYNSTATES  708

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   356

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,   120,     2,   121,     2,     2,
     111,   108,    20,   114,   112,   118,    18,   117,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,   110,
       2,   119,   116,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,   109,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   113,    21,   107,   115,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,     9,    11,    13,    15,    17,    19,
      21,    23,    29,    35,    40,    45,    50,    56,    61,    66,
      68,    71,    72,    74,    75,    78,    81,    83,    85,    87,
      89,    91,    93,    95,    97,    98,   104,   107,   108,   112,
     114,   116,   118,   120,   122,   124,   126,   128,   130,   131,
     134,   137,   139,   142,   143,   147,   150,   152,   154,   156,
     158,   160,   162,   164,   166,   169,   172,   173,   177,   184,
     191,   198,   199,   202,   204,   206,   207,   211,   218,   223,
     225,   231,   233,   239,   240,   243,   246,   249,   253,   255,
     260,   264,   265,   267,   269,   272,   274,   279,   285,   286,
     287,   289,   293,   302,   304,   315,   316,   317,   319,   321,
     323,   325,   329,   334,   339,   344,   349,   355,   357,   364,
     375,   386,   387,   391,   395,   405,   407,   409,   411,   414,
     415,   420,   426,   432,   435,   441,   444,   446,   449,   450,
     461,   464,   468,   471,   472,   474,   479,   480,   488,   490,
     492,   494,   496,   498,   500,   502,   504,   506,   508,   510,
     512,   514,   516,   518,   520,   521,   530,   531,   540,   543,
     546,   549,   554,   556,   557,   559,   561,   563,   564,   565,
     566,   576,   577,   579,   586,   588,   591,   595,   599,   601,
     604,   607,   609,   612,   614,   617,   621,   624,   626,   632,
     634,   636,   638,   641,   643,   645,   647,   649,   651,   654,
     657,   662,   671,   677,   687,   691,   693,   695,   697,   699,
     701,   703,   705,   707,   710,   714,   719,   727,   733,   740,
     745,   752,   760,   767,   772,   779,   784,   791,   796,   803,
     808,   812,   813,   815,   818,   820,   824,   829,   836,   845,
     852,   857,   861,   865,   868,   869,   872,   876,   880,   885,
     890,   892,   896,   899,   903,   906,   909,   912,   916,   919,
     922,   925,   929,   932,   935,   938,   941,   944,   947,   950,
     953,   955,   957,   959,   961,   963,   965,   967,   969,   971,
     973,   975,   977,   979,   981,   983,   985,   987,   989,   991,
     993,   995,   997,   999,  1001,  1003,  1005,  1007,  1009,  1011,
    1013,  1018,  1022,  1027,  1029,  1032,  1036,  1040,  1044,  1048,
    1049,  1051,  1057,  1062,  1064,  1068,  1072,  1074,  1080,  1085,
    1087,  1089,  1093,  1097,  1100,  1103,  1106,  1110,  1114,  1120,
    1124,  1127,  1130,  1134,  1140,  1144,  1148,  1152,  1157,  1160,
    1163,  1164,  1168,  1172,  1176,  1178,  1180,  1182,  1184,  1186,
    1188,  1190,  1192,  1193,  1194,  1195,  1198,  1201,  1204,  1207,
    1208,  1211,  1214
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     123,     0,    -1,   139,   134,   140,    -1,   127,    -1,   124,
      -1,   130,    -1,   128,    -1,   129,    -1,   125,    -1,   126,
      -1,   131,    -1,    32,   133,   141,   133,     0,    -1,    35,
     133,   186,   133,     0,    -1,    36,   133,   187,     0,    -1,
      30,   204,   226,     0,    -1,    34,   133,   233,     0,    -1,
      31,   133,   258,   162,     0,    -1,    33,   133,   210,     0,
      -1,    37,   133,   174,     0,    -1,     5,    -1,   132,     5,
      -1,    -1,   132,    -1,    -1,   134,     6,    -1,   134,     5,
      -1,   107,    -1,     0,    -1,   108,    -1,     0,    -1,   109,
      -1,     0,    -1,   110,    -1,     0,    -1,    -1,    27,   133,
      12,   133,   138,    -1,    27,   246,    -1,    -1,   140,   143,
     134,    -1,   208,    -1,   169,    -1,   191,    -1,   198,    -1,
     182,    -1,   152,    -1,   151,    -1,   171,    -1,   200,    -1,
      -1,   142,   141,    -1,   142,   250,    -1,   145,    -1,   145,
     146,    -1,    -1,   145,   148,   134,    -1,   142,   251,    -1,
     208,    -1,   191,    -1,   198,    -1,   169,    -1,   182,    -1,
     171,    -1,   200,    -1,   152,    -1,   142,   147,    -1,   142,
     250,    -1,    -1,   149,    22,   133,    -1,   150,   154,   133,
     258,   162,   138,    -1,   150,   154,   133,   258,   162,   253,
      -1,    28,   133,   153,   154,   133,   138,    -1,    -1,    13,
     133,    -1,    12,    -1,    97,    -1,    -1,    17,   133,   233,
      -1,   111,   133,    13,   133,   155,   136,    -1,   111,     1,
     259,   136,    -1,   156,    -1,   157,   133,    60,   133,   156,
      -1,   133,    -1,   133,    60,   133,   157,   133,    -1,    -1,
      58,   133,    -1,    59,   133,    -1,   157,   133,    -1,   159,
     170,   158,    -1,   160,    -1,   160,     1,   257,   260,    -1,
       1,   257,   260,    -1,    -1,   163,    -1,   161,    -1,   164,
     161,    -1,   164,    -1,   161,   112,   133,   258,    -1,   164,
     161,   112,   133,   258,    -1,    -1,    -1,   133,    -1,   166,
      24,   133,    -1,   168,   162,   206,   113,   165,   133,   144,
     135,    -1,    13,    -1,   172,    25,   133,   174,   173,   113,
     165,   133,   144,   135,    -1,    -1,    -1,   178,    -1,   175,
      -1,   176,    -1,   177,    -1,    62,   133,   178,    -1,   178,
      63,   133,   178,    -1,   176,    63,   133,   178,    -1,   178,
      64,   133,   178,    -1,   177,    64,   133,   178,    -1,   111,
     133,   174,   136,   133,    -1,   179,    -1,   111,     1,   257,
     259,   136,   133,    -1,   111,   133,    13,   133,    17,   133,
     233,   230,   136,   133,    -1,   111,   133,    13,   133,    17,
     133,     1,   259,   136,   133,    -1,    -1,   180,    38,   133,
      -1,   180,    39,   133,    -1,   181,   183,   167,   113,   165,
     133,   258,   184,   135,    -1,    13,    -1,    12,    -1,   185,
      -1,   185,   189,    -1,    -1,   185,   186,   133,   258,    -1,
     185,   189,   253,   133,   258,    -1,   187,   113,   133,   226,
     135,    -1,   188,   133,    -1,   187,   112,   133,   188,   133,
      -1,   202,    86,    -1,    13,    -1,     1,   260,    -1,    -1,
     190,    23,   133,   192,   206,   113,   165,   204,   193,   135,
      -1,    13,   133,    -1,    13,   223,   133,    -1,   223,   133,
      -1,    -1,   226,    -1,   193,   194,   133,   226,    -1,    -1,
     196,   195,   133,   113,   133,   226,   135,    -1,    40,    -1,
      41,    -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      -1,   197,    26,   167,   113,   165,   204,   226,   135,    -1,
      -1,   199,    29,   167,   113,   165,   204,   226,   135,    -1,
     114,   133,    -1,   115,   133,    -1,   116,   133,    -1,   117,
      13,   117,   133,    -1,   203,    -1,    -1,   118,    -1,   114,
      -1,   133,    -1,    -1,    -1,    -1,   205,   210,   207,   206,
     113,   165,   204,   226,   135,    -1,    -1,   211,    -1,   210,
     207,   112,   133,   209,   211,    -1,   213,    -1,   211,     5,
      -1,   211,     5,   213,    -1,   211,   201,   213,    -1,    21,
      -1,    20,    21,    -1,    13,    21,    -1,   215,    -1,   215,
     216,    -1,   216,    -1,   212,   215,    -1,   212,   215,   216,
      -1,   212,   216,    -1,   213,    -1,   214,   133,   112,   133,
     213,    -1,    13,    -1,    20,    -1,   217,    -1,   216,   217,
      -1,    16,    -1,    15,    -1,   218,    -1,   220,    -1,   224,
      -1,    18,    13,    -1,    13,   133,    -1,    19,   133,   219,
     137,    -1,    19,   133,   219,   221,   133,   222,   133,   137,
      -1,    19,   133,   212,   219,   137,    -1,    19,   133,   212,
     219,   221,   133,   222,   133,   137,    -1,    19,   225,   137,
      -1,   119,    -1,     7,    -1,     8,    -1,     9,    -1,    10,
      -1,    11,    -1,    13,    -1,    12,    -1,    17,    13,    -1,
      17,   257,    13,    -1,    17,    17,   257,    13,    -1,    17,
      17,   100,   133,   214,   133,   136,    -1,    17,    17,   100,
     225,   136,    -1,    17,    99,   133,   214,   133,   136,    -1,
      17,    99,   225,   136,    -1,    17,    98,   133,    14,   133,
     136,    -1,    17,    98,   133,   202,    88,   133,   136,    -1,
      17,    98,   133,    13,   133,   136,    -1,    17,    98,   225,
     136,    -1,    17,   101,   133,   213,   133,   136,    -1,    17,
     101,   225,   136,    -1,    17,   104,   133,   214,   133,   136,
      -1,    17,   104,   225,   136,    -1,    17,   105,   133,   214,
     133,   136,    -1,    17,   105,   225,   136,    -1,     1,   257,
     259,    -1,    -1,   228,    -1,   227,   228,    -1,   227,    -1,
     228,   110,   133,    -1,   227,   228,   110,   133,    -1,   229,
      17,   133,   257,   233,   230,    -1,   229,    17,   133,   257,
     233,   230,     1,   259,    -1,   229,    17,   133,   257,     1,
     259,    -1,   229,     1,   257,   259,    -1,     1,   257,   259,
      -1,   257,    13,   133,    -1,    57,   133,    -1,    -1,    13,
     133,    -1,   231,    13,   133,    -1,   111,   133,   136,    -1,
     111,   133,   231,   136,    -1,   111,   133,   234,   136,    -1,
     236,    -1,   233,   235,   236,    -1,   233,   236,    -1,     1,
     257,   259,    -1,   117,   133,    -1,   112,   133,    -1,   237,
     133,    -1,   203,   237,   133,    -1,    12,   133,    -1,    13,
     133,    -1,    84,   133,    -1,   203,    84,   133,    -1,    97,
     133,    -1,   106,   133,    -1,    15,   133,    -1,   120,   133,
      -1,   238,   133,    -1,   244,   133,    -1,   121,   133,    -1,
     232,   133,    -1,    88,    -1,    87,    -1,    86,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    82,    -1,    83,    -1,    68,    -1,    67,
      -1,    69,    -1,    65,    -1,    66,    -1,    89,    -1,    90,
      -1,    91,    -1,    92,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,    98,   133,   233,   136,    -1,    98,   133,
     136,    -1,    98,   133,   234,   136,    -1,   237,    -1,   203,
     237,    -1,   132,   114,   132,    -1,   132,   118,   132,    -1,
     241,    20,   133,    -1,   241,   117,   133,    -1,    -1,     5,
      -1,   111,   133,   243,   241,   136,    -1,   111,   133,   234,
     136,    -1,   239,    -1,   243,   240,   239,    -1,   243,   240,
     242,    -1,   242,    -1,   103,   133,   243,   241,   136,    -1,
     103,   133,   234,   136,    -1,    56,    -1,   196,    -1,   247,
     252,   248,    -1,     1,   257,   260,    -1,   252,   138,    -1,
     252,   253,    -1,   181,   247,    -1,   190,    23,   247,    -1,
     197,    26,   247,    -1,   172,    25,     1,   257,   260,    -1,
     199,    29,   247,    -1,   150,   247,    -1,    28,   247,    -1,
     257,   245,   247,    -1,     1,   257,   260,   252,   253,    -1,
     249,   252,   110,    -1,   249,   252,   253,    -1,   168,   162,
     110,    -1,     1,   257,   260,   252,    -1,   249,   252,    -1,
     168,   162,    -1,    -1,   113,   259,   135,    -1,    19,   259,
     137,    -1,   256,   259,   136,    -1,   111,    -1,    98,    -1,
     103,    -1,    99,    -1,   101,    -1,   100,    -1,   102,    -1,
     104,    -1,    -1,    -1,    -1,   259,     1,    -1,   259,   253,
      -1,   259,   254,    -1,   259,   255,    -1,    -1,   260,     1,
      -1,   260,   254,    -1,   260,   255,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   374,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   386,   392,   398,   404,   410,   420,   426,   433,   439,
     440,   444,   445,   448,   450,   451,   455,   456,   460,   461,
     465,   466,   470,   471,   474,   476,   481,   484,   486,   493,
     494,   495,   496,   497,   498,   499,   500,   501,   505,   511,
     516,   523,   524,   528,   529,   535,   541,   542,   543,   544,
     545,   546,   547,   548,   552,   556,   563,   569,   576,   579,
     585,   592,   593,   597,   598,   602,   605,   611,   617,   623,
     627,   634,   637,   643,   646,   649,   655,   658,   665,   666,
     670,   677,   680,   684,   688,   692,   699,   703,   710,   716,
     722,   728,   731,   737,   741,   747,   754,   761,   762,   763,
     764,   768,   774,   777,   783,   786,   792,   795,   796,   803,
     817,   824,   830,   833,   839,   845,   846,   850,   851,   856,
     860,   864,   871,   877,   881,   888,   891,   903,   909,   915,
     929,   933,   938,   942,   949,   950,   954,   954,   962,   965,
     968,   971,   974,   977,   980,   983,   986,   989,   992,   995,
     998,  1001,  1004,  1007,  1013,  1019,  1026,  1033,  1041,  1042,
    1043,  1044,  1053,  1054,  1058,  1059,  1063,  1069,  1076,  1082,
    1088,  1094,  1099,  1104,  1111,  1112,  1113,  1124,  1137,  1138,
    1139,  1143,  1146,  1151,  1156,  1161,  1166,  1174,  1178,  1185,
    1190,  1197,  1198,  1204,  1211,  1222,  1223,  1224,  1228,  1238,
    1246,  1251,  1257,  1262,  1268,  1274,  1277,  1280,  1283,  1286,
    1289,  1295,  1296,  1300,  1313,  1326,  1341,  1350,  1358,  1368,
    1372,  1382,  1392,  1409,  1413,  1428,  1431,  1441,  1445,  1455,
    1461,  1464,  1465,  1466,  1469,  1473,  1477,  1484,  1501,  1508,
    1514,  1520,  1527,  1536,  1537,  1541,  1545,  1552,  1555,  1558,
    1564,  1568,  1573,  1580,  1586,  1589,  1595,  1596,  1597,  1598,
    1600,  1601,  1602,  1603,  1604,  1605,  1607,  1608,  1609,  1612,
    1616,  1617,  1618,  1619,  1620,  1621,  1622,  1623,  1624,  1625,
    1626,  1627,  1628,  1629,  1630,  1631,  1632,  1633,  1634,  1635,
    1636,  1641,  1642,  1643,  1644,  1645,  1646,  1647,  1648,  1649,
    1653,  1656,  1659,  1665,  1666,  1670,  1673,  1676,  1679,  1684,
    1686,  1690,  1695,  1701,  1705,  1710,  1715,  1719,  1722,  1729,
    1730,  1734,  1738,  1744,  1745,  1749,  1750,  1751,  1752,  1756,
    1759,  1760,  1761,  1768,  1771,  1772,  1773,  1777,  1780,  1781,
    1785,  1791,  1797,  1801,  1804,  1804,  1804,  1804,  1804,  1804,
    1804,  1804,  1807,  1812,  1817,  1819,  1820,  1821,  1822,  1825,
    1827,  1828,  1829
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'",
  "'.'", "'['", "'*'", "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM",
  "SUPPORTS_SYM", "FONT_FACE_SYM", "CHARSET_SYM", "NAMESPACE_SYM",
  "VIEWPORT_RULE_SYM", "INTERNAL_DECLS_SYM", "INTERNAL_MEDIALIST_SYM",
  "INTERNAL_RULE_SYM", "INTERNAL_SELECTOR_SYM", "INTERNAL_VALUE_SYM",
  "INTERNAL_KEYFRAME_RULE_SYM", "INTERNAL_KEYFRAME_KEY_LIST_SYM",
  "INTERNAL_SUPPORTS_CONDITION_SYM", "KEYFRAMES_SYM",
  "WEBKIT_KEYFRAMES_SYM", "TOPLEFTCORNER_SYM", "TOPLEFT_SYM",
  "TOPCENTER_SYM", "TOPRIGHT_SYM", "TOPRIGHTCORNER_SYM",
  "BOTTOMLEFTCORNER_SYM", "BOTTOMLEFT_SYM", "BOTTOMCENTER_SYM",
  "BOTTOMRIGHT_SYM", "BOTTOMRIGHTCORNER_SYM", "LEFTTOP_SYM",
  "LEFTMIDDLE_SYM", "LEFTBOTTOM_SYM", "RIGHTTOP_SYM", "RIGHTMIDDLE_SYM",
  "RIGHTBOTTOM_SYM", "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY",
  "MEDIA_NOT", "MEDIA_AND", "MEDIA_OR", "SUPPORTS_NOT", "SUPPORTS_AND",
  "SUPPORTS_OR", "REMS", "CHS", "QEMS", "EMS", "EXS", "PXS", "CMS", "MMS",
  "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS", "MSECS", "SECS",
  "HERTZ", "KHERTZ", "DIMEN", "INVALIDDIMEN", "PERCENTAGE", "FLOATTOKEN",
  "INTEGER", "VW", "VH", "VMIN", "VMAX", "DPPX", "DPI", "DPCM", "FR",
  "URI", "FUNCTION", "ANYFUNCTION", "CUEFUNCTION", "NOTFUNCTION",
  "DISTRIBUTEDFUNCTION", "CALCFUNCTION", "HOSTFUNCTION",
  "HOSTCONTEXTFUNCTION", "UNICODERANGE", "'}'", "')'", "']'", "';'", "'('",
  "','", "'{'", "'+'", "'~'", "'>'", "'/'", "'-'", "'='", "'#'", "'%'",
  "$accept", "stylesheet", "internal_rule", "internal_keyframe_rule",
  "internal_keyframe_key_list", "internal_decls", "internal_value",
  "internal_medialist", "internal_selector", "internal_supports_condition",
  "space", "maybe_space", "maybe_sgml", "closing_brace",
  "closing_parenthesis", "closing_square_bracket", "semi_or_eof",
  "maybe_charset", "rule_list", "valid_rule", "before_rule", "rule",
  "block_rule_body", "block_rule_list", "block_rule_recovery",
  "block_valid_rule", "block_rule", "before_import_rule",
  "import_rule_start", "import", "namespace", "maybe_ns_prefix",
  "string_or_uri", "maybe_media_value", "media_query_exp",
  "media_query_exp_list", "maybe_and_media_query_exp_list",
  "maybe_media_restrictor", "valid_media_query", "media_query",
  "maybe_media_list", "media_list", "mq_list", "at_rule_body_start",
  "before_media_rule", "at_rule_header_end_maybe_space",
  "media_rule_start", "media", "medium", "supports",
  "before_supports_rule", "at_supports_rule_header_end",
  "supports_condition", "supports_negation", "supports_conjunction",
  "supports_disjunction", "supports_condition_in_parens",
  "supports_declaration_condition", "before_keyframes_rule",
  "keyframes_rule_start", "keyframes", "keyframe_name", "keyframes_rule",
  "keyframe_rule_list", "keyframe_rule", "key_list", "key",
  "keyframes_error_recovery", "before_page_rule", "page", "page_selector",
  "declarations_and_margins", "margin_box", "$@1", "margin_sym",
  "before_font_face_rule", "font_face", "before_viewport_rule", "viewport",
  "combinator", "maybe_unary_operator", "unary_operator",
  "maybe_space_before_declaration", "before_selector_list",
  "at_rule_header_end", "at_selector_end", "ruleset",
  "before_selector_group_item", "selector_list", "selector",
  "namespace_selector", "simple_selector", "simple_selector_list",
  "element_name", "specifier_list", "specifier", "class", "attr_name",
  "attrib", "match", "ident_or_string", "pseudo_page", "pseudo",
  "selector_recovery", "declaration_list", "decl_list", "declaration",
  "property", "prio", "ident_list", "track_names_list", "expr",
  "expr_recovery", "operator", "term", "unary_term", "function",
  "calc_func_term", "calc_func_operator", "calc_maybe_space",
  "calc_func_paren_expr", "calc_func_expr", "calc_function", "invalid_at",
  "at_rule_recovery", "at_rule_header_recovery", "at_rule_end",
  "regular_invalid_at_rule_header", "invalid_rule", "invalid_rule_header",
  "at_invalid_rule_header_end", "invalid_block",
  "invalid_square_brackets_block", "invalid_parentheses_block",
  "opening_parenthesis", "error_location", "location_label",
  "error_recovery", "rule_error_recovery", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    58,    46,    91,
      42,   124,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   125,    41,    93,
      59,    40,    44,   123,    43,   126,    62,    47,    45,    61,
      35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   122,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     132,   133,   133,   134,   134,   134,   135,   135,   136,   136,
     137,   137,   138,   138,   139,   139,   139,   140,   140,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   142,   143,
     143,   144,   144,   145,   145,   146,   147,   147,   147,   147,
     147,   147,   147,   147,   148,   148,   149,   150,   151,   151,
     152,   153,   153,   154,   154,   155,   155,   156,   156,   157,
     157,   158,   158,   159,   159,   159,   160,   160,   161,   161,
     161,   162,   162,   163,   163,   163,   164,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   174,   174,
     174,   175,   176,   176,   177,   177,   178,   178,   178,   179,
     179,   180,   181,   181,   182,   183,   183,   184,   184,   185,
     185,   185,   186,   187,   187,   188,   188,   189,   190,   191,
     192,   192,   192,   192,   193,   193,   195,   194,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   197,   198,   199,   200,   201,   201,
     201,   201,   202,   202,   203,   203,   204,   205,   206,   207,
     208,   209,   210,   210,   211,   211,   211,   211,   212,   212,
     212,   213,   213,   213,   213,   213,   213,   214,   214,   215,
     215,   216,   216,   217,   217,   217,   217,   217,   218,   219,
     220,   220,   220,   220,   220,   221,   221,   221,   221,   221,
     221,   222,   222,   223,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     225,   226,   226,   226,   226,   227,   227,   228,   228,   228,
     228,   228,   229,   230,   230,   231,   231,   232,   232,   232,
     233,   233,   233,   234,   235,   235,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     238,   238,   238,   239,   239,   240,   240,   240,   240,   241,
     241,   242,   242,   243,   243,   243,   243,   244,   244,   245,
     245,   246,   247,   248,   248,   249,   249,   249,   249,   249,
     249,   249,   249,   250,   250,   250,   250,   251,   251,   251,
     252,   253,   254,   255,   256,   256,   256,   256,   256,   256,
     256,   256,   257,   258,   259,   259,   259,   259,   259,   260,
     260,   260,   260
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     5,     5,     4,     4,     4,     5,     4,     4,     1,
       2,     0,     1,     0,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     5,     2,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     1,     2,     0,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     0,     3,     6,     6,
       6,     0,     2,     1,     1,     0,     3,     6,     4,     1,
       5,     1,     5,     0,     2,     2,     2,     3,     1,     4,
       3,     0,     1,     1,     2,     1,     4,     5,     0,     0,
       1,     3,     8,     1,    10,     0,     0,     1,     1,     1,
       1,     3,     4,     4,     4,     4,     5,     1,     6,    10,
      10,     0,     3,     3,     9,     1,     1,     1,     2,     0,
       4,     5,     5,     2,     5,     2,     1,     2,     0,    10,
       2,     3,     2,     0,     1,     4,     0,     7,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     8,     0,     8,     2,     2,
       2,     4,     1,     0,     1,     1,     1,     0,     0,     0,
       9,     0,     1,     6,     1,     2,     3,     3,     1,     2,
       2,     1,     2,     1,     2,     3,     2,     1,     5,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     2,     2,
       4,     8,     5,     9,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     4,     7,     5,     6,     4,
       6,     7,     6,     4,     6,     4,     6,     4,     6,     4,
       3,     0,     1,     2,     1,     3,     4,     6,     8,     6,
       4,     3,     3,     2,     0,     2,     3,     3,     4,     4,
       1,     3,     2,     3,     2,     2,     2,     3,     2,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     3,     4,     1,     2,     3,     3,     3,     3,     0,
       1,     5,     4,     1,     3,     3,     1,     5,     4,     1,
       1,     3,     3,     2,     2,     2,     3,     3,     5,     3,
       2,     2,     3,     5,     3,     3,     3,     4,     2,     2,
       0,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     0,     0,     2,     2,     2,     2,     0,
       2,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      34,     0,    21,    21,    21,    21,    21,    21,    21,    21,
       0,     4,     8,     9,     3,     6,     7,     5,    10,    23,
     362,    19,    22,     0,    36,   350,   176,     0,   363,   177,
       0,     0,   173,   173,     0,     1,    37,   369,    20,    21,
     350,   362,     0,     0,   242,     0,     0,     0,    21,    21,
       0,     0,    45,    44,     0,     0,    40,    46,     0,     0,
       0,    43,     0,    41,     0,    42,     0,    47,     0,    39,
     199,   204,   203,   362,     0,     0,   200,   188,   179,   182,
       0,   184,   191,   193,   201,   205,   206,   207,    21,    21,
      21,   300,   301,   298,   297,   299,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
      21,   282,   281,   280,   302,   303,   304,   305,   306,   307,
     308,   309,    21,    21,    21,    21,    21,   175,   174,    21,
      21,     0,    21,     0,   260,    21,    21,    21,   136,    21,
       0,    21,     0,   172,     0,    21,     0,     0,   108,   109,
     110,   107,   117,    25,    24,    48,     0,     0,   331,     0,
     364,    14,   243,    21,   362,    21,    21,   362,    21,    21,
       0,    79,    21,     0,     0,    93,     0,    92,     0,    71,
       0,    21,    73,    74,    21,    21,   178,    21,    21,    21,
     126,   125,    21,    21,    21,    21,   179,   190,   362,     0,
       0,     0,     0,     0,     0,   208,   362,     0,     0,   189,
      17,     0,   185,    21,    21,    21,     0,     0,   199,   200,
     194,   196,   192,   202,   268,   269,   274,   270,   272,     0,
       0,   273,     0,   275,   278,    21,    21,   279,    15,    21,
      21,     0,   262,   266,   276,   277,     0,    21,    21,   133,
     135,    13,     0,   362,     0,    18,    21,    21,    21,    21,
       0,    23,   370,   364,   355,   357,   359,   358,   360,   356,
     361,   354,   371,   372,   364,    33,    32,    35,   364,   333,
     334,     0,    21,   245,   364,   362,   252,   369,    84,    85,
     364,     0,    86,   103,    21,   362,    21,    16,    94,    21,
       0,    11,    67,   363,   101,     0,     0,   122,   123,   100,
       0,   143,     0,     0,   178,     0,     0,   173,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   224,   364,    21,
       0,     0,     0,    31,    30,   214,    21,   186,   168,   169,
     170,     0,   187,   195,    29,   362,    28,   311,     0,     0,
      21,     0,     0,   313,   323,   326,   319,    21,   257,     0,
       0,   271,   267,   265,   264,   261,    12,   173,     0,   111,
     364,    21,     0,     0,     0,     0,     0,   362,     0,    49,
       0,     0,     0,     0,     0,     0,     0,   350,    50,     0,
      38,     0,     0,     0,   365,   366,   367,   368,   246,     0,
       0,     0,     0,    21,    21,    81,    87,   369,   363,    21,
      72,    21,     0,    98,   106,    98,    21,     0,   178,    21,
      98,    98,     0,     0,     0,   225,    21,    21,     0,   233,
     197,    21,   229,    21,   235,    21,   237,    21,   239,     0,
     209,    21,     0,   216,   217,   218,   219,   220,   215,   210,
      21,   181,    21,   364,   310,   312,     0,   314,   328,   320,
       0,     0,     0,   255,    21,   258,   259,    21,     0,     0,
       0,    21,   113,   115,   112,   114,   369,   341,   340,   178,
       0,   335,     0,     0,     0,     0,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   329,   330,     0,   352,   353,    27,    26,   351,
     364,   254,    78,    75,     0,    21,     0,    96,   363,     0,
       0,    21,     0,    21,   140,    21,   223,     0,   142,    21,
      21,    98,    21,   227,     0,     0,    21,     0,     0,     0,
       0,   212,    21,     0,     0,   171,     0,     0,   319,     0,
       0,   324,   325,    21,    21,   327,   256,   134,   132,    21,
      21,   116,     0,   346,   362,   336,   337,   339,   344,   345,
     342,     0,    21,     0,    21,     0,    80,     0,    97,    70,
      68,    69,    53,    98,   363,   141,    98,     0,     0,    21,
       0,   232,   230,     0,    21,   228,   234,   236,   238,     0,
     222,   221,    21,   183,   322,     0,   315,   316,   317,   318,
     118,     0,     0,   369,   253,   364,     0,    77,    21,     0,
      48,    21,   129,    21,     0,     0,     0,   226,   231,     0,
      21,     0,   321,   364,   254,   343,     0,     0,    76,    82,
     102,     0,    52,    23,    53,     0,     0,     0,   165,   167,
       0,   198,     0,   211,     0,     0,   362,    64,     0,    63,
       0,    59,    61,    60,    57,    58,    62,    56,   350,    65,
      55,    54,     0,   124,   369,    21,   128,     0,   144,   180,
     213,    21,    21,   369,   349,   348,   104,     0,   363,    21,
     139,    21,   146,   120,   119,     0,   130,   363,     0,    21,
     347,   131,   145,     0,    21,     0,     0,   147
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      22,    26,    36,   509,   347,   335,   277,    19,   155,    49,
     260,   261,   619,   620,   642,   657,   643,    50,    51,    52,
      53,   300,   184,   575,   171,   172,   406,   173,   174,   175,
     176,   177,   178,   521,    54,   312,    55,    56,   294,    57,
     382,   522,   147,   148,   149,   150,   151,   152,    59,   383,
      61,   192,   645,   646,   139,   140,   141,   676,   384,    63,
     418,   677,   691,   699,   503,   385,    65,   386,    67,   217,
     142,   131,    27,    68,   305,   211,    69,   544,    78,    79,
      80,   430,   431,    82,    83,    84,    85,   332,    86,   450,
     602,   419,    87,   208,    42,    43,    44,    45,   573,   359,
     132,   133,   349,   241,   134,   135,   136,   354,   461,   462,
     355,   356,   137,   504,    24,   478,   158,   387,   388,   670,
      40,   395,   396,   397,   274,    46,    47,   281,   156
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -366
static const yytype_int16 yypact[] =
{
     795,   368,    45,    45,    45,    45,    45,    45,    45,    45,
      76,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  -366,    87,   100,  -366,  -366,  -366,   303,  -366,   820,
     859,  1739,   183,   183,    -4,  -366,   277,  -366,  -366,    45,
    -366,  -366,   127,  1838,    20,    62,   124,   271,    45,    45,
     120,   229,  -366,  -366,   149,   252,  -366,  -366,   178,   283,
     321,  -366,   213,  -366,   226,  -366,   235,  -366,   859,  -366,
     248,  -366,  -366,   444,   279,   505,   268,  -366,   300,   228,
     953,  -366,   469,   469,  -366,  -366,  -366,  -366,    45,    45,
      45,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
      45,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  -366,    45,    45,    45,    45,    45,  -366,  -366,    45,
      45,  1847,    45,   821,  -366,    45,    45,    45,  -366,    45,
     266,    45,   219,  -366,    21,    45,    38,   309,  -366,   256,
     251,   324,  -366,  -366,  -366,   325,   633,    16,  -366,    33,
    -366,  -366,   221,    45,  -366,    45,    45,  -366,    45,    45,
      80,  -366,    45,   338,    44,   225,   364,  -366,   361,   358,
     376,    45,  -366,  -366,    45,    45,  -366,    45,    45,    45,
    -366,  -366,    45,    45,    45,    45,  -366,  -366,   289,    64,
     638,   638,   638,   638,   379,  -366,  -366,   334,    30,  -366,
    -366,   292,   859,    45,    45,    45,   393,   859,  -366,  -366,
     469,   469,   469,  -366,  -366,  -366,  -366,  -366,  -366,   936,
    1193,  -366,   224,  -366,  -366,    45,    45,  -366,  -366,    45,
      45,  1739,  -366,  -366,  -366,  -366,   409,    45,    45,  -366,
    -366,  -366,   311,  -366,   237,  -366,    45,    45,    45,    45,
    1931,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  1277,    45,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,   403,   367,  -366,    45,  -366,    45,  -366,   323,    45,
     229,  -366,  -366,  -366,  -366,   308,    -4,  -366,  -366,  -366,
     326,    97,   329,   330,   292,   638,   425,   310,    82,   859,
      82,   859,    82,   859,    82,   859,    82,  -366,  -366,    49,
     268,   434,   448,  -366,  -366,  -366,    45,  -366,  -366,  -366,
    -366,   333,  -366,   469,  -366,  -366,  -366,  -366,   682,    82,
      45,  2033,    82,  -366,  -366,  -366,   447,    45,  -366,   230,
      82,  -366,  -366,  -366,  -366,  -366,  -366,   183,   104,  -366,
    -366,    45,    82,   311,   311,   311,   311,  -366,   432,  -366,
      79,   728,   428,   305,   431,   436,   435,  -366,  -366,  1734,
     277,   976,  1065,  1350,  -366,  -366,  -366,  -366,  -366,  1293,
    1036,   476,  1065,    45,    45,   405,  -366,  -366,  -366,    45,
    -366,    45,   591,  -366,  -366,  -366,    85,   454,  -366,    45,
    -366,  -366,   347,   859,    82,  -366,    45,    45,   381,  -366,
    -366,    45,  -366,    45,  -366,    45,  -366,    45,  -366,   854,
    -366,    45,   448,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
      45,  -366,    45,  -366,  -366,  -366,  1193,  -366,  -366,   103,
     108,  1979,    53,  -366,    45,  -366,  -366,    45,    46,  1065,
     458,    45,  -366,  -366,  -366,  -366,  -366,  -366,  -366,   370,
      70,  -366,   262,   110,   110,   -27,  -366,  -366,  -366,  -366,
    -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  -366,  -366,  -366,   477,  -366,  -366,  -366,  -366,  -366,
    -366,  1565,  -366,   472,   382,    45,   607,  -366,  -366,    16,
      33,    45,   391,    45,  -366,    45,  -366,   394,  -366,    45,
      45,  -366,    45,  -366,    82,    82,    45,   166,    82,   166,
     166,  -366,    45,   400,   859,  -366,  1424,    82,   447,    45,
      45,  -366,  -366,    45,    45,  -366,  -366,  -366,  -366,    45,
      45,  -366,   398,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,  1367,    45,  1519,    45,    82,  -366,   382,  -366,  -366,
    -366,  -366,  -366,  -366,  -366,  -366,  -366,   104,   104,    45,
     166,  -366,  -366,    82,    45,  -366,  -366,  -366,  -366,   400,
    -366,  -366,    45,   228,  -366,    53,    87,    87,  -366,  -366,
    -366,  1136,   402,  -366,  -366,  -366,  1739,  -366,    45,    46,
      72,    45,  -366,    45,    46,    46,   104,  -366,  -366,   859,
      45,    30,  -366,  -366,  1565,  -366,   960,  1441,  1652,   367,
    -366,  1987,  -366,  -366,  -366,    46,   159,  1854,  -366,  -366,
      46,  -366,    30,  -366,  1065,    82,  -366,  -366,   477,  -366,
     614,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -366,   277,    46,  -366,  -366,    45,   402,  1650,  -366,  -366,
    -366,    45,    45,  -366,     6,   -27,  -366,  1497,  -366,    45,
    -366,    45,  -366,  -366,  -366,  1514,  -366,  -366,  1854,    45,
     402,  -366,  -366,   406,    45,   104,    46,  -366
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,  -366,
    -330,    -1,  -246,  -365,   276,  -292,  -125,  -366,  -366,   234,
    -104,  -366,  -123,  -366,  -366,  -366,  -366,  -366,  -241,  -366,
    -118,  -366,   227,  -366,    14,   -44,  -366,  -366,  -366,   352,
     -54,  -366,  -366,  -354,  -366,  -164,  -236,  -107,  -366,  -103,
     510,  -366,  -205,  -366,  -366,  -366,  -168,  -366,  -366,   511,
     -97,  -366,  -366,  -366,   -99,   517,   184,  -366,   531,   -78,
    -366,  -366,  -366,  -366,  -112,   537,   -73,   540,   -71,  -366,
     255,   -15,  -239,  -366,  -255,   385,   -59,  -366,   516,    41,
     383,    -8,   -83,   515,   -25,   -47,  -366,   272,  -366,   155,
       0,   189,  -366,  -105,  -339,  -366,   566,  -366,   -24,  -366,
    -366,  -215,  -195,  -366,  -121,   -89,  -366,   150,  -366,    65,
     151,   160,  -366,  -366,  -366,     8,  -366,   -23,   -22,  -366,
     -30,  -139,  -133,  -131,  -366,    -9,  -290,  -222,  -260
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -363
static const yytype_int16 yytable[] =
{
      23,   186,    28,    29,    30,    31,    32,    33,    34,    25,
     159,    37,   242,   412,   348,   390,   275,   143,   143,   380,
     280,   251,    81,   272,   381,   273,   460,   401,   310,   468,
     333,   313,   160,   275,   279,   352,   223,   360,   157,   253,
     449,   391,   236,    21,   -88,   295,   507,   179,   180,   372,
      21,   -21,   392,   344,    21,   221,   393,   222,   145,   422,
      81,   523,   399,   164,   204,   206,   529,   530,   402,    21,
     197,   564,   -51,   553,   207,    21,    35,   -21,   -21,   165,
      20,   290,   344,   568,   369,    21,   278,   224,   225,   226,
      21,   182,    38,   -21,   318,   320,   322,   324,   326,   505,
     -21,   414,   417,   558,  -241,    41,   439,   146,   -19,   227,
     416,    20,    39,    38,   417,    21,   563,  -362,   517,  -178,
     365,   228,   229,   230,   231,   232,   276,   161,   233,   234,
     163,   237,   -21,   247,   243,   244,   245,   166,   246,   334,
     249,   353,   181,   276,   252,   254,   278,   516,   469,   -21,
     541,   -88,   -21,   508,   -88,   284,   -88,   -88,   287,  -127,
     674,   346,   283,   527,   285,   286,   344,   288,   289,   291,
     554,   292,   138,   185,   223,   223,   183,   589,   -21,   -51,
     302,   -21,   -21,   303,   304,   511,   306,   307,   308,   316,
     346,   309,   311,   309,   309,   343,   138,   328,   317,   319,
     321,   323,   325,   187,   337,   472,   473,   474,   475,   342,
     424,  -241,   338,   339,   340,   351,   562,   -19,   460,   606,
     607,   -19,   549,   -21,   344,   345,   550,   242,   578,   621,
     344,   546,   623,   212,   361,   362,   193,   357,   363,   364,
     435,   182,   437,   464,   370,  -173,   367,   368,   624,   625,
     371,   389,   194,   167,   640,   373,   374,   375,   376,   648,
     649,   547,   457,    20,   195,   -83,  -127,    21,   272,   197,
     273,   -91,   167,   127,   346,   -21,   400,   128,   594,   -21,
     673,   398,   153,   154,   -83,   679,   407,   650,   571,   209,
     587,   588,   205,   405,   622,   408,   223,   127,   410,   145,
     210,   128,   143,  -241,    41,   250,    20,   686,   678,   255,
     168,   169,   690,   433,   423,   257,  -362,   190,   191,   256,
     313,   188,   189,   426,   427,    -2,   183,   479,   440,   168,
     169,   282,   346,   190,   191,   451,   453,   296,   346,   653,
     532,   707,   213,   214,   215,   216,   569,   329,   146,   456,
     626,   293,   143,   636,   330,    77,   463,   485,   520,   702,
     680,   -95,   167,   170,   297,   -91,   706,   353,   476,    20,
     470,   299,   353,    21,   -83,   -21,   301,   179,   247,   248,
     -21,   581,   170,   272,   647,   273,   477,   258,   259,   315,
     242,   481,   327,   637,   579,   580,   634,   671,   696,   262,
     658,   638,   513,   514,   336,   660,   341,   701,   518,   366,
     519,   654,   600,   601,   687,   524,   403,   263,   528,   168,
     169,   413,   146,   695,   127,   534,   535,   404,   128,   272,
     537,   273,   538,    20,   539,   409,   540,    21,   425,   415,
     440,   351,   420,   421,   -21,   -21,   351,   441,   333,   543,
     452,   545,   459,   480,   482,   443,   444,   445,   446,   447,
     531,   198,   483,   556,   484,   515,   557,   526,   -95,   536,
     561,   -95,   170,   635,   -95,   560,   -90,   262,    20,   306,
     563,   311,   309,   309,    71,    72,    73,    74,    75,   574,
     565,   566,   567,   170,   379,   263,   264,   265,   266,   267,
     268,   269,   270,   272,   583,   273,   206,   586,   358,   271,
      21,  -350,   570,   242,   577,   278,   641,   242,   -21,   704,
     582,   672,   584,   659,   585,   -21,   -21,   411,   576,   -21,
     298,   590,   612,   618,   661,   593,    81,   689,   662,    58,
      60,   599,   199,   200,   663,   201,   569,   675,   202,   203,
     144,   467,   608,   609,   272,   613,   273,   334,   610,   611,
      62,   635,   272,   664,   273,   692,    64,   448,   665,    66,
     666,   614,   428,   616,   264,   265,   266,   267,   268,   269,
     270,   314,   667,   -90,   196,   603,   -90,   271,   -90,   -90,
     331,   -91,   167,   629,   429,   220,   432,   542,   434,   630,
     436,   631,   438,   442,   -83,   525,   684,   -89,   262,   162,
     655,   551,   552,   605,   -91,   167,   548,   639,   668,   669,
     644,   651,     0,     0,   454,   455,   263,   -83,   458,   652,
       0,   143,   389,  -332,   262,   465,   466,     0,   685,   206,
       0,     0,     0,    21,     0,     0,     0,   683,   471,   168,
     169,   -21,   263,   -21,   -21,   -21,   -21,   -21,   -21,   -21,
       0,     0,     0,     0,     0,   700,     0,     0,   506,     0,
       0,     0,   168,   169,   688,     0,     0,     0,   512,     0,
     693,   694,   344,     0,     0,     0,     0,     0,   697,     0,
     698,     0,     0,     0,    88,    89,     0,    90,   703,     0,
     533,   -91,   170,   705,   -91,   264,   265,   266,   267,   268,
     269,   270,     0,     0,   -89,     0,     0,   -89,   271,   -89,
     -89,   -91,     0,     0,   -91,   170,     0,   -91,     0,   167,
       0,   264,   265,   266,   267,   268,   269,   270,   555,     0,
    -332,   -83,     0,  -332,   271,   559,  -332,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,     0,     0,     0,     0,   124,   168,   169,   125,     0,
     346,     0,     0,   126,   239,     0,   127,     0,     0,   240,
     128,     0,   129,   130,     0,     0,     0,     0,     0,     0,
     591,   592,     0,   595,   596,   597,   598,     0,     0,     0,
       0,   238,     1,   604,     0,     2,     3,     4,     5,     6,
       7,     8,     9,    88,    89,     0,    90,     0,   -91,   170,
       0,   -91,   -66,  -138,   -99,  -105,  -164,     0,    48,  -166,
       0,   617,     0,     0,  -240,   394,     0,     0,  -121,  -121,
       0,     0,     0,     0,     0,     0,   627,     0,     0,   628,
       0,     0,    70,   263,    71,    72,    73,    74,    75,    76,
      77,   632,     0,     0,     0,     0,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
       0,     0,     0,     0,   124,     0,     0,   125,     0,     0,
     681,   682,   126,   239,     0,   127,   344,   345,   240,   128,
       0,   129,   130,     0,     0,     0,     0,     0,    88,    89,
       0,    90,   264,   265,   266,   267,   268,   269,   270,     0,
    -338,   262,  -240,  -240,     0,   271,   218,   278,    71,    72,
      73,    74,    75,   219,     0,     0,   333,   394,     0,   263,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   263,     0,     0,     0,     0,
       0,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,     0,     0,   510,     0,   124,
       0,     0,   125,     0,   346,     0,     0,   126,    88,    89,
     127,    90,     0,     0,   128,     0,   129,   130,   264,   265,
     266,   267,   268,   269,   270,   344,   394,  -338,     0,     0,
    -338,   271,     0,  -338,   264,   265,   266,   267,   268,   269,
     270,     0,     0,     0,   263,   334,     0,   271,     0,   278,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,     0,     0,   633,     0,   124,
       0,     0,   125,     0,     0,     0,     0,   126,    88,    89,
     127,    90,     0,     0,   128,     0,   129,   130,     0,     0,
       0,     0,     0,   264,   265,   266,   267,   268,   269,   270,
       0,     0,     0,   346,     0,     0,   271,     0,   278,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   345,     0,     0,     0,     0,     0,
       0,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,     0,     0,     0,     0,   124,
       0,     0,   125,     0,     0,     0,     0,   126,     0,     0,
     127,     0,     0,     0,   128,     0,   129,   130,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,  -251,   394,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
       0,     0,     0,  -250,   394,     0,   263,     0,     0,     0,
       0,     0,     0,     0,   350,     0,     0,   127,     0,     0,
       0,   128,   263,     0,     0,     0,     0,  -251,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,
    -251,  -251,  -251,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,     0,
     507,   394,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -249,   394,   263,
       0,     0,     0,     0,     0,   264,   265,   266,   267,   268,
     269,   270,     0,     0,  -251,     0,   263,  -251,   271,     0,
     278,   264,   265,   266,   267,   268,   269,   270,     0,     0,
    -250,     0,     0,  -250,   271,     0,   278,  -249,  -249,  -249,
    -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,
    -249,  -249,  -249,     0,  -263,   394,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -248,   394,   263,     0,     0,     0,     0,   264,   265,
     266,   267,   268,   269,   270,     0,     0,   508,     0,     0,
     263,   271,     0,   278,     0,   264,   265,   266,   267,   268,
     269,   270,     0,     0,  -249,     0,     0,  -249,   271,     0,
     278,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,  -137,   262,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  -350,   262,   263,     0,     0,  -247,
     615,     0,   264,   265,   266,   267,   268,   269,   270,     0,
       0,     0,  -263,   263,     0,   271,     0,   278,     0,   264,
     265,   266,   267,   268,   269,   270,     0,     0,  -248,     0,
       0,  -248,   271,     0,   278,     0,     0,     0,     0,  -247,
    -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,     0,     0,    88,    89,     0,
      90,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   264,   265,   266,   267,   268,
     269,   270,     0,     0,  -137,     0,     0,     0,   271,     0,
    -137,     0,   264,   265,   266,   267,   268,   269,   270,     0,
       0,  -350,   572,     0,     0,   271,  -247,  -350,     0,  -247,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     507,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    88,    89,     0,    90,   124,     0,
       0,   125,     0,     0,     0,     0,   126,   239,     0,   127,
       0,     0,   240,   128,     0,   129,   130,     0,     0,     0,
     486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    88,    89,     0,    90,   124,     0,   508,   125,     0,
       0,     0,     0,   126,   239,     0,   127,     0,     0,   240,
     128,     0,   129,   130,   486,   487,   488,   489,   490,   491,
     492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
     502,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,  -244,    41,
       0,     0,   124,     0,     0,   125,     0,     0,     0,     0,
     126,  -362,     0,   127,  -241,    41,     0,   128,     0,   129,
     130,     0,     0,     0,     0,     0,     0,  -362,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -244,  -244,
    -244,  -244,  -244,  -244,  -244,  -244,  -244,  -244,  -244,  -244,
    -244,  -244,  -244,  -244,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
       0,     0,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   235,   377,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,  -177,  -244,  -177,  -177,  -177,  -177,
    -177,  -177,  -177,   -66,  -138,   -99,  -105,  -164,     0,   378,
    -166,  -241,     0,     0,     0,     0,     0,     0,     0,  -121,
    -121,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,   656,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -177,     0,  -177,  -177,  -177,  -177,  -177,  -177,  -177,   -66,
    -138,   -99,  -105,  -164,     0,   378,  -166,     0,     0,     0,
       0,     0,     0,     0,     0,  -121,  -121,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,     0,     0,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     350,     0,     0,   127,     0,     0,     0,   128,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-366))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       1,    55,     3,     4,     5,     6,     7,     8,     9,     1,
      40,    20,   133,   303,   229,   261,     0,    32,    33,   260,
     159,     0,    30,   156,   260,   156,   356,   287,   192,   368,
       0,   195,    41,     0,   159,   230,    83,   232,    39,     1,
     332,   263,   131,     5,     0,     1,     0,    48,    49,   254,
       5,    13,   274,     0,     5,    80,   278,    82,    62,   314,
      68,   415,   284,     1,    73,     1,   420,   421,   290,     5,
      21,     1,     0,    20,    75,     5,     0,    13,    14,    17,
       1,     1,     0,   110,   252,     5,   113,    88,    89,    90,
       5,    12,     5,    13,   199,   200,   201,   202,   203,   391,
      62,   306,    17,   468,     0,     1,   328,   111,     5,   110,
      13,     1,    12,     5,    17,     5,   110,    13,   408,   113,
     241,   122,   123,   124,   125,   126,   110,     0,   129,   130,
     110,   132,    62,   112,   135,   136,   137,    13,   139,   109,
     141,   230,    22,   110,   145,   146,   113,   407,   370,   111,
     442,   107,    88,   107,   110,   164,   112,   113,   167,     0,
       1,   108,   163,   418,   165,   166,     0,   168,   169,   170,
     117,   172,    13,    24,   221,   222,    97,   531,   114,   107,
     181,   111,   118,   184,   185,   400,   187,   188,   189,   198,
     108,   192,   193,   194,   195,   220,    13,   206,   199,   200,
     201,   202,   203,    25,   212,   373,   374,   375,   376,   217,
     315,   107,   213,   214,   215,   230,   476,   114,   548,   549,
     550,   118,   114,   113,     0,     1,   118,   348,   518,   583,
       0,   453,   586,     5,   235,   236,    23,    13,   239,   240,
     323,    12,   325,    13,   253,    86,   247,   248,   587,   588,
      13,   260,    26,     1,   619,   256,   257,   258,   259,   624,
     625,   456,   351,     1,    29,    13,   107,     5,   401,    21,
     401,     0,     1,   114,   108,    13,   285,   118,   112,    17,
     645,   282,     5,     6,    13,   650,   295,   626,   510,    21,
     529,   530,    13,   294,   584,   296,   343,   114,   299,    62,
       0,   118,   317,     0,     1,    86,     1,   672,   647,     0,
      58,    59,   677,   321,   315,    64,    13,    12,    13,    63,
     484,    38,    39,    13,    14,     0,    97,   381,   329,    58,
      59,   110,   108,    12,    13,   336,   345,   112,   108,   631,
     423,   706,   114,   115,   116,   117,   485,    13,   111,   350,
     589,    13,   367,   613,    20,    21,   357,   387,   412,   698,
     652,     0,     1,   111,     0,   113,   705,   456,   377,     1,
     371,    13,   461,     5,    13,   113,     0,   378,   112,   113,
      12,   520,   111,   516,   623,   516,   378,    63,    64,   100,
     511,   383,    13,   615,   519,   520,   611,   643,   688,     1,
     641,   616,   403,   404,   112,   641,    13,   697,   409,     0,
     411,   633,    12,    13,   674,   416,    13,    19,   419,    58,
      59,   113,   111,   683,   114,   426,   427,    60,   118,   562,
     431,   562,   433,     1,   435,   112,   437,     5,    13,   113,
     441,   456,   113,   113,    12,    13,   461,    13,     0,   450,
     117,   452,     5,    25,    23,     7,     8,     9,    10,    11,
     113,    17,    26,   464,    29,    60,   467,    13,   107,    88,
     471,   110,   111,   612,   113,    17,     0,     1,     1,   480,
     110,   482,   483,   484,    15,    16,    17,    18,    19,    17,
     482,   483,   484,   111,   260,    19,    98,    99,   100,   101,
     102,   103,   104,   636,   113,   636,     1,   113,   232,   111,
       5,   113,   504,   634,   515,   113,   620,   638,    13,   113,
     521,   644,   523,   641,   525,    20,    21,   300,   514,    97,
     178,   532,   562,   577,   641,   536,   544,   676,   641,    29,
      29,   542,    98,    99,   641,   101,   685,   646,   104,   105,
      33,   367,   553,   554,   687,   564,   687,   109,   559,   560,
      29,   700,   695,   641,   695,   677,    29,   119,   641,    29,
     641,   572,   317,   574,    98,    99,   100,   101,   102,   103,
     104,   196,   641,   107,    68,   544,   110,   111,   112,   113,
     207,     0,     1,   594,   318,    80,   320,   442,   322,   599,
     324,   602,   326,   331,    13,   416,   660,     0,     1,    43,
     634,   461,   461,   548,     0,     1,   456,   618,   641,   641,
     621,   629,    -1,    -1,   348,   349,    19,    13,   352,   630,
      -1,   646,   641,     0,     1,   359,   360,    -1,   668,     1,
      -1,    -1,    -1,     5,    -1,    -1,    -1,   656,   372,    58,
      59,    13,    19,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,   695,    -1,    -1,   392,    -1,
      -1,    -1,    58,    59,   675,    -1,    -1,    -1,   402,    -1,
     681,   682,     0,    -1,    -1,    -1,    -1,    -1,   689,    -1,
     691,    -1,    -1,    -1,    12,    13,    -1,    15,   699,    -1,
     424,   110,   111,   704,   113,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,   107,    -1,    -1,   110,   111,   112,
     113,   107,    -1,    -1,   110,   111,    -1,   113,    -1,     1,
      -1,    98,    99,   100,   101,   102,   103,   104,   462,    -1,
     107,    13,    -1,   110,   111,   469,   113,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    -1,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,   103,    58,    59,   106,    -1,
     108,    -1,    -1,   111,   112,    -1,   114,    -1,    -1,   117,
     118,    -1,   120,   121,    -1,    -1,    -1,    -1,    -1,    -1,
     534,   535,    -1,   537,   538,   539,   540,    -1,    -1,    -1,
      -1,     0,    27,   547,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    12,    13,    -1,    15,    -1,   110,   111,
      -1,   113,    22,    23,    24,    25,    26,    -1,    28,    29,
      -1,   575,    -1,    -1,     0,     1,    -1,    -1,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,   590,    -1,    -1,   593,
      -1,    -1,    13,    19,    15,    16,    17,    18,    19,    20,
      21,   605,    -1,    -1,    -1,    -1,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    -1,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,   103,    -1,    -1,   106,    -1,    -1,
     654,   655,   111,   112,    -1,   114,     0,     1,   117,   118,
      -1,   120,   121,    -1,    -1,    -1,    -1,    -1,    12,    13,
      -1,    15,    98,    99,   100,   101,   102,   103,   104,    -1,
       0,     1,   108,   109,    -1,   111,    13,   113,    15,    16,
      17,    18,    19,    20,    -1,    -1,     0,     1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    -1,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    -1,    -1,     1,    -1,   103,
      -1,    -1,   106,    -1,   108,    -1,    -1,   111,    12,    13,
     114,    15,    -1,    -1,   118,    -1,   120,   121,    98,    99,
     100,   101,   102,   103,   104,     0,     1,   107,    -1,    -1,
     110,   111,    -1,   113,    98,    99,   100,   101,   102,   103,
     104,    -1,    -1,    -1,    19,   109,    -1,   111,    -1,   113,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    -1,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    -1,    -1,     1,    -1,   103,
      -1,    -1,   106,    -1,    -1,    -1,    -1,   111,    12,    13,
     114,    15,    -1,    -1,   118,    -1,   120,   121,    -1,    -1,
      -1,    -1,    -1,    98,    99,   100,   101,   102,   103,   104,
      -1,    -1,    -1,   108,    -1,    -1,   111,    -1,   113,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    -1,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,   103,
      -1,    -1,   106,    -1,    -1,    -1,    -1,   111,    -1,    -1,
     114,    -1,    -1,    -1,   118,    -1,   120,   121,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,     0,     1,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      -1,    -1,    -1,     0,     1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   111,    -1,    -1,   114,    -1,    -1,
      -1,   118,    19,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
       0,     1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,    19,
      -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,   107,    -1,    19,   110,   111,    -1,
     113,    98,    99,   100,   101,   102,   103,   104,    -1,    -1,
     107,    -1,    -1,   110,   111,    -1,   113,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,     0,     1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    19,    -1,    -1,    -1,    -1,    98,    99,
     100,   101,   102,   103,   104,    -1,    -1,   107,    -1,    -1,
      19,   111,    -1,   113,    -1,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,   107,    -1,    -1,   110,   111,    -1,
     113,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,     1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     0,     1,    19,    -1,    -1,     0,
       1,    -1,    98,    99,   100,   101,   102,   103,   104,    -1,
      -1,    -1,   108,    19,    -1,   111,    -1,   113,    -1,    98,
      99,   100,   101,   102,   103,   104,    -1,    -1,   107,    -1,
      -1,   110,   111,    -1,   113,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    12,    13,    -1,
      15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,   107,    -1,    -1,    -1,   111,    -1,
     113,    -1,    98,    99,   100,   101,   102,   103,   104,    -1,
      -1,   107,    57,    -1,    -1,   111,   107,   113,    -1,   110,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
       0,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    12,    13,    -1,    15,   103,    -1,
      -1,   106,    -1,    -1,    -1,    -1,   111,   112,    -1,   114,
      -1,    -1,   117,   118,    -1,   120,   121,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    -1,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    12,    13,    -1,    15,   103,    -1,   107,   106,    -1,
      -1,    -1,    -1,   111,   112,    -1,   114,    -1,    -1,   117,
     118,    -1,   120,   121,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    -1,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,     0,     1,
      -1,    -1,   103,    -1,    -1,   106,    -1,    -1,    -1,    -1,
     111,    13,    -1,   114,     0,     1,    -1,   118,    -1,   120,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,     1,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    13,   107,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    -1,    28,
      29,   107,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,     1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      13,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     111,    -1,    -1,   114,    -1,    -1,    -1,   118,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    -1,    -1,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    27,    30,    31,    32,    33,    34,    35,    36,    37,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   139,
       1,     5,   132,   133,   246,   247,   133,   204,   133,   133,
     133,   133,   133,   133,   133,     0,   134,   257,     5,    12,
     252,     1,   226,   227,   228,   229,   257,   258,    28,   141,
     149,   150,   151,   152,   166,   168,   169,   171,   172,   180,
     181,   182,   190,   191,   197,   198,   199,   200,   205,   208,
      13,    15,    16,    17,    18,    19,    20,    21,   210,   211,
     212,   213,   215,   216,   217,   218,   220,   224,    12,    13,
      15,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,   103,   106,   111,   114,   118,   120,
     121,   203,   232,   233,   236,   237,   238,   244,    13,   186,
     187,   188,   202,   203,   187,    62,   111,   174,   175,   176,
     177,   178,   179,     5,     6,   140,   260,   133,   248,   252,
     257,     0,   228,   110,     1,    17,    13,     1,    58,    59,
     111,   156,   157,   159,   160,   161,   162,   163,   164,   133,
     133,    22,    12,    97,   154,    24,   162,    25,    38,    39,
      12,    13,   183,    23,    26,    29,   210,    21,    17,    98,
      99,   101,   104,   105,   257,    13,     1,   133,   225,    21,
       0,   207,     5,   114,   115,   116,   117,   201,    13,    20,
     215,   216,   216,   217,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,    84,   237,   133,     0,   112,
     117,   235,   236,   133,   133,   133,   133,   112,   113,   133,
      86,     0,   133,     1,   133,     0,    63,    64,    63,    64,
     142,   143,     1,    19,    98,    99,   100,   101,   102,   103,
     104,   111,   254,   255,   256,     0,   110,   138,   113,   138,
     253,   259,   110,   133,   257,   133,   133,   257,   133,   133,
       1,   133,   133,    13,   170,     1,   112,     0,   161,    13,
     153,     0,   133,   133,   133,   206,   133,   133,   133,   133,
     167,   133,   167,   167,   207,   100,   257,   133,   225,   133,
     225,   133,   225,   133,   225,   133,   225,    13,   257,    13,
      20,   212,   219,     0,   109,   137,   112,   213,   133,   133,
     133,    13,   213,   216,     0,     1,   108,   136,   233,   234,
     111,   203,   234,   237,   239,   242,   243,    13,   136,   231,
     234,   133,   133,   133,   133,   236,     0,   133,   133,   178,
     257,    13,   174,   133,   133,   133,   133,     1,    28,   141,
     150,   168,   172,   181,   190,   197,   199,   249,   250,   257,
     134,   259,   259,   259,     1,   253,   254,   255,   133,   259,
     257,   260,   259,    13,    60,   133,   158,   257,   133,   112,
     133,   154,   258,   113,   174,   113,    13,    17,   192,   223,
     113,   113,   206,   133,   225,    13,    13,    14,   202,   136,
     213,   214,   136,   213,   136,   214,   136,   214,   136,   259,
     133,    13,   219,     7,     8,     9,    10,    11,   119,   137,
     221,   133,   117,   257,   136,   136,   133,   237,   136,     5,
     132,   240,   241,   133,    13,   136,   136,   188,   226,   259,
     133,   136,   178,   178,   178,   178,   257,   247,   247,   162,
      25,   247,    23,    26,    29,   252,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,   196,   245,   137,   136,     0,   107,   135,
       1,   233,   136,   133,   133,    60,   260,   258,   133,   133,
     162,   165,   173,   165,   133,   223,    13,   206,   133,   165,
     165,   113,   214,   136,   133,   133,    88,   133,   133,   133,
     133,   137,   221,   133,   209,   133,   259,   234,   243,   114,
     118,   239,   242,    20,   117,   136,   133,   133,   135,   136,
      17,   133,   260,   110,     1,   247,   247,   247,   110,   253,
     247,   259,    57,   230,    17,   155,   156,   133,   258,   138,
     138,   253,   133,   113,   133,   133,   113,   204,   204,   165,
     133,   136,   136,   133,   112,   136,   136,   136,   136,   133,
      12,    13,   222,   211,   136,   241,   132,   132,   133,   133,
     133,   133,   252,   257,   133,     1,   133,   136,   157,   144,
     145,   165,   258,   165,   226,   226,   204,   136,   136,   133,
     222,   133,   136,     1,   233,   253,   260,   259,   233,   133,
     135,   142,   146,   148,   133,   184,   185,   204,   135,   135,
     226,   213,   133,   137,   259,   230,     1,   147,   150,   152,
     168,   169,   171,   182,   191,   198,   200,   208,   249,   250,
     251,   134,   144,   135,     1,   186,   189,   193,   226,   135,
     137,   136,   136,   257,   162,   252,   135,   260,   133,   253,
     135,   194,   196,   133,   133,   260,   258,   133,   133,   195,
     252,   258,   226,   133,   113,   133,   226,   135
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (parser, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, parser)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, parser); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, BisonCSSParser* parser)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    BisonCSSParser* parser;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (parser);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, BisonCSSParser* parser)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    BisonCSSParser* parser;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parser);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, BisonCSSParser* parser)
#else
static void
yy_reduce_print (yyvsp, yyrule, parser)
    YYSTYPE *yyvsp;
    int yyrule;
    BisonCSSParser* parser;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , parser);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parser); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, BisonCSSParser* parser)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parser)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    BisonCSSParser* parser;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parser);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (BisonCSSParser* parser);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (BisonCSSParser* parser)
#else
int
yyparse (parser)
    BisonCSSParser* parser;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:

/* Line 1806 of yacc.c  */
#line 386 "css/CSSGrammar.y"
    {
        parser->m_rule = (yyvsp[(3) - (5)].rule);
    }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 392 "css/CSSGrammar.y"
    {
        parser->m_keyframe = (yyvsp[(3) - (5)].keyframe);
    }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 398 "css/CSSGrammar.y"
    {
        parser->m_valueList = parser->sinkFloatingValueList((yyvsp[(3) - (4)].valueList));
    }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 404 "css/CSSGrammar.y"
    {
        /* can be empty */
    }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 410 "css/CSSGrammar.y"
    {
        parser->m_valueList = parser->sinkFloatingValueList((yyvsp[(3) - (4)].valueList));
        int oldParsedProperties = parser->m_parsedProperties.size();
        if (!parser->parseValue(parser->m_id, parser->m_important))
            parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
        parser->m_valueList = nullptr;
    }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 420 "css/CSSGrammar.y"
    {
        parser->m_mediaList = (yyvsp[(4) - (5)].mediaList);
    }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 426 "css/CSSGrammar.y"
    {
        if (parser->m_selectorListForParseSelector)
            parser->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(3) - (4)].selectorList));
    }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 433 "css/CSSGrammar.y"
    {
        parser->m_supportsCondition = (yyvsp[(3) - (4)].boolean);
    }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 476 "css/CSSGrammar.y"
    {
       if (parser->m_styleSheet)
           parser->m_styleSheet->parserSetEncodingFromCharsetRule((yyvsp[(3) - (5)].string));
       parser->startEndUnknownRule();
    }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 486 "css/CSSGrammar.y"
    {
     if ((yyvsp[(2) - (3)].rule) && parser->m_styleSheet)
         parser->m_styleSheet->parserAppendRule((yyvsp[(2) - (3)].rule));
 }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 505 "css/CSSGrammar.y"
    {
        parser->startRule();
    }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 511 "css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(2) - (2)].rule);
        parser->m_hadSyntacticallyValidCSSRule = true;
        parser->endRule(!!(yyval.rule));
    }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 516 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
        parser->endRule(false);
    }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 528 "css/CSSGrammar.y"
    { (yyval.ruleList) = 0; }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 529 "css/CSSGrammar.y"
    {
      (yyval.ruleList) = parser->appendRule((yyvsp[(1) - (3)].ruleList), (yyvsp[(2) - (3)].rule));
    }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 535 "css/CSSGrammar.y"
    {
        parser->endRule(false);
    }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 552 "css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(2) - (2)].rule);
        parser->endRule(!!(yyval.rule));
    }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 556 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
        parser->endRule(false);
    }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 563 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::IMPORT_RULE);
    }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 569 "css/CSSGrammar.y"
    {
        parser->endRuleHeader();
        parser->startRuleBody();
    }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 576 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createImportRule((yyvsp[(2) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 579 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 585 "css/CSSGrammar.y"
    {
        parser->addNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
        (yyval.rule) = 0;
    }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 592 "css/CSSGrammar.y"
    { (yyval.string).clear(); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 602 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 605 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (3)].valueList);
    }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 611 "css/CSSGrammar.y"
    {
        parser->tokenToLowerCase((yyvsp[(3) - (6)].string));
        (yyval.mediaQueryExp) = parser->createFloatingMediaQueryExp((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].valueList));
        if (!(yyval.mediaQueryExp))
            YYERROR;
    }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 617 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 623 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = parser->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(parser->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 627 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(parser->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 634 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = parser->createFloatingMediaQueryExpList();
    }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 637 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(4) - (5)].mediaQueryExpList);
    }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 643 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 646 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 649 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 655 "css/CSSGrammar.y"
    {
        (yyval.mediaQuery) = parser->createFloatingMediaQuery(parser->sinkFloatingMediaQueryExpList((yyvsp[(1) - (2)].mediaQueryExpList)));
    }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 658 "css/CSSGrammar.y"
    {
        parser->tokenToLowerCase((yyvsp[(2) - (3)].string));
        (yyval.mediaQuery) = parser->createFloatingMediaQuery((yyvsp[(1) - (3)].mediaQueryRestrictor), (yyvsp[(2) - (3)].string), parser->sinkFloatingMediaQueryExpList((yyvsp[(3) - (3)].mediaQueryExpList)));
    }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 666 "css/CSSGrammar.y"
    {
        parser->reportError(parser->lastLocationLabel(), InvalidMediaQueryCSSError);
        (yyval.mediaQuery) = parser->createFloatingNotAllQuery();
    }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 670 "css/CSSGrammar.y"
    {
        parser->reportError(parser->lastLocationLabel(), InvalidMediaQueryCSSError);
        (yyval.mediaQuery) = parser->createFloatingNotAllQuery();
    }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 677 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = parser->createMediaQuerySet();
    }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 684 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = parser->createMediaQuerySet();
        (yyval.mediaList)->addMediaQuery(parser->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 688 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (2)].mediaList);
        (yyval.mediaList)->addMediaQuery(parser->sinkFloatingMediaQuery((yyvsp[(2) - (2)].mediaQuery)));
    }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 692 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (1)].mediaList);
        (yyval.mediaList)->addMediaQuery(parser->sinkFloatingMediaQuery(parser->createFloatingNotAllQuery()));
    }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 699 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = parser->createMediaQuerySet();
        (yyval.mediaList)->addMediaQuery(parser->sinkFloatingMediaQuery((yyvsp[(1) - (4)].mediaQuery)));
    }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 703 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (5)].mediaList);
        (yyval.mediaList)->addMediaQuery(parser->sinkFloatingMediaQuery((yyvsp[(2) - (5)].mediaQuery)));
    }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 710 "css/CSSGrammar.y"
    {
        parser->startRuleBody();
    }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 716 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::MEDIA_RULE);
    }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 722 "css/CSSGrammar.y"
    {
        parser->endRuleHeader();
    }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 731 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createMediaRule((yyvsp[(2) - (8)].mediaList), (yyvsp[(7) - (8)].ruleList));
    }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 741 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createSupportsRule((yyvsp[(4) - (10)].boolean), (yyvsp[(9) - (10)].ruleList));
    }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 747 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::SUPPORTS_RULE);
        parser->markSupportsRuleHeaderStart();
    }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 754 "css/CSSGrammar.y"
    {
        parser->endRuleHeader();
        parser->markSupportsRuleHeaderEnd();
    }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 768 "css/CSSGrammar.y"
    {
        (yyval.boolean) = !(yyvsp[(3) - (3)].boolean);
    }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 774 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean) && (yyvsp[(4) - (4)].boolean);
    }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 777 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean) && (yyvsp[(4) - (4)].boolean);
    }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 783 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean) || (yyvsp[(4) - (4)].boolean);
    }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 786 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean) || (yyvsp[(4) - (4)].boolean);
    }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 792 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(3) - (5)].boolean);
    }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 796 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(3) - (6)].location), InvalidSupportsConditionCSSError);
        (yyval.boolean) = false;
    }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 803 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        CSSPropertyID id = cssPropertyID((yyvsp[(3) - (10)].string));
        if (id != CSSPropertyInvalid) {
            parser->m_valueList = parser->sinkFloatingValueList((yyvsp[(7) - (10)].valueList));
            int oldParsedProperties = parser->m_parsedProperties.size();
            (yyval.boolean) = parser->parseValue(id, (yyvsp[(8) - (10)].boolean));
            // We just need to know if the declaration is supported as it is written. Rollback any additions.
            if ((yyval.boolean))
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
        }
        parser->m_valueList = nullptr;
        parser->endProperty((yyvsp[(8) - (10)].boolean), false);
    }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 817 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        parser->endProperty(false, false, GeneralCSSError);
    }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 824 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::KEYFRAMES_RULE);
    }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 830 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 833 "css/CSSGrammar.y"
    {
        (yyval.boolean) = true;
    }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 839 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createKeyframesRule((yyvsp[(2) - (9)].string), parser->sinkFloatingKeyframeVector((yyvsp[(8) - (9)].keyframeRuleList)), (yyvsp[(1) - (9)].boolean) /* isPrefixed */);
    }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 851 "css/CSSGrammar.y"
    {
        parser->clearProperties();
    }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 856 "css/CSSGrammar.y"
    {
        (yyval.keyframeRuleList) = parser->createFloatingKeyframeVector();
        parser->resumeErrorLogging();
    }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 860 "css/CSSGrammar.y"
    {
        (yyval.keyframeRuleList) = (yyvsp[(1) - (4)].keyframeRuleList);
        (yyval.keyframeRuleList)->append((yyvsp[(2) - (4)].keyframe));
    }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 864 "css/CSSGrammar.y"
    {
        parser->clearProperties();
        parser->resumeErrorLogging();
    }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 871 "css/CSSGrammar.y"
    {
        (yyval.keyframe) = parser->createKeyframe((yyvsp[(1) - (5)].valueList));
    }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 877 "css/CSSGrammar.y"
    {
        (yyval.valueList) = parser->createFloatingValueList();
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(1) - (2)].value)));
    }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 881 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(4) - (5)].value)));
    }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 888 "css/CSSGrammar.y"
    {
        (yyval.value).setFromNumber((yyvsp[(1) - (2)].integer) * (yyvsp[(2) - (2)].number));
    }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 891 "css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).equalIgnoringCase("from"))
            (yyval.value).setFromNumber(0);
        else if ((yyvsp[(1) - (1)].string).equalIgnoringCase("to"))
            (yyval.value).setFromNumber(100);
        else {
            YYERROR;
        }
    }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 903 "css/CSSGrammar.y"
    {
        parser->reportError(parser->lastLocationLabel(), InvalidKeyframeSelectorCSSError);
    }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 909 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::PAGE_RULE);
    }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 916 "css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (10)].selector))
            (yyval.rule) = parser->createPageRule(parser->sinkFloatingSelector((yyvsp[(4) - (10)].selector)));
        else {
            // Clear properties in the invalid @page rule.
            parser->clearProperties();
            // Also clear margin at-rules here once we fully implement margin at-rules parsing.
            (yyval.rule) = 0;
        }
    }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 929 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelectorWithTagName(QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), parser->m_defaultNamespace));
        (yyval.selector)->setForPage();
    }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 933 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (3)].selector);
        (yyval.selector)->prependTagSelector(QualifiedName(nullAtom, (yyvsp[(1) - (3)].string), parser->m_defaultNamespace));
        (yyval.selector)->setForPage();
    }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 938 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
        (yyval.selector)->setForPage();
    }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 942 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setForPage();
    }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 954 "css/CSSGrammar.y"
    {
        parser->startDeclarationsForMarginBox();
    }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 956 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createMarginAtRule((yyvsp[(1) - (7)].marginBox));
    }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 962 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftCornerMarginBox;
    }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 965 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopLeftMarginBox;
    }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 968 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopCenterMarginBox;
    }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 971 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightMarginBox;
    }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 974 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::TopRightCornerMarginBox;
    }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 977 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftCornerMarginBox;
    }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 980 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomLeftMarginBox;
    }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 983 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomCenterMarginBox;
    }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 986 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightMarginBox;
    }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 989 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::BottomRightCornerMarginBox;
    }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 992 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftTopMarginBox;
    }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 995 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftMiddleMarginBox;
    }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 998 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::LeftBottomMarginBox;
    }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 1001 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightTopMarginBox;
    }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 1004 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightMiddleMarginBox;
    }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 1007 "css/CSSGrammar.y"
    {
        (yyval.marginBox) = CSSSelector::RightBottomMarginBox;
    }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 1013 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::FONT_FACE_RULE);
    }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 1020 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createFontFaceRule();
    }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 1026 "css/CSSGrammar.y"
    {
        parser->markViewportRuleBodyStart();
        parser->startRuleHeader(CSSRuleSourceData::VIEWPORT_RULE);
    }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 1034 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createViewportRule();
        parser->markViewportRuleBodyEnd();
    }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 1041 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 1042 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 1043 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 1044 "css/CSSGrammar.y"
    {
        if ((yyvsp[(2) - (4)].string).equalIgnoringCase("deep"))
            (yyval.relation) = CSSSelector::ShadowDeep;
        else
            YYERROR;
    }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 1054 "css/CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 1058 "css/CSSGrammar.y"
    { (yyval.integer) = -1; }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 1059 "css/CSSGrammar.y"
    { (yyval.integer) = 1; }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 1063 "css/CSSGrammar.y"
    {
        parser->startProperty();
    }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 1069 "css/CSSGrammar.y"
    {
        parser->startRuleHeader(CSSRuleSourceData::STYLE_RULE);
        parser->startSelector();
    }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 1076 "css/CSSGrammar.y"
    {
        parser->endRuleHeader();
    }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 1082 "css/CSSGrammar.y"
    {
        parser->endSelector();
    }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 1088 "css/CSSGrammar.y"
    {
        (yyval.rule) = parser->createStyleRule((yyvsp[(2) - (9)].selectorList));
    }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 1094 "css/CSSGrammar.y"
    {
        parser->startSelector();
    }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 1099 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = parser->reusableSelectorVector();
        (yyval.selectorList)->shrink(0);
        (yyval.selectorList)->append(parser->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
    }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 1104 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = (yyvsp[(1) - (6)].selectorList);
        (yyval.selectorList)->append(parser->sinkFloatingSelector((yyvsp[(6) - (6)].selector)));
    }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 1114 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        CSSParserSelector* end = (yyval.selector);
        while (end->tagHistory())
            end = end->tagHistory();
        end->setRelation(CSSSelector::Descendant);
        if ((yyvsp[(1) - (3)].selector)->isContentPseudoElement())
            end->setRelationIsAffectedByPseudoContent();
        end->setTagHistory(parser->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
    }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 1124 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        CSSParserSelector* end = (yyval.selector);
        while (end->tagHistory())
            end = end->tagHistory();
        end->setRelation((yyvsp[(2) - (3)].relation));
        if ((yyvsp[(1) - (3)].selector)->isContentPseudoElement())
            end->setRelationIsAffectedByPseudoContent();
        end->setTagHistory(parser->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
    }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 1137 "css/CSSGrammar.y"
    { (yyval.string).clear(); }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 1138 "css/CSSGrammar.y"
    { static const LChar star = '*'; (yyval.string).init(&star, 1); }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 1143 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelectorWithTagName(QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), parser->m_defaultNamespace));
    }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 1146 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->rewriteSpecifiersWithElementName(nullAtom, (yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].selector));
        if (!(yyval.selector))
            YYERROR;
    }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 1151 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->rewriteSpecifiersWithNamespaceIfNeeded((yyvsp[(1) - (1)].selector));
        if (!(yyval.selector))
            YYERROR;
    }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 1156 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelectorWithTagName(parser->determineNameInNamespace((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string)));
        if (!(yyval.selector))
            YYERROR;
    }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 1161 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->rewriteSpecifiersWithElementName((yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].selector));
        if (!(yyval.selector))
            YYERROR;
    }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 1166 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->rewriteSpecifiersWithElementName((yyvsp[(1) - (2)].string), starAtom, (yyvsp[(2) - (2)].selector));
        if (!(yyval.selector))
            YYERROR;
    }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 1174 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = parser->createFloatingSelectorVector();
        (yyval.selectorList)->append(parser->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
    }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 1178 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = (yyvsp[(1) - (5)].selectorList);
        (yyval.selectorList)->append(parser->sinkFloatingSelector((yyvsp[(5) - (5)].selector)));
    }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 1185 "css/CSSGrammar.y"
    {
        if (parser->m_context.isHTMLDocument())
            parser->tokenToLowerCase((yyvsp[(1) - (1)].string));
        (yyval.string) = (yyvsp[(1) - (1)].string);
    }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 1190 "css/CSSGrammar.y"
    {
        static const LChar star = '*';
        (yyval.string).init(&star, 1);
    }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1198 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->rewriteSpecifiers((yyvsp[(1) - (2)].selector), (yyvsp[(2) - (2)].selector));
    }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 1204 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::Id);
        if (isQuirksModeBehavior(parser->m_context.mode()))
            parser->tokenToLowerCase((yyvsp[(1) - (1)].string));
        (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
    }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 1211 "css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string)[0] >= '0' && (yyvsp[(1) - (1)].string)[0] <= '9') {
            YYERROR;
        } else {
            (yyval.selector) = parser->createFloatingSelector();
            (yyval.selector)->setMatch(CSSSelector::Id);
            if (isQuirksModeBehavior(parser->m_context.mode()))
                parser->tokenToLowerCase((yyvsp[(1) - (1)].string));
            (yyval.selector)->setValue((yyvsp[(1) - (1)].string));
        }
    }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 1228 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::Class);
        if (isQuirksModeBehavior(parser->m_context.mode()))
            parser->tokenToLowerCase((yyvsp[(2) - (2)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
    }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 1238 "css/CSSGrammar.y"
    {
        if (parser->m_context.isHTMLDocument())
            parser->tokenToLowerCase((yyvsp[(1) - (2)].string));
        (yyval.string) = (yyvsp[(1) - (2)].string);
    }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1246 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 1251 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->setMatch((CSSSelector::Match)(yyvsp[(4) - (8)].integer));
        (yyval.selector)->setValue((yyvsp[(6) - (8)].string));
    }
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 1257 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[(3) - (5)].string), (yyvsp[(4) - (5)].string)));
        (yyval.selector)->setMatch(CSSSelector::Set);
    }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1262 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setAttribute(parser->determineNameInNamespace((yyvsp[(3) - (9)].string), (yyvsp[(4) - (9)].string)));
        (yyval.selector)->setMatch((CSSSelector::Match)(yyvsp[(5) - (9)].integer));
        (yyval.selector)->setValue((yyvsp[(7) - (9)].string));
    }
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 1268 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1274 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1277 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1280 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1283 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    }
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 1286 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    }
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1289 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1300 "css/CSSGrammar.y"
    {
        if ((yyvsp[(2) - (2)].string).isFunction())
            YYERROR;
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PagePseudoClass);
        parser->tokenToLowerCase((yyvsp[(2) - (2)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (2)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            YYERROR;
    }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1313 "css/CSSGrammar.y"
    {
        if ((yyvsp[(3) - (3)].string).isFunction())
            YYERROR;
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        parser->tokenToLowerCase((yyvsp[(3) - (3)].string));
        (yyval.selector)->setValue((yyvsp[(3) - (3)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown) {
            parser->reportError((yyvsp[(2) - (3)].location), InvalidSelectorPseudoCSSError);
            YYERROR;
        }
    }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1326 "css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (4)].string).isFunction())
            YYERROR;
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoElement);
        parser->tokenToLowerCase((yyvsp[(4) - (4)].string));
        (yyval.selector)->setValue((yyvsp[(4) - (4)].string));
        // FIXME: This call is needed to force selector to compute the pseudoType early enough.
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown) {
            parser->reportError((yyvsp[(3) - (4)].location), InvalidSelectorPseudoCSSError);
            YYERROR;
        }
    }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1341 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoElement);
        (yyval.selector)->adoptSelectorVector(*parser->sinkFloatingSelectorVector((yyvsp[(5) - (7)].selectorList)));
        (yyval.selector)->setValue((yyvsp[(3) - (7)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type != CSSSelector::PseudoCue)
            YYERROR;
    }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1350 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1358 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->adoptSelectorVector(*parser->sinkFloatingSelectorVector((yyvsp[(4) - (6)].selectorList)));
        parser->tokenToLowerCase((yyvsp[(2) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type != CSSSelector::PseudoAny)
            YYERROR;
    }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1368 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1372 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            YYERROR;
    }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 1382 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument(AtomicString::number((yyvsp[(4) - (7)].integer) * (yyvsp[(5) - (7)].number)));
        (yyval.selector)->setValue((yyvsp[(2) - (7)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            YYERROR;
    }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1392 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        parser->tokenToLowerCase((yyvsp[(2) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            YYERROR;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (!isValidNthToken((yyvsp[(4) - (6)].string)))
                YYERROR;
        }
    }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1409 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1413 "css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector)->isSimple())
            YYERROR;
        else {
            (yyval.selector) = parser->createFloatingSelector();
            (yyval.selector)->setMatch(CSSSelector::PseudoClass);

            Vector<OwnPtr<CSSParserSelector> > selectorVector;
            selectorVector.append(parser->sinkFloatingSelector((yyvsp[(4) - (6)].selector)));
            (yyval.selector)->adoptSelectorVector(selectorVector);

            parser->tokenToLowerCase((yyvsp[(2) - (6)].string));
            (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        }
    }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 1428 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 1431 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->adoptSelectorVector(*parser->sinkFloatingSelectorVector((yyvsp[(4) - (6)].selectorList)));
        parser->tokenToLowerCase((yyvsp[(2) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type != CSSSelector::PseudoHost)
            YYERROR;
    }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 1441 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 1445 "css/CSSGrammar.y"
    {
        (yyval.selector) = parser->createFloatingSelector();
        (yyval.selector)->setMatch(CSSSelector::PseudoClass);
        (yyval.selector)->adoptSelectorVector(*parser->sinkFloatingSelectorVector((yyvsp[(4) - (6)].selectorList)));
        parser->tokenToLowerCase((yyvsp[(2) - (6)].string));
        (yyval.selector)->setValue((yyvsp[(2) - (6)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type != CSSSelector::PseudoHostContext)
            YYERROR;
    }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 1455 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 1464 "css/CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 1466 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean) || (yyvsp[(2) - (2)].boolean);
    }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 1473 "css/CSSGrammar.y"
    {
        parser->startProperty();
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 1477 "css/CSSGrammar.y"
    {
        parser->startProperty();
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean) || (yyvsp[(2) - (4)].boolean);
    }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 1484 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        bool isPropertyParsed = false;
        if ((yyvsp[(1) - (6)].id) != CSSPropertyInvalid) {
            parser->m_valueList = parser->sinkFloatingValueList((yyvsp[(5) - (6)].valueList));
            int oldParsedProperties = parser->m_parsedProperties.size();
            (yyval.boolean) = parser->parseValue((yyvsp[(1) - (6)].id), (yyvsp[(6) - (6)].boolean));
            if (!(yyval.boolean)) {
                parser->rollbackLastProperties(parser->m_parsedProperties.size() - oldParsedProperties);
                parser->reportError((yyvsp[(4) - (6)].location), InvalidPropertyValueCSSError);
            } else
                isPropertyParsed = true;
            parser->m_valueList = nullptr;
        }
        parser->endProperty((yyvsp[(6) - (6)].boolean), isPropertyParsed);
    }
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 1501 "css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        parser->reportError((yyvsp[(4) - (8)].location), InvalidPropertyValueCSSError);
        parser->endProperty(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 1508 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(4) - (6)].location), InvalidPropertyValueCSSError);
        parser->endProperty(false, false);
        (yyval.boolean) = false;
    }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 1514 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(3) - (4)].location), PropertyDeclarationCSSError);
        parser->endProperty(false, false, GeneralCSSError);
        (yyval.boolean) = false;
    }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 1520 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(2) - (3)].location), PropertyDeclarationCSSError);
        (yyval.boolean) = false;
    }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 1527 "css/CSSGrammar.y"
    {
        (yyval.id) = cssPropertyID((yyvsp[(2) - (3)].string));
        parser->setCurrentProperty((yyval.id));
        if ((yyval.id) == CSSPropertyInvalid)
            parser->reportError((yyvsp[(1) - (3)].location), InvalidPropertyCSSError);
    }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 1536 "css/CSSGrammar.y"
    { (yyval.boolean) = true; }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 1537 "css/CSSGrammar.y"
    { (yyval.boolean) = false; }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 1541 "css/CSSGrammar.y"
    {
        (yyval.valueList) = parser->createFloatingValueList();
        (yyval.valueList)->addValue(makeIdentValue((yyvsp[(1) - (2)].string)));
    }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 1545 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        (yyval.valueList)->addValue(makeIdentValue((yyvsp[(2) - (3)].string)));
    }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 1552 "css/CSSGrammar.y"
    {
        (yyval.value).setFromValueList(parser->sinkFloatingValueList(parser->createFloatingValueList()));
    }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 1555 "css/CSSGrammar.y"
    {
        (yyval.value).setFromValueList(parser->sinkFloatingValueList((yyvsp[(3) - (4)].valueList)));
    }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 1558 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 1564 "css/CSSGrammar.y"
    {
        (yyval.valueList) = parser->createFloatingValueList();
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 1568 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        (yyval.valueList)->addValue(makeOperatorValue((yyvsp[(2) - (3)].character)));
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(3) - (3)].value)));
    }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 1573 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (2)].valueList);
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(2) - (2)].value)));
    }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 1580 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(2) - (3)].location), PropertyDeclarationCSSError);
    }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 1586 "css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 1589 "css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 1596 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (3)].value); (yyval.value).fValue *= (yyvsp[(1) - (3)].integer); }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 1597 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 1598 "css/CSSGrammar.y"
    { (yyval.value) = makeIdentValue((yyvsp[(1) - (2)].string)); }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 1600 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 1601 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 1602 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_URI; }
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 1603 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; }
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 1604 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 1605 "css/CSSGrammar.y"
    { (yyval.value).id = CSSValueInvalid; (yyval.value).string = CSSParserString(); (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 1609 "css/CSSGrammar.y"
    { /* Handle width: %; */
      (yyval.value).id = CSSValueInvalid; (yyval.value).isInt = false; (yyval.value).unit = 0;
  }
    break;

  case 280:

/* Line 1806 of yacc.c  */
#line 1616 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number)); (yyval.value).isInt = true; }
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 1617 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number)); }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 1618 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_PERCENTAGE); }
    break;

  case 283:

/* Line 1806 of yacc.c  */
#line 1619 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_PX); }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 1620 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_CM); }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 1621 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_MM); }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 1622 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_IN); }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 1623 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_PT); }
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 1624 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_PC); }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 1625 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_DEG); }
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 1626 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_RAD); }
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 1627 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_GRAD); }
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 1628 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_TURN); }
    break;

  case 293:

/* Line 1806 of yacc.c  */
#line 1629 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_MS); }
    break;

  case 294:

/* Line 1806 of yacc.c  */
#line 1630 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_S); }
    break;

  case 295:

/* Line 1806 of yacc.c  */
#line 1631 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_HZ); }
    break;

  case 296:

/* Line 1806 of yacc.c  */
#line 1632 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_KHZ); }
    break;

  case 297:

/* Line 1806 of yacc.c  */
#line 1633 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_EMS); }
    break;

  case 298:

/* Line 1806 of yacc.c  */
#line 1634 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSParserValue::Q_EMS); }
    break;

  case 299:

/* Line 1806 of yacc.c  */
#line 1635 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_EXS); }
    break;

  case 300:

/* Line 1806 of yacc.c  */
#line 1636 "css/CSSGrammar.y"
    {
      (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_REMS);
      if (parser->m_styleSheet)
          parser->m_styleSheet->parserSetUsesRemUnits(true);
  }
    break;

  case 301:

/* Line 1806 of yacc.c  */
#line 1641 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_CHS); }
    break;

  case 302:

/* Line 1806 of yacc.c  */
#line 1642 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_VW); }
    break;

  case 303:

/* Line 1806 of yacc.c  */
#line 1643 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_VH); }
    break;

  case 304:

/* Line 1806 of yacc.c  */
#line 1644 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_VMIN); }
    break;

  case 305:

/* Line 1806 of yacc.c  */
#line 1645 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_VMAX); }
    break;

  case 306:

/* Line 1806 of yacc.c  */
#line 1646 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_DPPX); }
    break;

  case 307:

/* Line 1806 of yacc.c  */
#line 1647 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_DPI); }
    break;

  case 308:

/* Line 1806 of yacc.c  */
#line 1648 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_DPCM); }
    break;

  case 309:

/* Line 1806 of yacc.c  */
#line 1649 "css/CSSGrammar.y"
    { (yyval.value).setFromNumber((yyvsp[(1) - (1)].number), CSSPrimitiveValue::CSS_FR); }
    break;

  case 310:

/* Line 1806 of yacc.c  */
#line 1653 "css/CSSGrammar.y"
    {
        (yyval.value).setFromFunction(parser->createFloatingFunction((yyvsp[(1) - (4)].string), parser->sinkFloatingValueList((yyvsp[(3) - (4)].valueList))));
    }
    break;

  case 311:

/* Line 1806 of yacc.c  */
#line 1656 "css/CSSGrammar.y"
    {
        (yyval.value).setFromFunction(parser->createFloatingFunction((yyvsp[(1) - (3)].string), parser->sinkFloatingValueList(parser->createFloatingValueList())));
    }
    break;

  case 312:

/* Line 1806 of yacc.c  */
#line 1659 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 314:

/* Line 1806 of yacc.c  */
#line 1666 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); }
    break;

  case 315:

/* Line 1806 of yacc.c  */
#line 1670 "css/CSSGrammar.y"
    {
        (yyval.character) = '+';
    }
    break;

  case 316:

/* Line 1806 of yacc.c  */
#line 1673 "css/CSSGrammar.y"
    {
        (yyval.character) = '-';
    }
    break;

  case 317:

/* Line 1806 of yacc.c  */
#line 1676 "css/CSSGrammar.y"
    {
        (yyval.character) = '*';
    }
    break;

  case 318:

/* Line 1806 of yacc.c  */
#line 1679 "css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    }
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 1690 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (5)].valueList);
        (yyval.valueList)->insertValueAt(0, makeOperatorValue('('));
        (yyval.valueList)->addValue(makeOperatorValue(')'));
    }
    break;

  case 322:

/* Line 1806 of yacc.c  */
#line 1695 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 323:

/* Line 1806 of yacc.c  */
#line 1701 "css/CSSGrammar.y"
    {
        (yyval.valueList) = parser->createFloatingValueList();
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    }
    break;

  case 324:

/* Line 1806 of yacc.c  */
#line 1705 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        (yyval.valueList)->addValue(makeOperatorValue((yyvsp[(2) - (3)].character)));
        (yyval.valueList)->addValue(parser->sinkFloatingValue((yyvsp[(3) - (3)].value)));
    }
    break;

  case 325:

/* Line 1806 of yacc.c  */
#line 1710 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        (yyval.valueList)->addValue(makeOperatorValue((yyvsp[(2) - (3)].character)));
        (yyval.valueList)->stealValues(*((yyvsp[(3) - (3)].valueList)));
    }
    break;

  case 327:

/* Line 1806 of yacc.c  */
#line 1719 "css/CSSGrammar.y"
    {
        (yyval.value).setFromFunction(parser->createFloatingFunction((yyvsp[(1) - (5)].string), parser->sinkFloatingValueList((yyvsp[(3) - (5)].valueList))));
    }
    break;

  case 328:

/* Line 1806 of yacc.c  */
#line 1722 "css/CSSGrammar.y"
    {
        YYERROR;
    }
    break;

  case 332:

/* Line 1806 of yacc.c  */
#line 1738 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(2) - (3)].location), InvalidRuleCSSError);
    }
    break;

  case 338:

/* Line 1806 of yacc.c  */
#line 1752 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(4) - (5)].location), InvalidSupportsConditionCSSError);
        parser->popSupportsRuleData();
    }
    break;

  case 339:

/* Line 1806 of yacc.c  */
#line 1756 "css/CSSGrammar.y"
    {
        parser->markViewportRuleBodyEnd();
    }
    break;

  case 342:

/* Line 1806 of yacc.c  */
#line 1761 "css/CSSGrammar.y"
    {
        parser->resumeErrorLogging();
        parser->reportError((yyvsp[(1) - (3)].location), InvalidRuleCSSError);
    }
    break;

  case 343:

/* Line 1806 of yacc.c  */
#line 1768 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(2) - (5)].location), InvalidRuleCSSError);
    }
    break;

  case 347:

/* Line 1806 of yacc.c  */
#line 1777 "css/CSSGrammar.y"
    {
        parser->reportError((yyvsp[(2) - (4)].location), InvalidRuleCSSError);
    }
    break;

  case 350:

/* Line 1806 of yacc.c  */
#line 1785 "css/CSSGrammar.y"
    {
       parser->endInvalidRuleHeader();
   }
    break;

  case 351:

/* Line 1806 of yacc.c  */
#line 1791 "css/CSSGrammar.y"
    {
        parser->invalidBlockHit();
    }
    break;

  case 362:

/* Line 1806 of yacc.c  */
#line 1807 "css/CSSGrammar.y"
    {
        (yyval.location) = parser->currentLocation();
    }
    break;

  case 363:

/* Line 1806 of yacc.c  */
#line 1812 "css/CSSGrammar.y"
    {
        parser->setLocationLabel(parser->currentLocation());
    }
    break;



/* Line 1806 of yacc.c  */
#line 5051 "/home/whm/kt_work/Android/CM12_0-Android501/out/target/product/victara/obj/GYP/shared_intermediates/blink/core/CSSGrammar.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parser, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parser, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, parser);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, parser);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parser, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parser);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, parser);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1832 "css/CSSGrammar.y"


