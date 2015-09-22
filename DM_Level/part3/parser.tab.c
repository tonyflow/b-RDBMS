/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_ID = 258,
     TK_CONST_INT = 259,
     TK_CONST_REAL = 260,
     TK_SQSTRING = 261,
     TK_DQSTRING = 262,
     TK_OPER_EQUAL = 263,
     TK_OPER_GRE = 264,
     TK_OPER_LESS = 265,
     TK_OPER_DIF = 266,
     TK_OPER_GEQ = 267,
     TK_OPER_LEQ = 268,
     TK_OPER_LIKE = 269,
     TK_SEP_SEM = 270,
     TK_SEP_FSTOP = 271,
     TK_SEP_RPAR = 272,
     TK_SEP_LPAR = 273,
     TK_SEP_COMMA = 274,
     TK_matched = 275,
     TK_SELECT = 276,
     TK_CREATE = 277,
     TK_BUILDINDEX = 278,
     TK_DESTROY = 279,
     TK_QUIT = 280,
     TK_DELETE = 281,
     TK_INSERT = 282,
     TK_INTO = 283,
     TK_WHERE = 284,
     TK_AND = 285
   };
#endif
/* Tokens.  */
#define TK_ID 258
#define TK_CONST_INT 259
#define TK_CONST_REAL 260
#define TK_SQSTRING 261
#define TK_DQSTRING 262
#define TK_OPER_EQUAL 263
#define TK_OPER_GRE 264
#define TK_OPER_LESS 265
#define TK_OPER_DIF 266
#define TK_OPER_GEQ 267
#define TK_OPER_LEQ 268
#define TK_OPER_LIKE 269
#define TK_SEP_SEM 270
#define TK_SEP_FSTOP 271
#define TK_SEP_RPAR 272
#define TK_SEP_LPAR 273
#define TK_SEP_COMMA 274
#define TK_matched 275
#define TK_SELECT 276
#define TK_CREATE 277
#define TK_BUILDINDEX 278
#define TK_DESTROY 279
#define TK_QUIT 280
#define TK_DELETE 281
#define TK_INSERT 282
#define TK_INTO 283
#define TK_WHERE 284
#define TK_AND 285




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

	#include <stdio.h>
	#include <stdlib.h>	
	#include <string.h>

	
	typedef struct {
		int (*f)(int argc, char **argv);
		int attrNo;
		int argc;
		char** argv;
	} command;

	typedef struct{
		char *relname;
		char *attrname;
	} attribute;

	#define COM_CREATE 1
	#define COM_DELETE 2
	#define COM_DESTROY 3
	#define COM_QUIT 4
	#define COM_BUILD 5
	#define COM_SELECT 6
	#define COM_INSERT 7

	#define MEMSIZE 200

	#define RESTART -3

	#define ERROR_SYNAX_ERROR "Syntax Error\n"	
	#define MEMORY_ALLOCATION_ERROR "Fatal error while trying to allocate new memory. Please try compiling again your code in a while.\n"
	#define FOPEN_WRITE_ERROR "Fatal error while openning file %s for writing.\n"

	int stop = 0;

	extern int num_line;

	char** allocedMemory;

	int memSize=0;
	int memIndex=0;

	void insertMem(void *ptr);
	void freeMem();
	void replaceMem(void *ptr1, void* ptr2);


/* Functions */

void checkPtr(void *ptr);
void consumeTillQMark();
void printError(char* msg);

int UT_create(int argc,char* argv[]);
int UT_buildindex(int argc,char* argv[]);
int UT_destroy(int argc,char* argv[]);
int UT_quit(void);

int DM_select(int argc, char* argv[]);
int DM_join(int argc, char* argv[]);
int DM_delete(int argc, char* argv[]);
int DM_insert(int argc,char* argv[]);
int DM_selectivejoin(int argc, char* argv[]);


void yyerror (char *s)
{
  fprintf (stderr, "%s\n", s);
}

int yywrap()
{
	return 1;
}
	
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 70 "parser.y"
{
		char stringBuffer[150];
		char *string;
		char character;
		char *charPtr;
		command cmd;
		attribute attr;
	}
