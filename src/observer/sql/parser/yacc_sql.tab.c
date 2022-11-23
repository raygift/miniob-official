/* A Bison parser, made by GNU Bison 3.7.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
  size_t select_length;
  size_t condition_length;
  size_t from_length;
  size_t value_length;
  size_t array_length;// 用于记录insert 多行数据时的数据行数
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
	char id[MAX_NUM];
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  context->ssql->sstr.insertion.value_num = 0;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 129 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_SELECT = 9,                     /* SELECT  */
  YYSYMBOL_DESC = 10,                      /* DESC  */
  YYSYMBOL_SHOW = 11,                      /* SHOW  */
  YYSYMBOL_SYNC = 12,                      /* SYNC  */
  YYSYMBOL_INSERT = 13,                    /* INSERT  */
  YYSYMBOL_DELETE = 14,                    /* DELETE  */
  YYSYMBOL_UPDATE = 15,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 16,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 17,                    /* RBRACE  */
  YYSYMBOL_COMMA = 18,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 19,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 20,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 21,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 22,                     /* INT_T  */
  YYSYMBOL_STRING_T = 23,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 24,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 25,                    /* DATE_T  */
  YYSYMBOL_UNIQUE = 26,                    /* UNIQUE  */
  YYSYMBOL_HELP = 27,                      /* HELP  */
  YYSYMBOL_EXIT = 28,                      /* EXIT  */
  YYSYMBOL_DOT = 29,                       /* DOT  */
  YYSYMBOL_INTO = 30,                      /* INTO  */
  YYSYMBOL_VALUES = 31,                    /* VALUES  */
  YYSYMBOL_FROM = 32,                      /* FROM  */
  YYSYMBOL_WHERE = 33,                     /* WHERE  */
  YYSYMBOL_AND = 34,                       /* AND  */
  YYSYMBOL_SET = 35,                       /* SET  */
  YYSYMBOL_ON = 36,                        /* ON  */
  YYSYMBOL_LOAD = 37,                      /* LOAD  */
  YYSYMBOL_DATA = 38,                      /* DATA  */
  YYSYMBOL_INFILE = 39,                    /* INFILE  */
  YYSYMBOL_MAX_SYM = 40,                   /* MAX_SYM  */
  YYSYMBOL_MIN_SYM = 41,                   /* MIN_SYM  */
  YYSYMBOL_AVG_SYM = 42,                   /* AVG_SYM  */
  YYSYMBOL_SUM_SYM = 43,                   /* SUM_SYM  */
  YYSYMBOL_COUNT_SYM = 44,                 /* COUNT_SYM  */
  YYSYMBOL_LIKE_SYM = 45,                  /* LIKE_SYM  */
  YYSYMBOL_NOT = 46,                       /* NOT  */
  YYSYMBOL_EQ = 47,                        /* EQ  */
  YYSYMBOL_LT = 48,                        /* LT  */
  YYSYMBOL_GT = 49,                        /* GT  */
  YYSYMBOL_LE = 50,                        /* LE  */
  YYSYMBOL_GE = 51,                        /* GE  */
  YYSYMBOL_NE = 52,                        /* NE  */
  YYSYMBOL_NUMBER = 53,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 54,                     /* FLOAT  */
  YYSYMBOL_DATE_STR = 55,                  /* DATE_STR  */
  YYSYMBOL_ID = 56,                        /* ID  */
  YYSYMBOL_PATH = 57,                      /* PATH  */
  YYSYMBOL_SSS = 58,                       /* SSS  */
  YYSYMBOL_STAR = 59,                      /* STAR  */
  YYSYMBOL_STRING_V = 60,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 61,                  /* $accept  */
  YYSYMBOL_commands = 62,                  /* commands  */
  YYSYMBOL_command = 63,                   /* command  */
  YYSYMBOL_exit = 64,                      /* exit  */
  YYSYMBOL_help = 65,                      /* help  */
  YYSYMBOL_sync = 66,                      /* sync  */
  YYSYMBOL_begin = 67,                     /* begin  */
  YYSYMBOL_commit = 68,                    /* commit  */
  YYSYMBOL_rollback = 69,                  /* rollback  */
  YYSYMBOL_drop_table = 70,                /* drop_table  */
  YYSYMBOL_show_tables = 71,               /* show_tables  */
  YYSYMBOL_show_index = 72,                /* show_index  */
  YYSYMBOL_desc_table = 73,                /* desc_table  */
  YYSYMBOL_create_index = 74,              /* create_index  */
  YYSYMBOL_index_list = 75,                /* index_list  */
  YYSYMBOL_drop_index = 76,                /* drop_index  */
  YYSYMBOL_create_table = 77,              /* create_table  */
  YYSYMBOL_attr_def_list = 78,             /* attr_def_list  */
  YYSYMBOL_attr_def = 79,                  /* attr_def  */
  YYSYMBOL_number = 80,                    /* number  */
  YYSYMBOL_type = 81,                      /* type  */
  YYSYMBOL_ID_get = 82,                    /* ID_get  */
  YYSYMBOL_insert = 83,                    /* insert  */
  YYSYMBOL_value_item = 84,                /* value_item  */
  YYSYMBOL_value_array = 85,               /* value_array  */
  YYSYMBOL_value_list = 86,                /* value_list  */
  YYSYMBOL_value = 87,                     /* value  */
  YYSYMBOL_delete = 88,                    /* delete  */
  YYSYMBOL_update = 89,                    /* update  */
  YYSYMBOL_select = 90,                    /* select  */
  YYSYMBOL_select_attr = 91,               /* select_attr  */
  YYSYMBOL_aggregation_list = 92,          /* aggregation_list  */
  YYSYMBOL_attr_list = 93,                 /* attr_list  */
  YYSYMBOL_rel_list = 94,                  /* rel_list  */
  YYSYMBOL_where = 95,                     /* where  */
  YYSYMBOL_condition_list = 96,            /* condition_list  */
  YYSYMBOL_condition = 97,                 /* condition  */
  YYSYMBOL_comOp = 98,                     /* comOp  */
  YYSYMBOL_load_data = 99                  /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   299

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  39
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  297

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   315


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   143,   143,   145,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   170,   175,   180,   186,   192,   198,   204,   210,
     216,   223,   230,   235,   240,   250,   261,   263,   272,   279,
     288,   290,   294,   305,   318,   321,   322,   323,   324,   327,
     336,   356,   363,   365,   369,   371,   375,   378,   381,   388,
     395,   405,   415,   434,   439,   444,   449,   454,   460,   466,
     472,   478,   484,   490,   496,   502,   508,   514,   521,   523,
     529,   535,   541,   547,   553,   559,   565,   571,   577,   583,
     590,   592,   599,   608,   610,   614,   616,   620,   622,   627,
     648,   668,   688,   710,   731,   752,   773,   774,   775,   776,
     777,   778,   779,   780,   784
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC",
  "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T",
  "UNIQUE", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE",
  "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "MAX_SYM", "MIN_SYM",
  "AVG_SYM", "SUM_SYM", "COUNT_SYM", "LIKE_SYM", "NOT", "EQ", "LT", "GT",
  "LE", "GE", "NE", "NUMBER", "FLOAT", "DATE_STR", "ID", "PATH", "SSS",
  "STAR", "STRING_V", "$accept", "commands", "command", "exit", "help",
  "sync", "begin", "commit", "rollback", "drop_table", "show_tables",
  "show_index", "desc_table", "create_index", "index_list", "drop_index",
  "create_table", "attr_def_list", "attr_def", "number", "type", "ID_get",
  "insert", "value_item", "value_array", "value_list", "value", "delete",
  "update", "select", "select_attr", "aggregation_list", "attr_list",
  "rel_list", "where", "condition_list", "condition", "comOp", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315
};
#endif

