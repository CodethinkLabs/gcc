/* Part of CPP library.
   Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This header defines all the internal data structures and functions
   that need to be visible across files.  It's called cpphash.h for
   historical reasons.  */

#ifndef __GCC_CPPHASH__
#define __GCC_CPPHASH__

typedef unsigned char U_CHAR;

/* Structure allocated for every #define.  For a simple replacement
   such as
   	#define foo bar ,
   nargs = -1, the `pattern' list is null, and the expansion is just
   the replacement text.  Nargs = 0 means a functionlike macro with no args,
   e.g.,
       #define getchar() getc (stdin) .
   When there are args, the expansion is the replacement text with the
   args squashed out, and the reflist is a list describing how to
   build the output from the input: e.g., "3 chars, then the 1st arg,
   then 9 chars, then the 3rd arg, then 0 chars, then the 2nd arg".
   The chars here come from the expansion.  Whatever is left of the
   expansion after the last arg-occurrence is copied after that arg.
   Note that the reflist can be arbitrarily long---
   its length depends on the number of times the arguments appear in
   the replacement text, not how many args there are.  Example:
   #define f(x) x+x+x+x+x+x+x would have replacement text "++++++" and
   pattern list
     { (0, 1), (1, 1), (1, 1), ..., (1, 1), NULL }
   where (x, y) means (nchars, argno). */

struct reflist
{
  struct reflist *next;
  char stringify;		/* nonzero if this arg was preceded by a
				   # operator. */
  char raw_before;		/* Nonzero if a ## operator before arg. */
  char raw_after;		/* Nonzero if a ## operator after arg. */
  char rest_args;		/* Nonzero if this arg. absorbs the rest */
  int nchars;			/* Number of literal chars to copy before
				   this arg occurrence.  */
  int argno;			/* Number of arg to substitute (origin-0) */
};

typedef struct definition DEFINITION;
struct definition
{
  int nargs;
  int length;			/* length of expansion string */
  U_CHAR *expansion;
  int line;			/* Line number of definition */
  int col;
  const char *file;		/* File of definition */
  char rest_args;		/* Nonzero if last arg. absorbs the rest */
  struct reflist *pattern;

  /* Names of macro args, concatenated in order with \0 between
     them.  The only use of this is that we warn on redefinition if
     this differs between the old and new definitions.  */
  U_CHAR *argnames;
};

/* The structure of a node in the hash table.  The hash table
   has entries for all tokens defined by #define commands (type T_MACRO),
   plus some special tokens like __LINE__ (these each have their own
   type, and the appropriate code is run when that type of node is seen.
   It does not contain control words like "#define", which are recognized
   by a separate piece of code. */

/* different flavors of hash nodes --- also used in keyword table */
enum node_type
{
  T_DEFINE = 1,	   /* `#define' */
  T_INCLUDE,	   /* `#include' */
  T_INCLUDE_NEXT,  /* `#include_next' */
  T_IMPORT,        /* `#import' */
  T_IFDEF,	   /* `#ifdef' */
  T_IFNDEF,	   /* `#ifndef' */
  T_IF,		   /* `#if' */
  T_ELSE,	   /* `#else' */
  T_PRAGMA,	   /* `#pragma' */
  T_ELIF,	   /* `#elif' */
  T_UNDEF,	   /* `#undef' */
  T_LINE,	   /* `#line' */
  T_ERROR,	   /* `#error' */
  T_WARNING,	   /* `#warning' */
  T_ENDIF,	   /* `#endif' */
  T_SCCS,	   /* `#sccs' */
  T_IDENT,	   /* `#ident' */
  T_ASSERT,	   /* `#assert' */
  T_UNASSERT,	   /* `#unassert', */
  T_SPECLINE,	   /* `__LINE__' */
  T_DATE,	   /* `__DATE__' */
  T_FILE,	   /* `__FILE__' */
  T_BASE_FILE,	   /* `__BASE_FILE__' */
  T_INCLUDE_LEVEL, /* `__INCLUDE_LEVEL__' */
  T_VERSION,	   /* `__VERSION__' */
  T_TIME,	   /* `__TIME__' */
  T_STDC,	   /* `__STDC__' */
  T_CONST,	   /* Constant string, used by `__SIZE_TYPE__' etc */
  T_MACRO,	   /* macro defined by `#define' */
  T_DISABLED,	   /* macro temporarily turned off for rescan */
  T_POISON,	   /* macro defined with `#pragma poison' */
  T_UNUSED	   /* Used for something not defined.  */
};