/* Line 187 of yacc.c.  */
#line 233 "parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 246 "parser.tab.c"

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
# if YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  26
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   74

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  43
/* YYNRULES -- Number of states.  */
#define YYNSTATES  89

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   285

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     9,    12,    14,    16,    18,    20,
      22,    24,    26,    32,    38,    42,    48,    51,    58,    61,
      67,    73,    77,    82,    85,    91,    95,    99,   101,   103,
     105,   109,   113,   117,   121,   123,   125,   127,   129,   131,
     133,   135,   137,   139
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      32,     0,    -1,    -1,    34,    15,    33,    32,    -1,    25,
      15,    -1,     1,    -1,    35,    -1,    37,    -1,    38,    -1,
      39,    -1,    40,    -1,    42,    -1,    22,     3,    18,    36,
      17,    -1,    36,    19,     3,     8,     6,    -1,     3,     8,
       6,    -1,    23,     3,    18,     3,    17,    -1,    24,     3,
      -1,    26,     3,    29,    48,    49,    50,    -1,    26,     3,
      -1,    27,     3,    18,    41,    17,    -1,    41,    19,     3,
       8,    50,    -1,     3,     8,    50,    -1,    21,    28,     3,
      43,    -1,    21,    43,    -1,    18,    44,    17,    29,    45,
      -1,    18,    44,    17,    -1,    44,    19,    48,    -1,    48,
      -1,    47,    -1,    46,    -1,    47,    30,    46,    -1,    48,
      49,    48,    -1,    48,    49,    50,    -1,     3,    16,     3,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,    12,
      -1,    13,    -1,     4,    -1,     5,    -1,     7,    -1,     6,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   143,   143,   142,   150,   154,   162,   168,   174,   180,
     186,   192,   201,   219,   230,   243,   260,   275,   292,   307,
     326,   337,   350,   379,   409,   420,   429,   438,   449,   455,
     461,   474,   491,   507,   516,   521,   526,   531,   536,   541,
     548,   553,   558,   563
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TK_ID", "TK_CONST_INT", "TK_CONST_REAL",
  "TK_SQSTRING", "TK_DQSTRING", "TK_OPER_EQUAL", "TK_OPER_GRE",
  "TK_OPER_LESS", "TK_OPER_DIF", "TK_OPER_GEQ", "TK_OPER_LEQ",
  "TK_OPER_LIKE", "TK_SEP_SEM", "TK_SEP_FSTOP", "TK_SEP_RPAR",
  "TK_SEP_LPAR", "TK_SEP_COMMA", "TK_matched", "TK_SELECT", "TK_CREATE",
  "TK_BUILDINDEX", "TK_DESTROY", "TK_QUIT", "TK_DELETE", "TK_INSERT",
  "TK_INTO", "TK_WHERE", "TK_AND", "$accept", "queryList", "@1", "query",
  "createCommand", "identifierList", "buildIndexCommand", "destroyCommand",
  "deleteCommand", "insertCommand", "attrList", "selectCommand",
  "restSelect", "targetList", "condition", "simpleJoinCondition",
  "simpleSelectCondition", "relAttr", "op", "constant", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    31,    33,    32,    32,    32,    34,    34,    34,    34,
      34,    34,    35,    36,    36,    37,    38,    39,    39,    40,
      41,    41,    42,    42,    43,    43,    44,    44,    45,    45,
      45,    46,    47,    48,    49,    49,    49,    49,    49,    49,
      50,    50,    50,    50
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     4,     2,     1,     1,     1,     1,     1,
       1,     1,     5,     5,     3,     5,     2,     6,     2,     5,
       5,     3,     4,     2,     5,     3,     3,     1,     1,     1,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     7,     8,     9,    10,    11,     0,     0,    23,
       0,     0,    16,     4,    18,     0,     1,     2,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    25,     0,
      22,     0,     0,     0,     0,     0,     0,     3,    33,     0,
      26,     0,    12,     0,    15,    34,    35,    36,    37,    38,
      39,     0,     0,    19,     0,    24,    29,    28,     0,    14,
       0,    40,    41,    43,    42,    17,    21,     0,     0,     0,
       0,     0,    30,     0,    31,    32,    13,    20,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     9,    36,    10,    11,    42,    12,    13,    14,    15,
      46,    16,    19,    29,    65,    66,    67,    84,    61,    75
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int8 yypact[] =
{
       2,   -65,   -16,     8,    18,    27,    16,    40,    42,    46,
      32,   -65,   -65,   -65,   -65,   -65,   -65,    45,    47,   -65,
      31,    33,   -65,   -65,    23,    35,   -65,   -65,    38,    21,
     -65,    37,    53,    54,    45,    55,     2,    56,    34,    45,
     -65,    52,    22,    48,    -3,    58,    25,   -65,   -65,    45,
     -65,    61,   -65,    59,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,     9,     9,   -65,    65,   -65,   -65,    39,    -3,   -65,
      62,   -65,   -65,   -65,   -65,   -65,   -65,    63,    45,    30,
      66,     9,   -65,    -3,   -65,   -65,   -65,   -65,    45
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,    28,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,    43,   -65,   -65,    -5,   -65,   -17,   -64,   -61
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      30,    76,    17,     1,    79,    55,    56,    57,    58,    59,
      60,    20,    18,    71,    72,    73,    74,    44,    85,    88,
      87,    21,    50,     2,     3,     4,     5,     6,     7,     8,
      22,    23,    68,    28,    71,    72,    73,    74,    38,    52,
      39,    53,    63,    24,    64,    25,    26,    27,    28,    32,
      31,    33,    34,    35,    37,    17,    41,    43,    45,    48,
      51,    83,    70,    49,    47,    54,    62,    69,    77,    78,
      80,    81,    86,    82,    40
};