#define YYPACT_NINF (-249)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -249,    14,  -249,    30,    85,    44,   -39,    68,    94,    -8,
       7,    -9,    95,    96,    98,    99,   114,    84,  -249,  -249,
    -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,
    -249,  -249,  -249,  -249,  -249,  -249,  -249,    72,    74,   123,
      76,    81,   130,   132,   134,   136,   141,     3,   140,   133,
     163,   164,   137,  -249,   112,   115,   135,  -249,  -249,  -249,
    -249,  -249,   139,   156,   143,   124,   178,   179,   127,   128,
     129,   131,   -44,   138,   142,  -249,    44,   144,  -249,  -249,
     145,   155,   157,   146,   147,   148,   150,   152,  -249,  -249,
      23,    26,    28,    29,    60,   172,    19,   173,  -249,   174,
     190,   180,    65,   192,   160,   167,  -249,   181,    46,   187,
     153,   193,   154,   193,   158,   193,   159,   193,   161,   193,
     162,   193,   165,  -249,  -249,   166,   157,  -249,   101,   194,
    -249,  -249,  -249,    15,  -249,    64,   182,  -249,   101,   202,
     148,   196,  -249,  -249,  -249,  -249,   203,   168,   204,   120,
    -249,   206,  -249,   208,  -249,   209,  -249,   210,  -249,   211,
    -249,   173,   174,   226,   212,   180,   228,   176,  -249,   188,
    -249,  -249,  -249,  -249,  -249,  -249,    71,    80,    65,  -249,
     157,   183,   181,   231,   184,    13,   185,   219,   220,   222,
     224,   227,   193,   193,   193,   193,   193,  -249,  -249,  -249,
     101,   225,  -249,  -249,    64,  -249,  -249,  -249,   215,  -249,
     182,   242,   243,  -249,  -249,  -249,   230,   245,   195,    36,
     197,   198,   199,   200,   -43,  -249,  -249,  -249,  -249,  -249,
     212,  -249,    86,   201,  -249,  -249,  -249,  -249,  -249,   232,
     246,   205,    61,    75,    77,    78,    79,   235,  -249,   229,
    -249,  -249,   207,   247,  -249,   232,   193,   213,   193,   214,
     193,   216,   193,   217,   193,   218,   193,   221,   232,   256,
     248,  -249,   249,  -249,   250,  -249,   251,  -249,   254,  -249,
     258,  -249,  -249,  -249,  -249,   257,   193,   193,   193,   193,
     193,  -249,  -249,  -249,  -249,  -249,  -249
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    14,    11,    12,
      13,     8,     5,     7,     6,     4,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,    63,     0,
       0,     0,     0,    24,     0,     0,     0,    25,    26,    27,
      23,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,     0,     0,    31,    29,
       0,     0,    95,     0,     0,     0,     0,     0,    28,    38,
       0,     0,     0,     0,     0,     0,    90,    90,    66,    93,
       0,     0,     0,     0,     0,     0,    49,    40,     0,     0,
       0,    78,     0,    78,     0,    78,     0,    78,     0,    78,
       0,    78,     0,    91,    65,     0,    95,    30,     0,    52,
      56,    57,    58,     0,    59,     0,    97,    60,     0,     0,
       0,     0,    45,    46,    47,    48,    43,     0,     0,     0,
      67,     0,    69,     0,    71,     0,    73,     0,    75,     0,
      77,    90,    93,     0,    54,     0,     0,     0,   112,     0,
     106,   107,   108,   109,   110,   111,     0,     0,     0,    96,
      95,     0,    40,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    78,    78,    78,    78,    78,    92,    94,    62,
       0,     0,    53,    50,     0,   113,   101,    99,   102,   100,
      97,     0,     0,    41,    39,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    68,    70,    72,    74,    76,
      54,    51,     0,     0,    98,    61,   114,    42,    32,    36,
       0,     0,     0,     0,     0,     0,     0,     0,    55,     0,
     103,   104,     0,     0,    33,    36,    78,     0,    78,     0,
      78,     0,    78,     0,    78,     0,    78,     0,    36,     0,
       0,    79,     0,    81,     0,    83,     0,    85,     0,    87,
       0,    89,   105,    37,    34,     0,    78,    78,    78,    78,
      78,    35,    80,    82,    84,    86,    88
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,  -249,
    -249,  -249,  -249,  -249,  -248,  -249,  -249,    97,   122,  -249,
    -249,  -249,  -249,   111,  -249,    48,  -127,  -249,  -249,  -249,
     223,  -113,   -87,   118,  -121,    73,   103,  -132,  -249
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,   253,    30,    31,   141,   107,   216,
     146,   108,    32,   129,   166,   201,   135,    33,    34,    35,
      49,   150,    75,   126,   103,   179,   136,   176,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     152,   164,   154,   177,   156,   163,   158,   270,   160,   123,
     124,   180,    94,   246,     2,    95,   247,    50,     3,     4,
     283,    73,    54,     5,     6,     7,     8,     9,    10,    11,
     217,   218,    74,    12,    13,    14,    37,    73,    38,    55,
     111,    15,    16,   113,   167,   115,   117,    56,   122,   207,
     209,    17,   112,   240,   241,   114,    39,   116,   118,   211,
     168,   169,   170,   171,   172,   173,   174,   175,   142,   143,
     144,   145,   232,   230,   197,    51,    52,   119,   256,   225,
     226,   227,   228,   229,    42,    43,    44,    45,    46,   120,
     257,    40,   258,    41,   260,   262,   264,    53,    57,    58,
      47,    59,    60,    48,   259,   250,   261,   263,   265,   168,
     169,   170,   171,   172,   173,   174,   175,    61,   130,   131,
     132,   133,    62,   134,   130,   131,   132,   206,    63,   134,
      64,    65,    66,   130,   131,   132,   208,    67,   134,   130,
     131,   132,   249,   271,   134,   273,    68,   275,    69,   277,
      70,   279,    71,   281,   130,   131,   132,    72,    76,   134,
     187,   188,   189,   190,   191,    77,    78,    79,    81,    80,
      83,    82,    85,   292,   293,   294,   295,   296,    84,    86,
      87,    88,    89,    90,    91,    92,   101,    93,   110,   121,
     102,    73,   125,   127,    96,   137,   128,   139,    97,   140,
      99,   100,   104,   147,   106,   105,   109,   138,   181,   148,
     151,   149,   165,   183,   153,   155,   178,   157,   159,   184,
     186,   161,   162,   192,   185,   193,   194,   195,   196,   199,
     200,   203,   204,   205,   214,   220,   221,   215,   222,   212,
     223,   219,   231,   224,   233,   235,   236,   237,   238,   254,
     252,   239,   266,   242,   243,   244,   245,   251,   267,   284,
     291,   255,   182,   268,   269,   285,   286,   287,   288,   272,
     274,   289,   276,   278,   280,   290,   202,   282,   248,   213,
     198,   210,     0,   234,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98
};