/* different kinds of things that can appear in the value field
   of a hash node. */
union hashval
{
  const char *cpval;		/* some predefined macros */
  DEFINITION *defn;		/* #define */
  struct hashnode *aschain;	/* #assert */
};

typedef struct hashnode HASHNODE;
struct hashnode
{
  struct hashnode *next;	/* double links for easy deletion */
  struct hashnode *prev;
  struct hashnode **bucket_hdr;	/* also, a back pointer to this node's hash
				   chain is kept, in case the node is the head
				   of the chain and gets deleted. */
  enum node_type type;		/* type of special token */
  int length;			/* length of token, for quick comparison */
  U_CHAR *name;			/* the actual name */
  union hashval value;		/* pointer to expansion, or whatever */
};

/* List of directories to look for include files in. */
struct file_name_list
{
  struct file_name_list *next;
  struct file_name_list *alloc; /* for the cache of
				   current directory entries */
  char *name;
  unsigned int nlen;
  /* We use these to tell if the directory mentioned here is a duplicate
     of an earlier directory on the search path. */
  ino_t ino;
  dev_t dev;
  /* If the following is nonzero, it is a C-language system include
     directory.  */
  int sysp;
  /* Mapping of file names for this directory.
     Only used on MS-DOS and related platforms. */
  struct file_name_map *name_map;
};
#define ABSOLUTE_PATH ((struct file_name_list *)-1)

/* This structure is used for the table of all includes.  It is
   indexed by the `short name' (the name as it appeared in the
   #include statement) which is stored in *nshort.  */
struct ihash
{
  struct ihash *next;
  /* Next file with the same short name but a
     different (partial) pathname). */
  struct ihash *next_this_file;

  /* Location of the file in the include search path.
     Used for include_next */
  struct file_name_list *foundhere;
  const char *name;		/* (partial) pathname of file */
  const char *nshort;		/* name of file as referenced in #include */
  const U_CHAR *control_macro;	/* macro, if any, preventing reinclusion -
				   see redundant_include_p */
  char *buf, *limit;		/* for file content cache,
				   not yet implemented */
};
typedef struct ihash IHASH;

/* Character classes.
   If the definition of `numchar' looks odd to you, please look up the
   definition of a pp-number in the C standard [section 6.4.8 of C99] */
#define ISidnum		0x01	/* a-zA-Z0-9_ */
#define ISidstart	0x02	/* _a-zA-Z */
#define ISnumstart	0x04	/* 0-9 */
#define IShspace	0x08	/* ' ' \t \f \v */
#define ISspace		0x10	/* ' ' \t \f \v \n */

#define _dollar_ok(x)	((x) == '$' && CPP_OPTIONS (pfile)->dollars_in_ident)

#define is_idchar(x)	((_cpp_IStable[x] & ISidnum) || _dollar_ok(x))
#define is_idstart(x)	((_cpp_IStable[x] & ISidstart) || _dollar_ok(x))
#define is_numchar(x)	(_cpp_IStable[x] & ISidnum)
#define is_numstart(x)	(_cpp_IStable[x] & ISnumstart)
#define is_hspace(x)	(_cpp_IStable[x] & IShspace)
#define is_space(x)	(_cpp_IStable[x] & ISspace)

/* This table is constant if it can be initialized at compile time,
   which is the case if cpp was compiled with GCC >=2.7, or another
   compiler that supports C99.  */
