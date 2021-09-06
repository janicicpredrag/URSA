/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_URSA_TAB_HPP_INCLUDED
# define YY_YY_URSA_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    BOOLEAN = 259,
    BOOLEAN_VARIABLE = 260,
    INTEGER_VARIABLE = 261,
    BOOLEAN_ARRAY = 262,
    INTEGER_ARRAY = 263,
    PROCEDURE_ID = 264,
    FOR = 265,
    WHILE = 266,
    IF = 267,
    PRINT = 268,
    PRINTB = 269,
    PRINTX = 270,
    MINIMIZE = 271,
    MAXIMIZE = 272,
    ASSERT = 273,
    ASSERTA = 274,
    LIST = 275,
    CLEAR = 276,
    HALT = 277,
    PROCEDURE = 278,
    CALL = 279,
    IFX = 280,
    ELSE = 281,
    PLUSEQ = 282,
    MINUSEQ = 283,
    MULTEQ = 284,
    DIVEQ = 285,
    ANDEQ = 286,
    OREQ = 287,
    XOREQ = 288,
    LSHIFTEQ = 289,
    RSHIFTEQ = 290,
    BITWISEANDEQ = 291,
    BITWISEOREQ = 292,
    BITWISEXOREQ = 293,
    LOGICALXOR = 294,
    LOGICALOR = 295,
    LOGICALAND = 296,
    GE = 297,
    LE = 298,
    EQ = 299,
    NE = 300,
    LSHIFT = 301,
    RSHIFT = 302,
    PLUSPLUS = 303,
    MINUSMINUS = 304,
    UMINUS = 305,
    ITE = 306,
    BOOL2NUM = 307,
    NUM2BOOL = 308,
    SGN = 309
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 22 "Parser/ursa.ypp" /* yacc.c:1909  */

    char* sValue;               /* integer value written as string */
    char  bValue;               /* boolean value */
    char* sName;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */

#line 116 "ursa.tab.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_URSA_TAB_HPP_INCLUDED  */