static const yytype_int16 yycheck[] =
{
     113,   128,   115,   135,   117,   126,   119,   255,   121,    96,
      97,   138,    56,    56,     0,    59,    59,    56,     4,     5,
     268,    18,    30,     9,    10,    11,    12,    13,    14,    15,
      17,    18,    29,    19,    20,    21,     6,    18,     8,    32,
      17,    27,    28,    17,    29,    17,    17,    56,    29,   176,
     177,    37,    29,    17,    18,    29,    26,    29,    29,   180,
      45,    46,    47,    48,    49,    50,    51,    52,    22,    23,
      24,    25,   204,   200,   161,     7,     8,    17,    17,   192,
     193,   194,   195,   196,    40,    41,    42,    43,    44,    29,
      29,     6,    17,     8,    17,    17,    17,     3,     3,     3,
      56,     3,     3,    59,    29,   232,    29,    29,    29,    45,
      46,    47,    48,    49,    50,    51,    52,     3,    53,    54,
      55,    56,    38,    58,    53,    54,    55,    56,    56,    58,
      56,     8,    56,    53,    54,    55,    56,    56,    58,    53,
      54,    55,    56,   256,    58,   258,    16,   260,    16,   262,
      16,   264,    16,   266,    53,    54,    55,    16,    18,    58,
      40,    41,    42,    43,    44,    32,     3,     3,    56,    32,
      35,    56,    16,   286,   287,   288,   289,   290,    39,    36,
      56,     3,     3,    56,    56,    56,    31,    56,    36,    17,
      33,    18,    18,     3,    56,     3,    16,    30,    56,    18,
      56,    56,    56,    16,    56,    58,    56,    47,     6,    56,
      56,    18,    18,    17,    56,    56,    34,    56,    56,    16,
      16,    56,    56,    17,    56,    17,    17,    17,    17,     3,
      18,     3,    56,    45,     3,    16,    16,    53,    16,    56,
      16,    56,    17,    16,    29,     3,     3,    17,     3,     3,
      18,    56,    17,    56,    56,    56,    56,    56,    29,     3,
       3,    56,   140,    56,    17,    17,    17,    17,    17,    56,
      56,    17,    56,    56,    56,    17,   165,    56,   230,   182,
     162,   178,    -1,   210,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    62,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    27,    28,    37,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      76,    77,    83,    88,    89,    90,    99,     6,     8,    26,
       6,     8,    40,    41,    42,    43,    44,    56,    59,    91,
      56,     7,     8,     3,    30,    32,    56,     3,     3,     3,
       3,     3,    38,    56,    56,     8,    56,    56,    16,    16,
      16,    16,    16,    18,    29,    93,    18,    32,     3,     3,
      32,    56,    56,    35,    39,    16,    36,    56,     3,     3,
      56,    56,    56,    56,    56,    59,    56,    56,    91,    56,
      56,    31,    33,    95,    56,    58,    56,    79,    82,    56,
      36,    17,    29,    17,    29,    17,    29,    17,    29,    17,
      29,    17,    29,    93,    93,    18,    94,     3,    16,    84,
      53,    54,    55,    56,    58,    87,    97,     3,    47,    30,
      18,    78,    22,    23,    24,    25,    81,    16,    56,    18,
      92,    56,    92,    56,    92,    56,    92,    56,    92,    56,
      92,    56,    56,    95,    87,    18,    85,    29,    45,    46,
      47,    48,    49,    50,    51,    52,    98,    98,    34,    96,
      87,     6,    79,    17,    16,    56,    16,    40,    41,    42,
      43,    44,    17,    17,    17,    17,    17,    93,    94,     3,
      18,    86,    84,     3,    56,    45,    56,    87,    56,    87,
      97,    95,    56,    78,     3,    53,    80,    17,    18,    56,
      16,    16,    16,    16,    16,    92,    92,    92,    92,    92,
      87,    17,    98,    29,    96,     3,     3,    17,     3,    56,
      17,    18,    56,    56,    56,    56,    56,    59,    86,    56,
      87,    56,    18,    75,     3,    56,    17,    29,    17,    29,
      17,    29,    17,    29,    17,    29,    17,    29,    56,    17,
      75,    92,    56,    92,    56,    92,    56,    92,    56,    92,
      56,    92,    56,    75,     3,    17,    17,    17,    17,    17,
      17,     3,    92,    92,    92,    92,    92
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    61,    62,    62,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    74,    74,    74,    75,    75,    76,    77,
      78,    78,    79,    79,    80,    81,    81,    81,    81,    82,
      83,    84,    85,    85,    86,    86,    87,    87,    87,    87,
      88,    89,    90,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      93,    93,    93,    94,    94,    95,    95,    96,    96,    97,
      97,    97,    97,    97,    97,    97,    98,    98,    98,    98,
      98,    98,    98,    98,    99
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       5,     3,     9,    10,    12,    13,     0,     3,     4,     8,
       0,     3,     5,     2,     1,     1,     1,     1,     1,     1,
       7,     4,     0,     2,     0,     3,     1,     1,     1,     1,
       5,     8,     7,     1,     2,     4,     3,     5,     7,     5,
       7,     5,     7,     5,     7,     5,     7,     5,     0,     6,
       8,     6,     8,     6,     8,     6,     8,     6,     8,     6,
       0,     3,     5,     0,     3,     0,     3,     0,     3,     3,
       3,     3,     3,     5,     5,     7,     1,     1,     1,     1,
       1,     1,     1,     2,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 22: /* exit: EXIT SEMICOLON  */
#line 170 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1415 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 175 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1423 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 180 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1431 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 186 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1439 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 192 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1447 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 198 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1455 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 204 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1464 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 210 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1472 "yacc_sql.tab.c"
    break;

  case 30: /* show_index: SHOW INDEX FROM ID SEMICOLON  */
#line 216 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
      show_index_init(&CONTEXT->ssql->sstr.show_index, (yyvsp[-1].string));
    }