static const yytype_uint8 yycheck[] =
{
      17,    62,    18,     1,    68,     8,     9,    10,    11,    12,
      13,     3,    28,     4,     5,     6,     7,    34,    79,    83,
      81,     3,    39,    21,    22,    23,    24,    25,    26,    27,
       3,    15,    49,     3,     4,     5,     6,     7,    17,    17,
      19,    19,    17,     3,    19,     3,     0,    15,     3,    18,
       3,    18,    29,    18,    16,    18,     3,     3,     3,     3,
       8,    78,     3,    29,    36,    17,     8,     6,     3,    30,
       8,     8,     6,    78,    31
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    21,    22,    23,    24,    25,    26,    27,    32,
      34,    35,    37,    38,    39,    40,    42,    18,    28,    43,
       3,     3,     3,    15,     3,     3,     0,    15,     3,    44,
      48,     3,    18,    18,    29,    18,    33,    16,    17,    19,
      43,     3,    36,     3,    48,     3,    41,    32,     3,    29,
      48,     8,    17,    19,    17,     8,     9,    10,    11,    12,
      13,    49,     8,    17,    19,    45,    46,    47,    48,     6,
       3,     4,     5,     6,     7,    50,    50,     3,    30,    49,
       8,     8,    46,    48,    48,    50,     6,    50,    49
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

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
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
        case 2:
#line 143 "parser.y"
    { 
		int i;
		
		(yyvsp[(1) - (2)].cmd).f((yyvsp[(1) - (2)].cmd).argc,(yyvsp[(1) - (2)].cmd).argv);
		
		freeMem(); 
	;}
    break;

  case 4:
#line 151 "parser.y"
    {
		UT_quit();
	;}
    break;

  case 5:
#line 154 "parser.y"
    {
		printError(ERROR_SYNAX_ERROR);
		consumeTillQMark();
		freeMem();
		return RESTART;
	;}
    break;

  case 6:
#line 163 "parser.y"
    {
		(yyval.cmd).f=UT_create;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 7:
#line 169 "parser.y"
    {
		(yyval.cmd).f=UT_buildindex;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 8:
#line 175 "parser.y"
    {
		(yyval.cmd).f=UT_destroy;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 9:
#line 181 "parser.y"
    { 
		(yyval.cmd).f=DM_delete;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 10:
#line 187 "parser.y"
    { 
		(yyval.cmd).f=DM_insert;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 11:
#line 193 "parser.y"
    {
		(yyval.cmd).f=(yyvsp[(1) - (1)].cmd).f;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(1) - (1)].cmd).argv;
	;}
    break;

  case 12:
#line 202 "parser.y"
    {	
			int i=0;
			(yyval.cmd).argc = (yyvsp[(4) - (5)].cmd).argc + 2;
			checkPtr((yyval.cmd).argv = malloc(((yyval.cmd).argc+1)*sizeof(char*)));
			insertMem((yyval.cmd).argv);
			
			checkPtr((yyval.cmd).argv[0]=strdup("create"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
			
			memcpy((yyval.cmd).argv+2,(yyvsp[(4) - (5)].cmd).argv,(yyvsp[(4) - (5)].cmd).argc*sizeof(char*));
			
			(yyval.cmd).argv[(yyval.cmd).argc]=NULL;
		;}
    break;

  case 13:
#line 220 "parser.y"
    {
			checkPtr((yyval.cmd).argv=realloc((yyvsp[(1) - (5)].cmd).argv,((yyvsp[(1) - (5)].cmd).argc+2)*sizeof(char*)));
			replaceMem((yyvsp[(1) - (5)].cmd).argv,(yyval.cmd).argv);

			(yyval.cmd).argc=(yyvsp[(1) - (5)].cmd).argc+2;
			checkPtr((yyval.cmd).argv[(yyval.cmd).argc-2]=strdup((yyvsp[(3) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[(yyval.cmd).argc-2]);
			checkPtr((yyval.cmd).argv[(yyval.cmd).argc-1]=strdup((yyvsp[(5) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[(yyval.cmd).argc-1]);
		;}
    break;

  case 14:
#line 231 "parser.y"
    {
			(yyval.cmd).argc = 2;
			checkPtr((yyval.cmd).argv = malloc(2*sizeof(char*)));
			insertMem((yyval.cmd).argv);

			checkPtr((yyval.cmd).argv[0] = strdup((yyvsp[(1) - (3)].stringBuffer)));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1] = strdup((yyvsp[(3) - (3)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
		;}
    break;

  case 15:
#line 244 "parser.y"
    {
			(yyval.cmd).argc=3;
			checkPtr((yyval.cmd).argv=malloc(4*sizeof(char*)));
			insertMem((yyval.cmd).argv);

			checkPtr((yyval.cmd).argv[0]=strdup("buildindex"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
			checkPtr((yyval.cmd).argv[2]=strdup((yyvsp[(4) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[2]);

			(yyval.cmd).argv[3]=NULL;
		;}
    break;

  case 16:
#line 261 "parser.y"
    {
			(yyval.cmd).argc=2;
			checkPtr((yyval.cmd).argv=malloc(3*sizeof(char*)));
			insertMem((yyval.cmd).argv);

			checkPtr((yyval.cmd).argv[0]=strdup("destroy"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (2)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);

			(yyval.cmd).argv[2]=NULL;
		;}
    break;

  case 17:
#line 276 "parser.y"
    {
			(yyval.cmd).argc=5;
			checkPtr((yyval.cmd).argv=malloc(6*sizeof(char*)));
			insertMem((yyval.cmd).argv);
			
			checkPtr((yyval.cmd).argv[0]=strdup("delete"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (6)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
			
			(yyval.cmd).argv[2]=(yyvsp[(4) - (6)].attr).attrname;
			(yyval.cmd).argv[3]=(yyvsp[(5) - (6)].string);
			(yyval.cmd).argv[4]=(yyvsp[(6) - (6)].string);
			
			(yyval.cmd).argv[5]=NULL;
		;}
    break;

  case 18:
#line 293 "parser.y"
    {
			(yyval.cmd).argc=2;
			checkPtr((yyval.cmd).argv=malloc(3*sizeof(char*)));
			insertMem((yyval.cmd).argv);
			
			checkPtr((yyval.cmd).argv[0]=strdup("delete"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (2)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);

			(yyval.cmd).argv[2]=NULL;
		;}
    break;

  case 19:
#line 308 "parser.y"
    {
			int i=0;
			
			(yyval.cmd).argc = (yyvsp[(4) - (5)].cmd).argc + 2;
			checkPtr((yyval.cmd).argv = malloc(((yyval.cmd).argc+1)*sizeof(char*)));
			insertMem((yyval.cmd).argv);
			
			checkPtr((yyval.cmd).argv[0]=strdup("insert"));
			insertMem((yyval.cmd).argv[0]);
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(2) - (5)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
			
			memcpy((yyval.cmd).argv+2,(yyvsp[(4) - (5)].cmd).argv,(yyvsp[(4) - (5)].cmd).argc*sizeof(char*));
			
			(yyval.cmd).argv[(yyval.cmd).argc]=NULL;
		;}
    break;

  case 20:
#line 327 "parser.y"
    {
		checkPtr((yyval.cmd).argv=realloc((yyvsp[(1) - (5)].cmd).argv,((yyvsp[(1) - (5)].cmd).argc+2)*sizeof(char*)));
		replaceMem((yyvsp[(1) - (5)].cmd).argv,(yyval.cmd).argv);

		(yyval.cmd).argc=(yyvsp[(1) - (5)].cmd).argc+2;
		checkPtr((yyval.cmd).argv[(yyval.cmd).argc-2]=strdup((yyvsp[(3) - (5)].stringBuffer)));
		insertMem((yyval.cmd).argv[(yyval.cmd).argc-2]);
		checkPtr((yyval.cmd).argv[(yyval.cmd).argc-1]=strdup((yyvsp[(5) - (5)].string)));
		insertMem((yyval.cmd).argv[(yyval.cmd).argc-1]);
	;}
    break;

  case 21:
#line 338 "parser.y"
    {
		(yyval.cmd).argc = 2;
		checkPtr((yyval.cmd).argv = malloc(2*sizeof(char*)));
		insertMem((yyval.cmd).argv);

		checkPtr((yyval.cmd).argv[0] = strdup((yyvsp[(1) - (3)].stringBuffer)));
		insertMem((yyval.cmd).argv[0]);
		checkPtr((yyval.cmd).argv[1] = strdup((yyvsp[(3) - (3)].string)));
		insertMem((yyval.cmd).argv[1]);
	;}
    break;

  case 22:
#line 351 "parser.y"
    {
			char buffer[100];
			(yyval.cmd).f=(yyvsp[(4) - (4)].cmd).f;
			
			(yyval.cmd).argc=(yyvsp[(4) - (4)].cmd).argc+3;
			
			checkPtr((yyval.cmd).argv=malloc(((yyval.cmd).argc+1)*sizeof(char*)));
			insertMem((yyval.cmd).argv);
			
			if((yyval.cmd).f==DM_select)
				checkPtr((yyval.cmd).argv[0]=strdup("select"));
			else if((yyval.cmd).f==DM_join)
				checkPtr((yyval.cmd).argv[0]=strdup("join"));
			else if((yyval.cmd).f==DM_selectivejoin)
				checkPtr((yyval.cmd).argv[0]=strdup("selectivejoin"));
			insertMem((yyval.cmd).argv[0]);
			
			checkPtr((yyval.cmd).argv[1]=strdup((yyvsp[(3) - (4)].stringBuffer)));
			insertMem((yyval.cmd).argv[1]);
			
			sprintf(buffer,"%d",(yyvsp[(4) - (4)].cmd).attrNo);
			checkPtr((yyval.cmd).argv[2]=strdup(buffer));
			insertMem((yyval.cmd).argv[2]);
			
			memcpy((yyval.cmd).argv+3,(yyvsp[(4) - (4)].cmd).argv,(yyvsp[(4) - (4)].cmd).argc*sizeof(char*));
			
			(yyval.cmd).argv[(yyval.cmd).argc]=NULL;
		;}
    break;

  case 23:
#line 380 "parser.y"
    {
			char buffer[100];
			(yyval.cmd).f=(yyvsp[(2) - (2)].cmd).f;
			
			(yyval.cmd).argc=(yyvsp[(2) - (2)].cmd).argc+2;
			
			checkPtr((yyval.cmd).argv=malloc(((yyval.cmd).argc+1)*sizeof(char*)));
			insertMem((yyval.cmd).argv);

			if((yyval.cmd).f==DM_select)
				checkPtr((yyval.cmd).argv[0]=strdup("select"));
			else if((yyval.cmd).f==DM_join)
				checkPtr((yyval.cmd).argv[0]=strdup("join"));
			else if((yyval.cmd).f==DM_selectivejoin)
				checkPtr((yyval.cmd).argv[0]=strdup("selectivejoin"));
			
			insertMem((yyval.cmd).argv[0]);
			
			sprintf(buffer,"%d",(yyvsp[(2) - (2)].cmd).attrNo);
			checkPtr((yyval.cmd).argv[1]=strdup(buffer));
			
			insertMem((yyval.cmd).argv[1]);			

			memcpy((yyval.cmd).argv+2,(yyvsp[(2) - (2)].cmd).argv,(yyvsp[(2) - (2)].cmd).argc*sizeof(char*));
			
			(yyval.cmd).argv[(yyval.cmd).argc]=NULL;
		;}
    break;

  case 24:
#line 410 "parser.y"
    {
		(yyval.cmd).attrNo=(yyvsp[(2) - (5)].cmd).argc/2;
		(yyval.cmd).f=(yyvsp[(5) - (5)].cmd).f;
		(yyval.cmd).argc=(yyvsp[(2) - (5)].cmd).argc + (yyvsp[(5) - (5)].cmd).argc;
		
		checkPtr((yyval.cmd).argv=realloc((yyvsp[(2) - (5)].cmd).argv,(yyval.cmd).argc*sizeof(char*)));
		replaceMem((yyvsp[(2) - (5)].cmd).argv,(yyval.cmd).argv);
		
		memcpy((yyval.cmd).argv+(yyvsp[(2) - (5)].cmd).argc,(yyvsp[(5) - (5)].cmd).argv,(yyvsp[(5) - (5)].cmd).argc*sizeof(char*));
	;}
    break;

  case 25:
#line 421 "parser.y"
    {
		(yyval.cmd).attrNo=(yyvsp[(2) - (3)].cmd).argc/2;
		(yyval.cmd).f=DM_select;
		(yyval.cmd).argc=(yyvsp[(2) - (3)].cmd).argc;
		(yyval.cmd).argv=(yyvsp[(2) - (3)].cmd).argv;
	;}
    break;

  case 26:
#line 430 "parser.y"
    {
		checkPtr((yyval.cmd).argv=realloc((yyvsp[(1) - (3)].cmd).argv,((yyvsp[(1) - (3)].cmd).argc+2)*sizeof(char*)));
		replaceMem((yyvsp[(1) - (3)].cmd).argv,(yyval.cmd).argv);

		(yyval.cmd).argc=(yyvsp[(1) - (3)].cmd).argc+2;
		(yyval.cmd).argv[(yyval.cmd).argc-2]=(yyvsp[(3) - (3)].attr).relname;
		(yyval.cmd).argv[(yyval.cmd).argc-1]=(yyvsp[(3) - (3)].attr).attrname;
	;}
    break;

  case 27:
#line 439 "parser.y"
    {
		(yyval.cmd).argc = 2;
		checkPtr((yyval.cmd).argv = malloc(2*sizeof(char*)));
		insertMem((yyval.cmd).argv);	

		(yyval.cmd).argv[0] = (yyvsp[(1) - (1)].attr).relname;
		(yyval.cmd).argv[1] = (yyvsp[(1) - (1)].attr).attrname;
	;}
    break;

  case 28:
#line 450 "parser.y"
    {
		(yyval.cmd).f=(yyvsp[(1) - (1)].cmd).f;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
	;}
    break;

  case 29:
#line 456 "parser.y"
    {
		(yyval.cmd).f=(yyvsp[(1) - (1)].cmd).f;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
		(yyval.cmd).argc=(yyvsp[(1) - (1)].cmd).argc;
	;}
    break;

  case 30:
#line 462 "parser.y"
    {
		(yyval.cmd).f=DM_selectivejoin;
		(yyval.cmd).argc=(yyvsp[(1) - (3)].cmd).argc+(yyvsp[(3) - (3)].cmd).argc;
		
		checkPtr((yyval.cmd).argv=realloc((yyvsp[(1) - (3)].cmd).argv,(yyval.cmd).argc*sizeof(char*)));

		memcpy((yyval.cmd).argv+(yyvsp[(1) - (3)].cmd).argc,(yyvsp[(3) - (3)].cmd).argv,(yyvsp[(3) - (3)].cmd).argc*sizeof(char*));

		replaceMem((yyvsp[(1) - (3)].cmd).argv,(yyval.cmd).argv);
	;}
    break;

  case 31:
#line 475 "parser.y"
    {
		(yyval.cmd).f=DM_join;
		(yyval.cmd).argc=5;
		checkPtr((yyval.cmd).argv=malloc(5*sizeof(char*)));
		insertMem((yyval.cmd).argv);	
		
		(yyval.cmd).argv[0]=(yyvsp[(1) - (3)].attr).relname;
		(yyval.cmd).argv[1]=(yyvsp[(1) - (3)].attr).attrname;
		
		(yyval.cmd).argv[2]=(yyvsp[(2) - (3)].string);
		
		(yyval.cmd).argv[3]=(yyvsp[(3) - (3)].attr).relname;
		(yyval.cmd).argv[4]=(yyvsp[(3) - (3)].attr).attrname;
	;}
    break;

  case 32:
#line 492 "parser.y"
    {
		(yyval.cmd).f=DM_select;
		(yyval.cmd).argc=4;
		checkPtr((yyval.cmd).argv=malloc(4*sizeof(char*)));
		insertMem((yyval.cmd).argv);	
		
		(yyval.cmd).argv[0]=(yyvsp[(1) - (3)].attr).relname;
		(yyval.cmd).argv[1]=(yyvsp[(1) - (3)].attr).attrname;
		
		(yyval.cmd).argv[2]=(yyvsp[(2) - (3)].string);
		
		(yyval.cmd).argv[3]=(yyvsp[(3) - (3)].string);
	;}
    break;

  case 33:
#line 508 "parser.y"
    {
		checkPtr((yyval.attr).relname=strdup((yyvsp[(1) - (3)].stringBuffer))); //Edw xreiazetai to relname?
		insertMem((yyval.attr).relname);	
		checkPtr((yyval.attr).attrname=strdup((yyvsp[(3) - (3)].stringBuffer)));
		insertMem((yyval.attr).attrname);			
	;}
    break;

  case 34:
#line 517 "parser.y"
    {
		checkPtr((yyval.string)=strdup("="));
		insertMem((yyval.string));	
	;}
    break;

  case 35:
#line 522 "parser.y"
    {
		checkPtr((yyval.string)=strdup(">"));
		insertMem((yyval.string));	
	;}
    break;

  case 36:
#line 527 "parser.y"
    {
		checkPtr((yyval.string)=strdup("<"));
		insertMem((yyval.string));	
	;}
    break;

  case 37:
#line 532 "parser.y"
    {
		checkPtr((yyval.string)=strdup("!="));
		insertMem((yyval.string));	
	;}
    break;

  case 38:
#line 537 "parser.y"
    {
		checkPtr((yyval.string)=strdup(">="));
		insertMem((yyval.string));	
	;}
    break;

  case 39:
#line 542 "parser.y"
    {
		checkPtr((yyval.string)=strdup("<="));
		insertMem((yyval.string));	
	;}
    break;

  case 40:
#line 549 "parser.y"
    {
		checkPtr((yyval.string)=strdup((yyvsp[(1) - (1)].stringBuffer)));
		insertMem((yyval.string));	
	;}
    break;

  case 41:
#line 554 "parser.y"
    {
		checkPtr((yyval.string)=strdup((yyvsp[(1) - (1)].stringBuffer)));	
		insertMem((yyval.string));	
	;}
    break;

  case 42:
#line 559 "parser.y"
    {
		checkPtr((yyval.string)=strdup((yyvsp[(1) - (1)].stringBuffer)));
		insertMem((yyval.string));		
	;}
    break;

  case 43:
#line 564 "parser.y"
    {
		checkPtr((yyval.string)=strdup((yyvsp[(1) - (1)].stringBuffer)));
		insertMem((yyval.string));		
	;}
    break;


/* Line 1267 of yacc.c.  */
#line 2025 "parser.tab.c"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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
      if (yyn != YYPACT_NINF)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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


#line 570 "parser.y"


void insertMem(void *ptr){
	if(memSize==0){
		checkPtr(allocedMemory=malloc(MEMSIZE*sizeof(char*)));
		memSize=MEMSIZE;
	}
	else if(memIndex==memSize){
		checkPtr(allocedMemory=realloc(allocedMemory,(memSize+MEMSIZE)*sizeof(char*)));
		memSize+=MEMSIZE;
	}
	
	allocedMemory[memIndex]=ptr;
	
	++memIndex;	
}

void freeMem(){
	int i;

	for(i=0;i<memIndex;++i)
		free(allocedMemory[i]);

	if(memSize > 0)
		free(allocedMemory);

	memSize=0;
	memIndex=0;	
}

void replaceMem(void *ptr1, void* ptr2){
	int i=0;

	while(allocedMemory[i]!=ptr1)
		++i;

	allocedMemory[i]=ptr2;
}

void consumeTillQMark(){
	int token;

	do{
		token=yylex();
	}while(token!=TK_SEP_SEM && token>=0);
}


void printError(char* msg) {
	fprintf(stderr, "Error in line %d: %s\n", num_line, msg);
}

void checkPtr(void *ptr){
	if(ptr==NULL){
		fprintf(stderr,MEMORY_ALLOCATION_ERROR);
		exit(1);
	}
} 