#if (GCC_VERSION >= 2007) || (__STDC_VERSION__ >= 199901L)
extern const unsigned char _cpp_IStable[256];
#else
extern unsigned char _cpp_IStable[256];
#endif

/* Macros.  */

#define CPP_BUF_PEEK(BUFFER) \
  ((BUFFER)->cur < (BUFFER)->rlimit ? *(BUFFER)->cur : EOF)
#define CPP_BUF_GET(BUFFER) \
  ((BUFFER)->cur < (BUFFER)->rlimit ? *(BUFFER)->cur++ : EOF)
#define CPP_FORWARD(BUFFER, N) ((BUFFER)->cur += (N))

/* Append string STR (of length N) to PFILE's output buffer.
   Assume there is enough space. */
#define CPP_PUTS_Q(PFILE, STR, N) \
  (memcpy ((PFILE)->limit, STR, (N)), (PFILE)->limit += (N))
/* Append string STR (of length N) to PFILE's output buffer.  Make space. */
#define CPP_PUTS(PFILE, STR, N) CPP_RESERVE(PFILE, N), CPP_PUTS_Q(PFILE, STR,N)
/* Append character CH to PFILE's output buffer.  Assume sufficient space. */
#define CPP_PUTC_Q(PFILE, CH) (*(PFILE)->limit++ = (CH))
/* Append character CH to PFILE's output buffer.  Make space if need be. */
#define CPP_PUTC(PFILE, CH) (CPP_RESERVE (PFILE, 1), CPP_PUTC_Q (PFILE, CH))
/* Make sure PFILE->limit is followed by '\0'. */
#define CPP_NUL_TERMINATE_Q(PFILE) (*(PFILE)->limit = 0)
#define CPP_NUL_TERMINATE(PFILE) (CPP_RESERVE(PFILE, 1), *(PFILE)->limit = 0)

/* Advance the current line by one. */
#define CPP_BUMP_BUFFER_LINE(PBUF) ((PBUF)->lineno++,\
				    (PBUF)->line_base = (PBUF)->cur)
#define CPP_BUMP_LINE(PFILE) CPP_BUMP_BUFFER_LINE(CPP_BUFFER(PFILE))
#define CPP_PREV_BUFFER(BUFFER) ((BUFFER)->prev)

#define CPP_PRINT_DEPS(PFILE) (CPP_OPTIONS (PFILE)->print_deps)
#define CPP_TRADITIONAL(PFILE) (CPP_OPTIONS(PFILE)->traditional)
#define CPP_PEDANTIC(PFILE) \
  (CPP_OPTIONS (PFILE)->pedantic && !CPP_BUFFER (pfile)->system_header_p)

/* In cpphash.c */
extern HASHNODE *_cpp_install	  PARAMS ((cpp_reader *, const U_CHAR *, int,
					   enum node_type, const char *));
extern HASHNODE *_cpp_lookup	  PARAMS ((cpp_reader *, const U_CHAR *, int));
extern void _cpp_free_definition  PARAMS ((DEFINITION *));
extern void _cpp_delete_macro	  PARAMS ((HASHNODE *));

extern DEFINITION *_cpp_create_definition
				  PARAMS ((cpp_reader *, int));
extern int _cpp_compare_defs		  PARAMS ((cpp_reader *, DEFINITION *,
					   DEFINITION *));
extern void _cpp_macroexpand	  PARAMS ((cpp_reader *, HASHNODE *));
extern void _cpp_dump_definition  PARAMS ((cpp_reader *, const U_CHAR *, long,
					   DEFINITION *));

/* In cppfiles.c */
extern void _cpp_simplify_pathname	PARAMS ((char *));
extern int _cpp_find_include_file	PARAMS ((cpp_reader *, const char *,
						struct file_name_list *,
						IHASH **, int *));
extern int _cpp_read_include_file	PARAMS ((cpp_reader *, int, IHASH *));

/* In cppexp.c */
extern int _cpp_parse_expr		PARAMS ((cpp_reader *));

#endif