#line 1481 "yacc_sql.tab.c"
    break;

  case 31: /* desc_table: DESC ID SEMICOLON  */
#line 223 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1490 "yacc_sql.tab.c"
    break;

  case 32: /* create_index: CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 231 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string), "");
		}
#line 1499 "yacc_sql.tab.c"
    break;

  case 33: /* create_index: CREATE UNIQUE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 236 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string), "UNIQUE");
		}
#line 1508 "yacc_sql.tab.c"
    break;

  case 34: /* create_index: CREATE INDEX ID ON ID LBRACE ID COMMA ID index_list RBRACE SEMICOLON  */
#line 241 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-9].string), (yyvsp[-7].string), (yyvsp[-5].string), "");
			AttrInfo attribute;
			index_attr_init(&attribute, (yyvsp[-5].string));
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			index_attr_init(&attribute, (yyvsp[-3].string));
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_index, &attribute);
		}
#line 1522 "yacc_sql.tab.c"
    break;

  case 35: /* create_index: CREATE UNIQUE INDEX ID ON ID LBRACE ID COMMA ID index_list RBRACE SEMICOLON  */
#line 251 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-9].string), (yyvsp[-7].string), (yyvsp[-5].string), "UNIQUE");
			AttrInfo attribute;
			index_attr_init(&attribute, (yyvsp[-5].string));
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			index_attr_init(&attribute, (yyvsp[-3].string));
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_index, &attribute);
		}
#line 1536 "yacc_sql.tab.c"
    break;

  case 37: /* index_list: COMMA ID index_list  */
#line 264 "yacc_sql.y"
                {
			AttrInfo attribute;
			index_attr_init(&attribute, (yyvsp[-1].string));
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1546 "yacc_sql.tab.c"
    break;

  case 38: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 273 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1555 "yacc_sql.tab.c"
    break;

  case 39: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 280 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1567 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 290 "yacc_sql.y"
                                   {    }
#line 1573 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type LBRACE number RBRACE  */
#line 295 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1588 "yacc_sql.tab.c"
    break;

  case 43: /* attr_def: ID_get type  */
#line 306 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1603 "yacc_sql.tab.c"
    break;

  case 44: /* number: NUMBER  */
#line 318 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1609 "yacc_sql.tab.c"
    break;

  case 45: /* type: INT_T  */
#line 321 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1615 "yacc_sql.tab.c"
    break;

  case 46: /* type: STRING_T  */
#line 322 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1621 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 323 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1627 "yacc_sql.tab.c"
    break;

  case 48: /* type: DATE_T  */
#line 324 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1633 "yacc_sql.tab.c"
    break;

  case 49: /* ID_get: ID  */
#line 328 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1642 "yacc_sql.tab.c"
    break;

  case 50: /* insert: INSERT INTO ID VALUES value_item value_array SEMICOLON  */
#line 337 "yacc_sql.y"
                {
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			// inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_array[CONTEXT->array_length]);
			// 为了支持多行数据插入，获取 value 在匹配规约为 value_item 时处理
			// 此处只为Inserts 结构体的relation 赋值
			inserts_init_relation(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string));

      //临时变量清零
      CONTEXT->value_length=0;
    }
#line 1664 "yacc_sql.tab.c"
    break;

  case 51: /* value_item: LBRACE value value_list RBRACE  */
#line 357 "yacc_sql.y"
                {
			CONTEXT->array_length++;// 从 insert 解析到 value_array 时，已经解析出来了一组 value_list，此时是多行插入的第二行
			inserts_append_values(&CONTEXT->ssql->sstr.insertion, CONTEXT->values, CONTEXT->value_length);
			CONTEXT->value_length=0;
		}
#line 1674 "yacc_sql.tab.c"
    break;

  case 53: /* value_array: COMMA value_item  */
#line 366 "yacc_sql.y"
                {
		}
#line 1681 "yacc_sql.tab.c"
    break;

  case 55: /* value_list: COMMA value value_list  */
#line 371 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1689 "yacc_sql.tab.c"
    break;

  case 56: /* value: NUMBER  */
#line 375 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1697 "yacc_sql.tab.c"
    break;

  case 57: /* value: FLOAT  */
#line 378 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1705 "yacc_sql.tab.c"
    break;

  case 58: /* value: DATE_STR  */
#line 381 "yacc_sql.y"
                 {
		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);// 跳过左右引号，只去引号包裹的字符串
		int ret = value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		if (ret){
			return ret;
		}
	}
#line 1717 "yacc_sql.tab.c"
    break;

  case 59: /* value: SSS  */
#line 388 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1726 "yacc_sql.tab.c"
    break;

  case 60: /* delete: DELETE FROM ID where SEMICOLON  */
#line 396 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1738 "yacc_sql.tab.c"
    break;

  case 61: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 406 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1750 "yacc_sql.tab.c"
    break;

  case 62: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 416 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 1770 "yacc_sql.tab.c"
    break;

  case 63: /* select_attr: STAR  */
#line 434 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1780 "yacc_sql.tab.c"
    break;

  case 64: /* select_attr: ID attr_list  */
#line 439 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1790 "yacc_sql.tab.c"
    break;

  case 65: /* select_attr: ID DOT ID attr_list  */
#line 444 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1800 "yacc_sql.tab.c"
    break;

  case 66: /* select_attr: STAR COMMA select_attr  */
#line 449 "yacc_sql.y"
                                 {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1810 "yacc_sql.tab.c"
    break;

  case 67: /* select_attr: MAX_SYM LBRACE ID RBRACE aggregation_list  */
#line 454 "yacc_sql.y"
                                                    {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MAX);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1821 "yacc_sql.tab.c"
    break;

  case 68: /* select_attr: MAX_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 460 "yacc_sql.y"
                                                           {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MAX);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1832 "yacc_sql.tab.c"
    break;

  case 69: /* select_attr: MIN_SYM LBRACE ID RBRACE aggregation_list  */
#line 466 "yacc_sql.y"
                                                    {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MIN);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1843 "yacc_sql.tab.c"
    break;

  case 70: /* select_attr: MIN_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 472 "yacc_sql.y"
                                                           {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MIN);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1854 "yacc_sql.tab.c"
    break;

  case 71: /* select_attr: AVG_SYM LBRACE ID RBRACE aggregation_list  */
#line 478 "yacc_sql.y"
                                                    {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, AVG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1865 "yacc_sql.tab.c"
    break;

  case 72: /* select_attr: AVG_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 484 "yacc_sql.y"
                                                           {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, AVG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1876 "yacc_sql.tab.c"
    break;

  case 73: /* select_attr: SUM_SYM LBRACE ID RBRACE aggregation_list  */
#line 490 "yacc_sql.y"
                                                    {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, SUM);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1887 "yacc_sql.tab.c"
    break;

  case 74: /* select_attr: SUM_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 496 "yacc_sql.y"
                                                           {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, SUM);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1898 "yacc_sql.tab.c"
    break;

  case 75: /* select_attr: COUNT_SYM LBRACE ID RBRACE aggregation_list  */
#line 502 "yacc_sql.y"
                                                      {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1909 "yacc_sql.tab.c"
    break;

  case 76: /* select_attr: COUNT_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 508 "yacc_sql.y"
                                                             {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1920 "yacc_sql.tab.c"
    break;

  case 77: /* select_attr: COUNT_SYM LBRACE STAR RBRACE aggregation_list  */
#line 514 "yacc_sql.y"
                                                        {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1931 "yacc_sql.tab.c"
    break;

  case 79: /* aggregation_list: COMMA MAX_SYM LBRACE ID RBRACE aggregation_list  */
#line 523 "yacc_sql.y"
                                                          {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MAX);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1942 "yacc_sql.tab.c"
    break;

  case 80: /* aggregation_list: COMMA MAX_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 529 "yacc_sql.y"
                                                                 {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MAX);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1953 "yacc_sql.tab.c"
    break;

  case 81: /* aggregation_list: COMMA MIN_SYM LBRACE ID RBRACE aggregation_list  */
#line 535 "yacc_sql.y"
                                                          {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MIN);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1964 "yacc_sql.tab.c"
    break;

  case 82: /* aggregation_list: COMMA MIN_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 541 "yacc_sql.y"
                                                                 {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, MIN);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1975 "yacc_sql.tab.c"
    break;

  case 83: /* aggregation_list: COMMA AVG_SYM LBRACE ID RBRACE aggregation_list  */
#line 547 "yacc_sql.y"
                                                          {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, AVG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1986 "yacc_sql.tab.c"
    break;

  case 84: /* aggregation_list: COMMA AVG_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 553 "yacc_sql.y"
                                                                 {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, AVG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1997 "yacc_sql.tab.c"
    break;

  case 85: /* aggregation_list: COMMA SUM_SYM LBRACE ID RBRACE aggregation_list  */
#line 559 "yacc_sql.y"
                                                          {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, SUM);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2008 "yacc_sql.tab.c"
    break;

  case 86: /* aggregation_list: COMMA SUM_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 565 "yacc_sql.y"
                                                                 {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, SUM);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2019 "yacc_sql.tab.c"
    break;

  case 87: /* aggregation_list: COMMA COUNT_SYM LBRACE ID RBRACE aggregation_list  */
#line 571 "yacc_sql.y"
                                                            {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2030 "yacc_sql.tab.c"
    break;

  case 88: /* aggregation_list: COMMA COUNT_SYM LBRACE ID DOT ID RBRACE aggregation_list  */
#line 577 "yacc_sql.y"
                                                                   {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string));
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2041 "yacc_sql.tab.c"
    break;

  case 89: /* aggregation_list: COMMA COUNT_SYM LBRACE STAR RBRACE aggregation_list  */
#line 583 "yacc_sql.y"
                                                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			relation_attr_set_aggre_type(&attr, COUNT);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2052 "yacc_sql.tab.c"
    break;

  case 91: /* attr_list: COMMA ID attr_list  */
#line 592 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2064 "yacc_sql.tab.c"
    break;

  case 92: /* attr_list: COMMA ID DOT ID attr_list  */
#line 599 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2076 "yacc_sql.tab.c"
    break;

  case 94: /* rel_list: COMMA ID rel_list  */
#line 610 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2084 "yacc_sql.tab.c"
    break;

  case 96: /* where: WHERE condition condition_list  */
#line 616 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2092 "yacc_sql.tab.c"
    break;

  case 98: /* condition_list: AND condition condition_list  */
#line 622 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2100 "yacc_sql.tab.c"
    break;

  case 99: /* condition: ID comOp value  */
#line 628 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2125 "yacc_sql.tab.c"
    break;

  case 100: /* condition: value comOp value  */
#line 649 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2149 "yacc_sql.tab.c"
    break;

  case 101: /* condition: ID comOp ID  */
#line 669 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
#line 2173 "yacc_sql.tab.c"
    break;

  case 102: /* condition: value comOp ID  */
#line 689 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
#line 2199 "yacc_sql.tab.c"
    break;

  case 103: /* condition: ID DOT ID comOp value  */
#line 711 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
#line 2224 "yacc_sql.tab.c"
    break;

  case 104: /* condition: value comOp ID DOT ID  */
#line 732 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
#line 2249 "yacc_sql.tab.c"
    break;

  case 105: /* condition: ID DOT ID comOp ID DOT ID  */
#line 753 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }
#line 2272 "yacc_sql.tab.c"
    break;

  case 106: /* comOp: EQ  */
#line 773 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2278 "yacc_sql.tab.c"
    break;

  case 107: /* comOp: LT  */
#line 774 "yacc_sql.y"
             { CONTEXT->comp = LESS_THAN; }
#line 2284 "yacc_sql.tab.c"
    break;

  case 108: /* comOp: GT  */
#line 775 "yacc_sql.y"
             { CONTEXT->comp = GREAT_THAN; }
#line 2290 "yacc_sql.tab.c"
    break;

  case 109: /* comOp: LE  */
#line 776 "yacc_sql.y"
             { CONTEXT->comp = LESS_EQUAL; }
#line 2296 "yacc_sql.tab.c"
    break;

  case 110: /* comOp: GE  */
#line 777 "yacc_sql.y"
             { CONTEXT->comp = GREAT_EQUAL; }
#line 2302 "yacc_sql.tab.c"
    break;

  case 111: /* comOp: NE  */
#line 778 "yacc_sql.y"
             { CONTEXT->comp = NOT_EQUAL; }
#line 2308 "yacc_sql.tab.c"
    break;

  case 112: /* comOp: LIKE_SYM  */
#line 779 "yacc_sql.y"
                   { CONTEXT->comp = LIKE; }
#line 2314 "yacc_sql.tab.c"
    break;

  case 113: /* comOp: NOT LIKE_SYM  */
#line 780 "yacc_sql.y"
                       { CONTEXT->comp = NOT_LIKE; }
#line 2320 "yacc_sql.tab.c"
    break;

  case 114: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 785 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2329 "yacc_sql.tab.c"
    break;


#line 2333 "yacc_sql.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 790 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
