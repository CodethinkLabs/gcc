/* Definitions for CPP library.
   Copyright (C) 1995, 96-99, 2000 Free Software Foundation, Inc.
   Written by Per Bothner, 1994-95.

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
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */
#ifndef __GCC_CPPLIB__
#define __GCC_CPPLIB__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For complex reasons, cpp_reader is also typedefed in c-pragma.h.  */
#ifndef _C_PRAGMA_H
typedef struct cpp_reader cpp_reader;
#endif
typedef struct cpp_buffer cpp_buffer;
typedef struct cpp_options cpp_options;
typedef struct cpp_token cpp_token;
typedef struct cpp_string cpp_string;
typedef struct cpp_hashnode cpp_hashnode;
typedef struct cpp_pool cpp_pool;
typedef struct cpp_macro cpp_macro;
typedef struct cpp_lexer_pos cpp_lexer_pos;
typedef struct cpp_lookahead cpp_lookahead;

struct directive;		/* These are deliberately incomplete.  */
struct answer;
struct cpp_macro;
struct macro_args;
struct cpp_chunk;
struct file_name_map_list;
struct htab;

/* The first two groups, apart from '=', can appear in preprocessor
   expressions.  This allows a lookup table to be implemented in
   _cpp_parse_expr.

   The first group, to CPP_LAST_EQ, can be immediately followed by an
   '='.  The lexer needs operators ending in '=', like ">>=", to be in
   the same order as their counterparts without the '=', like ">>".  */

/* Positions in the table.  */
#define CPP_LAST_EQ CPP_MAX
#define CPP_FIRST_DIGRAPH CPP_HASH
#define CPP_LAST_PUNCTUATOR CPP_DOT_STAR

#define TTYPE_TABLE				\
  OP(CPP_EQ = 0,	"=")			\
  OP(CPP_NOT,		"!")			\
  OP(CPP_GREATER,	">")	/* compare */	\
  OP(CPP_LESS,		"<")			\
  OP(CPP_PLUS,		"+")	/* math */	\
  OP(CPP_MINUS,		"-")			\
  OP(CPP_MULT,		"*")			\
  OP(CPP_DIV,		"/")			\
  OP(CPP_MOD,		"%")			\
  OP(CPP_AND,		"&")	/* bit ops */	\
  OP(CPP_OR,		"|")			\
  OP(CPP_XOR,		"^")			\
  OP(CPP_RSHIFT,	">>")			\
  OP(CPP_LSHIFT,	"<<")			\
  OP(CPP_MIN,		"<?")	/* extension */	\
  OP(CPP_MAX,		">?")			\
\
  OP(CPP_COMPL,		"~")			\
  OP(CPP_AND_AND,	"&&")	/* logical */	\
  OP(CPP_OR_OR,		"||")			\
  OP(CPP_QUERY,		"?")			\
  OP(CPP_COLON,		":")			\
  OP(CPP_COMMA,		",")	/* grouping */	\
  OP(CPP_OPEN_PAREN,	"(")			\
  OP(CPP_CLOSE_PAREN,	")")			\
  OP(CPP_EQ_EQ,		"==")	/* compare */	\
  OP(CPP_NOT_EQ,	"!=")			\
  OP(CPP_GREATER_EQ,	">=")			\
  OP(CPP_LESS_EQ,	"<=")			\
\
  OP(CPP_PLUS_EQ,	"+=")	/* math */	\
  OP(CPP_MINUS_EQ,	"-=")			\
  OP(CPP_MULT_EQ,	"*=")			\
  OP(CPP_DIV_EQ,	"/=")			\
  OP(CPP_MOD_EQ,	"%=")			\
  OP(CPP_AND_EQ,	"&=")	/* bit ops */	\
  OP(CPP_OR_EQ,		"|=")			\
  OP(CPP_XOR_EQ,	"^=")			\
  OP(CPP_RSHIFT_EQ,	">>=")			\
  OP(CPP_LSHIFT_EQ,	"<<=")			\
  OP(CPP_MIN_EQ,	"<?=")	/* extension */	\
  OP(CPP_MAX_EQ,	">?=")			\
  /* Digraphs together, beginning with CPP_FIRST_DIGRAPH.  */	\
  OP(CPP_HASH,		"#")	/* digraphs */	\
  OP(CPP_PASTE,		"##")			\
  OP(CPP_OPEN_SQUARE,	"[")			\
  OP(CPP_CLOSE_SQUARE,	"]")			\
  OP(CPP_OPEN_BRACE,	"{")			\
  OP(CPP_CLOSE_BRACE,	"}")			\
  /* The remainder of the punctuation.  Order is not significant.  */	\
  OP(CPP_SEMICOLON,	";")	/* structure */	\
  OP(CPP_ELLIPSIS,	"...")			\
  OP(CPP_PLUS_PLUS,	"++")	/* increment */	\
  OP(CPP_MINUS_MINUS,	"--")			\
  OP(CPP_DEREF,		"->")	/* accessors */	\
  OP(CPP_DOT,		".")			\
  OP(CPP_SCOPE,		"::")			\
  OP(CPP_DEREF_STAR,	"->*")			\
  OP(CPP_DOT_STAR,	".*")			\
\
  TK(CPP_NAME,		SPELL_IDENT)	/* word */			\
  TK(CPP_INT,		SPELL_STRING)	/* 23 */			\
  TK(CPP_FLOAT,		SPELL_STRING)	/* 3.14159 */			\
  TK(CPP_NUMBER,	SPELL_STRING)	/* 34_be+ta  */			\
\
  TK(CPP_CHAR,		SPELL_STRING)	/* 'char' */			\
  TK(CPP_WCHAR,		SPELL_STRING)	/* L'char' */			\
  TK(CPP_OTHER,		SPELL_CHAR)	/* stray punctuation */		\
\
  TK(CPP_STRING,	SPELL_STRING)	/* "string" */			\
  TK(CPP_WSTRING,	SPELL_STRING)	/* L"string" */			\
  TK(CPP_OSTRING,	SPELL_STRING)	/* @"string" - Objective C */	\
  TK(CPP_HEADER_NAME,	SPELL_STRING)	/* <stdio.h> in #include */	\
\
  TK(CPP_COMMENT,	SPELL_STRING)	/* Only if output comments.  */ \
  TK(CPP_MACRO_ARG,	SPELL_NONE)	/* Macro argument.  */		\
  OP(CPP_EOF,		"EOL")		/* End of line or file.  */

#define OP(e, s) e,
#define TK(e, s) e,
enum cpp_ttype
{
  TTYPE_TABLE
  N_TTYPES
};
#undef OP
#undef TK

/* Multiple-include optimisation.  */
enum mi_state {MI_FAILED = 0, MI_OUTSIDE};
enum mi_ind {MI_IND_NONE = 0, MI_IND_NOT};

/* Payload of a NUMBER, FLOAT, STRING, or COMMENT token.  */
struct cpp_string
{
  unsigned int len;
  const unsigned char *text;
};

/* Flags for the cpp_token structure.  */
#define PREV_WHITE	(1 << 0) /* If whitespace before this token.  */
#define DIGRAPH		(1 << 1) /* If it was a digraph.  */
#define STRINGIFY_ARG	(1 << 2) /* If macro argument to be stringified.  */
#define PASTE_LEFT	(1 << 3) /* If on LHS of a ## operator.  */
#define NAMED_OP	(1 << 4) /* C++ named operators, also "defined".  */
#define NO_EXPAND	(1 << 5) /* Do not macro-expand this token.  */

/* A preprocessing token.  This has been carefully packed and should
   occupy 12 bytes on 32-bit hosts and 16 bytes on 64-bit hosts.  */
struct cpp_token
{
  ENUM_BITFIELD(cpp_ttype) type : CHAR_BIT;  /* token type */
  unsigned char flags;		/* flags - see above */

  union
  {
    struct cpp_hashnode *node;	/* An identifier.  */
    struct cpp_string str;	/* A string, or number.  */
    unsigned int arg_no;	/* Argument no. for a CPP_MACRO_ARG.  */
    unsigned char c;		/* Character represented by CPP_OTHER.  */
  } val;
};

/* The position of a token in the current file.  */
struct cpp_lexer_pos
{
  unsigned int line;
  unsigned int output_line;
  unsigned short col;
};

typedef struct cpp_token_with_pos cpp_token_with_pos;
struct cpp_token_with_pos
{
  cpp_token token;
  cpp_lexer_pos pos;
};

/* Token lookahead.  */
struct cpp_lookahead
{
  struct cpp_lookahead *next;
  cpp_token_with_pos *tokens;
  cpp_lexer_pos pos;
  unsigned int cur, count, cap;
};

/* Memory pools.  */
struct cpp_pool
{
  struct cpp_chunk *cur, *locked;
  unsigned char *pos;		/* Current position.  */
  unsigned int align;
  unsigned int locks;
};

typedef struct toklist toklist;
struct toklist
{
  cpp_token *first;
  cpp_token *limit;
};

typedef struct cpp_context cpp_context;
struct cpp_context
{
  /* Doubly-linked list.  */
  cpp_context *next, *prev;

  /* Contexts other than the base context are contiguous tokens.
     e.g. macro expansions, expanded argument tokens.  */
  struct toklist list;

  /* For a macro context, these are the macro and its arguments.  */
  cpp_macro *macro;
};

/* A standalone character.  We may want to make it unsigned for the
   same reason we use unsigned char - to avoid signedness issues.  */
typedef int cppchar_t;

struct cpp_buffer
{
  const unsigned char *cur;	 /* current position */
  const unsigned char *rlimit; /* end of valid data */
  const unsigned char *line_base; /* start of current line */
  cppchar_t read_ahead;		/* read ahead character */
  cppchar_t extra_char;		/* extra read-ahead for long tokens.  */

  struct cpp_reader *pfile;	/* Owns this buffer.  */
  struct cpp_buffer *prev;

  const unsigned char *buf;	 /* entire buffer */

  /* Filename specified with #line command.  */
  const char *nominal_fname;

  /* Actual directory of this file, used only for "" includes */
  struct file_name_list *actual_dir;

  /* Pointer into the include table.  Used for include_next and
     to record control macros. */
  struct include_file *inc;

  /* Value of if_stack at start of this file.
     Used to prohibit unmatched #endif (etc) in an include file.  */
  struct if_stack *if_stack;

  /* Token column position adjustment owing to tabs in whitespace.  */
  unsigned int col_adjust;

  /* Line number at line_base (above). */
  unsigned int lineno;

  /* True if we have already warned about C++ comments in this file.
     The warning happens only for C89 extended mode with -pedantic on,
     or for -Wtraditional, and only once per file (otherwise it would
     be far too noisy).  */
  unsigned char warned_cplusplus_comments;

  /* True if we don't process trigraphs and escaped newlines.  True
     for preprocessed input, command line directives, and _Pragma
     buffers.  */
  unsigned char from_stage3;

  /* Temporary storage for pfile->skipping whilst in a directive.  */
  unsigned char was_skipping;
};

/* Maximum nesting of cpp_buffers.  We use a static limit, partly for
   efficiency, and partly to limit runaway recursion.  */
#define CPP_STACK_MAX 200

/* Values for opts.dump_macros.
  dump_only means inhibit output of the preprocessed text
             and instead output the definitions of all user-defined
             macros in a form suitable for use as input to cpp.
   dump_names means pass #define and the macro name through to output.
   dump_definitions means pass the whole definition (plus #define) through
*/
enum { dump_none = 0, dump_only, dump_names, dump_definitions };

/* This structure is nested inside struct cpp_reader, and
   carries all the options visible to the command line.  */
struct cpp_options
{
  /* Name of input and output files.  */
  const char *in_fname;
  const char *out_fname;

  /* Characters between tab stops.  */
  unsigned int tabstop;

  /* Pending options - -D, -U, -A, -I, -ixxx. */
  struct cpp_pending *pending;

  /* File name which deps are being written to.  This is 0 if deps are
     being written to stdout.  */
  const char *deps_file;

  /* Target-name to write with the dependency information.  */
  char *deps_target;

  /* Search paths for include files.  */
  struct file_name_list *quote_include;	 /* First dir to search for "file" */
  struct file_name_list *bracket_include;/* First dir to search for <file> */

  /* Map between header names and file names, used only on DOS where
     file names are limited in length.  */
  struct file_name_map_list *map_list;

  /* Directory prefix that should replace `/usr/lib/gcc-lib/TARGET/VERSION'
     in the standard include file directories.  */
  const char *include_prefix;
  unsigned int include_prefix_len;

  /* -fleading_underscore sets this to "_".  */
  const char *user_label_prefix;

  /* Non-0 means -v, so print the full set of include dirs.  */
  unsigned char verbose;

  /* Nonzero means use extra default include directories for C++.  */
  unsigned char cplusplus;

  /* Nonzero means handle cplusplus style comments */
  unsigned char cplusplus_comments;

  /* Nonzero means handle #import, for objective C.  */
  unsigned char objc;

  /* Nonzero means this is an assembly file, so ignore unrecognized
     directives and the "# 33" form of #line, both of which are
     probably comments.  Also, permit unbalanced ' strings (again,
     likely to be in comments).  */
  unsigned char lang_asm;

  /* Nonzero means don't copy comments into the output file.  */
  unsigned char discard_comments;

  /* Nonzero means process the ISO trigraph sequences.  */
  unsigned char trigraphs;

  /* Nonzero means process the ISO digraph sequences.  */
  unsigned char digraphs;

  /* Nonzero means print the names of included files rather than the
     preprocessed output.  1 means just the #include "...", 2 means
     #include <...> as well.  */
  unsigned char print_deps;

  /* Nonzero if missing .h files in -M output are assumed to be
     generated files and not errors.  */
  unsigned char print_deps_missing_files;

  /* If true, fopen (deps_file, "a") else fopen (deps_file, "w"). */
  unsigned char print_deps_append;

  /* Nonzero means print names of header files (-H).  */
  unsigned char print_include_names;

  /* Nonzero means cpp_pedwarn causes a hard error.  */
  unsigned char pedantic_errors;

  /* Nonzero means don't print warning messages.  */
  unsigned char inhibit_warnings;

  /* Nonzero means don't suppress warnings from system headers.  */
  unsigned char warn_system_headers;

  /* Nonzero means don't print error messages.  Has no option to
     select it, but can be set by a user of cpplib (e.g. fix-header).  */
  unsigned char inhibit_errors;

  /* Nonzero means warn if slash-star appears in a comment.  */
  unsigned char warn_comments;

  /* Nonzero means warn if there are any trigraphs.  */
  unsigned char warn_trigraphs;

  /* Nonzero means warn if #import is used.  */
  unsigned char warn_import;

  /* Nonzero means warn about various incompatibilities with
     traditional C.  */
  unsigned char warn_traditional;

  /* Nonzero means warn if ## is applied to two tokens that cannot be
     pasted together.  */
  unsigned char warn_paste;

  /* Nonzero means turn warnings into errors.  */
  unsigned char warnings_are_errors;

  /* Nonzero causes output not to be done, but directives such as
     #define that have side effects are still obeyed.  */
  unsigned char no_output;

  /* Nonzero means we should look for header.gcc files that remap file
     names.  */
  unsigned char remap;

  /* Nonzero means don't output line number information.  */
  unsigned char no_line_commands;

  /* Nonzero means -I- has been seen, so don't look for #include "foo"
     the source-file directory.  */
  unsigned char ignore_srcdir;

  /* Zero means dollar signs are punctuation. */
  unsigned char dollars_in_ident;

  /* Nonzero means warn if undefined identifiers are evaluated in an #if.  */
  unsigned char warn_undef;

  /* Nonzero for the 1989 C Standard, including corrigenda and amendments.  */
  unsigned char c89;

  /* Nonzero for the 1999 C Standard, including corrigenda and amendments.  */
  unsigned char c99;

  /* Nonzero means give all the error messages the ANSI standard requires.  */
  unsigned char pedantic;

  /* Nonzero means we're looking at already preprocessed code, so don't
     bother trying to do macro expansion and whatnot.  */
  unsigned char preprocessed;

  /* Nonzero disables all the standard directories for headers.  */
  unsigned char no_standard_includes;

  /* Nonzero disables the C++-specific standard directories for headers.  */
  unsigned char no_standard_cplusplus_includes;

  /* Nonzero means dump macros in some fashion - see above.  */
  unsigned char dump_macros;

  /* Nonzero means pass all #define and #undef directives which we
     actually process through to the output stream.  This feature is
     used primarily to allow cc1 to record the #defines and #undefs
     for the sake of debuggers which understand about preprocessor
     macros, but it may also be useful with -E to figure out how
     symbols are defined, and where they are defined.  */
  unsigned char debug_output;

  /* Nonzero means pass #include lines through to the output.  */
  unsigned char dump_includes;

  /* Print column number in error messages.  */
  unsigned char show_column;
};

struct lexer_state
{
  /* Nonzero if first token on line is CPP_HASH.  */
  unsigned char in_directive;

  /* Nonzero if in a directive that takes angle-bracketed headers.  */
  unsigned char angled_headers;

  /* Nonzero to save comments.  Turned off if discard_comments, and in
     all directives apart from #define.  */
  unsigned char save_comments;

  /* If nonzero the next token is at the beginning of the line.  */
  unsigned char next_bol;

  /* Nonzero if we're mid-comment.  */
  unsigned char lexing_comment;

  /* Nonzero if lexing __VA_ARGS__ is valid.  */
  unsigned char va_args_ok;

  /* Nonzero if lexing poisoned identifiers is valid.  */
  unsigned char poisoned_ok;

  /* Nonzero to prevent macro expansion.  */
  unsigned char prevent_expansion;  

  /* Nonzero when parsing arguments to a function-like macro.  */
  unsigned char parsing_args;
};

/* Special nodes - identifiers with predefined significance.  */
struct spec_nodes
{
  cpp_hashnode *n_L;			/* L"str" */
  cpp_hashnode *n_defined;		/* defined operator */
  cpp_hashnode *n__Pragma;		/* _Pragma operator */
  cpp_hashnode *n__STRICT_ANSI__;	/* STDC_0_IN_SYSTEM_HEADERS */
  cpp_hashnode *n__CHAR_UNSIGNED__;	/* plain char is unsigned */
  cpp_hashnode *n__VA_ARGS__;		/* C99 vararg macros */
};

/* a cpp_reader encapsulates the "state" of a pre-processor run.
   Applying cpp_get_token repeatedly yields a stream of pre-processor
   tokens.  Usually, there is only one cpp_reader object active.  */

struct cpp_reader
{
  /* Top of buffer stack.  */
  cpp_buffer *buffer;

  /* Lexer state.  */
  struct lexer_state state;

  /* The position of the last lexed token, last lexed directive, and
     last macro invocation.  */
  cpp_lexer_pos lexer_pos;
  cpp_lexer_pos macro_pos;
  cpp_lexer_pos directive_pos;

  /* Memory pools.  */
  cpp_pool ident_pool;		/* For all identifiers, and permanent
				   numbers and strings.  */
  cpp_pool temp_string_pool;	/* For temporary numbers and strings.   */
  cpp_pool macro_pool;		/* For macro definitions.  Permanent.  */
  cpp_pool argument_pool;	/* For macro arguments.  Temporary.   */
  cpp_pool* string_pool;	/* Either temp_string_pool or ident_pool.   */

  /* Context stack.  */
  struct cpp_context base_context;
  struct cpp_context *context;

  /* If in_directive, the directive if known.  */
  const struct directive *directive;

  /* Multiple inlcude optimisation.  */
  enum mi_state mi_state;
  enum mi_ind mi_if_not_defined;
  unsigned int mi_lexed;
  const cpp_hashnode *mi_cmacro;
  const cpp_hashnode *mi_ind_cmacro;

  /* Token lookahead.  */
  struct cpp_lookahead *la_read;	/* Read from this lookahead.  */
  struct cpp_lookahead *la_write;	/* Write to this lookahead.  */
  struct cpp_lookahead *la_unused;	/* Free store.  */

  /* Error counter for exit code.  */
  unsigned int errors;

  /* Line and column where a newline was first seen in a string
     constant (multi-line strings).  */
  cpp_lexer_pos mlstring_pos;

  /* Buffer to hold macro definition string.  */
  unsigned char *macro_buffer;
  unsigned int macro_buffer_len;

  /* Current depth in #include directives that use <...>.  */
  unsigned int system_include_depth;

  /* Current depth of buffer stack.  */
  unsigned int buffer_stack_depth;

  /* Current depth in #include directives.  */
  unsigned int include_depth;

  /* Hash table of macros and assertions.  See cpphash.c.  */
  struct htab *hashtab;

  /* Tree of other included files.  See cppfiles.c.  */
  struct splay_tree_s *all_include_files;

  /* Chain of `actual directory' file_name_list entries, for ""
     inclusion.  */
  struct file_name_list *actual_dirs;

  /* Current maximum length of directory names in the search path
     for include files.  (Altered as we get more of them.)  */
  unsigned int max_include_len;

  /* Date and time tokens.  Calculated together if either is requested.  */
  cpp_token date;
  cpp_token time;

  /* Buffer of -M output.  */
  struct deps *deps;

  /* Obstack holding all macro hash nodes.  This never shrinks.
     See cpphash.c */
  struct obstack *hash_ob;

  /* Obstack holding buffer and conditional structures.  This is a
     real stack.  See cpplib.c */
  struct obstack *buffer_ob;

  /* Pragma table - dynamic, because a library user can add to the
     list of recognized pragmas.  */
  struct pragma_entry *pragmas;

  /* Call backs.  */
  struct {
    void (*enter_file) PARAMS ((cpp_reader *));
    void (*leave_file) PARAMS ((cpp_reader *));
    void (*rename_file) PARAMS ((cpp_reader *));
    void (*include) PARAMS ((cpp_reader *, const unsigned char *,
			     const cpp_token *));
    void (*define) PARAMS ((cpp_reader *, cpp_hashnode *));
    void (*undef) PARAMS ((cpp_reader *, cpp_hashnode *));
    void (*poison) PARAMS ((cpp_reader *));
    void (*ident) PARAMS ((cpp_reader *, const cpp_string *));
    void (*def_pragma) PARAMS ((cpp_reader *));
  } cb;

  /* User visible options.  */
  struct cpp_options opts;

  /* Special nodes - identifiers with predefined significance to the
     preprocessor.  */
  struct spec_nodes spec_nodes;

  /* Nonzero means we have printed (while error reporting) a list of
     containing files that matches the current status.  */
  unsigned char input_stack_listing_current;

  /* We're printed a warning recommending against using #import.  */
  unsigned char import_warning;

  /* True after cpp_start_read completes.  Used to inhibit some
     warnings while parsing the command line.  */
  unsigned char done_initializing;

  /* True if we are skipping a failed conditional group.  */
  unsigned char skipping;
};

#define CPP_FATAL_LIMIT 1000
/* True if we have seen a "fatal" error. */
#define CPP_FATAL_ERRORS(READER) ((READER)->errors >= CPP_FATAL_LIMIT)

#define CPP_OPTION(PFILE, OPTION) ((PFILE)->opts.OPTION)
#define CPP_BUFFER(PFILE) ((PFILE)->buffer)
#define CPP_BUF_LINE(BUF) ((BUF)->lineno)
#define CPP_BUF_COLUMN(BUF, CUR) ((CUR) - (BUF)->line_base + (BUF)->col_adjust)
#define CPP_BUF_COL(BUF) CPP_BUF_COLUMN(BUF, (BUF)->cur)

/* Name under which this program was invoked.  */
extern const char *progname;

/* The structure of a node in the hash table.  The hash table has
   entries for all identifiers: either macros defined by #define
   commands (type NT_MACRO), assertions created with #assert
   (NT_ASSERTION), or neither of the above (NT_VOID).  Builtin macros
   like __LINE__ are flagged NODE_BUILTIN.  Poisioned identifiers are
   flagged NODE_POISONED.  NODE_OPERATOR (C++ only) indicates an
   identifier that behaves like an operator such as "xor".
   NODE_DIAGNOSTIC is for speed in lex_token: it indicates a
   diagnostic may be required for this node.  Currently this only
   applies to __VA_ARGS__ and poisoned identifiers.  */

/* Hash node flags.  */
#define NODE_OPERATOR	(1 << 0)	/* C++ named operator.  */
#define NODE_POISONED	(1 << 1)	/* Poisoned identifier.  */
#define NODE_BUILTIN	(1 << 2)	/* Builtin macro.  */
#define NODE_DIAGNOSTIC (1 << 3)	/* Possible diagnostic when lexed.  */

/* Different flavors of hash node.  */
enum node_type
{
  NT_VOID = 0,	   /* No definition yet.  */
  NT_MACRO,	   /* A macro of some form.  */
  NT_ASSERTION	   /* Predicate for #assert.  */
};

/* Different flavors of builtin macro.  */
enum builtin_type
{
  BT_SPECLINE = 0,		/* `__LINE__' */
  BT_DATE,			/* `__DATE__' */
  BT_FILE,			/* `__FILE__' */
  BT_BASE_FILE,			/* `__BASE_FILE__' */
  BT_INCLUDE_LEVEL,		/* `__INCLUDE_LEVEL__' */
  BT_TIME,			/* `__TIME__' */
  BT_STDC			/* `__STDC__' */
};

/* There is a slot in the hashnode for use by front ends when integrated
   with cpplib.  It holds a tree (see tree.h) but we mustn't drag that
   header into every user of cpplib.h.  cpplib does not do anything with
   this slot except clear it when a new node is created.  */
union tree_node;

struct cpp_hashnode
{
  const unsigned char *name;		/* null-terminated name */
  unsigned int hash;			/* cached hash value */
  unsigned short length;		/* length of name excluding null */
  unsigned short arg_index;		/* macro argument index */
  unsigned char directive_index;	/* index into directive table.  */
  ENUM_BITFIELD(node_type) type : 8;	/* node type.  */
  unsigned char flags;			/* node flags.  */

  union
  {
    cpp_macro *macro;			/* a macro.  */
    struct answer *answers;		/* answers to an assertion.  */
    enum cpp_ttype operator;		/* code for a named operator.  */
    enum builtin_type builtin;		/* code for a builtin macro.  */
  } value;

  union tree_node *fe_value;		/* front end value */
};

extern unsigned int cpp_token_len PARAMS ((const cpp_token *));
extern unsigned char *cpp_token_as_text PARAMS ((cpp_reader *, const cpp_token *));
extern unsigned char *cpp_spell_token PARAMS ((cpp_reader *, const cpp_token *,
					       unsigned char *));
extern void cpp_init PARAMS ((void));
extern int cpp_handle_options PARAMS ((cpp_reader *, int, char **));
extern int cpp_handle_option PARAMS ((cpp_reader *, int, char **));
extern void cpp_reader_init PARAMS ((cpp_reader *));

extern void cpp_register_pragma PARAMS ((cpp_reader *,
					 const char *, const char *,
					 void (*) PARAMS ((cpp_reader *))));
extern void cpp_register_pragma_space PARAMS ((cpp_reader *, const char *));

extern int cpp_start_read PARAMS ((cpp_reader *, const char *));
extern void cpp_finish PARAMS ((cpp_reader *));
extern void cpp_cleanup PARAMS ((cpp_reader *));
extern int cpp_avoid_paste PARAMS ((cpp_reader *, const cpp_token *,
				    const cpp_token *));
extern enum cpp_ttype cpp_can_paste PARAMS ((cpp_reader *, const cpp_token *,
					     const cpp_token *, int *));
extern void cpp_get_token PARAMS ((cpp_reader *, cpp_token *));
extern const cpp_lexer_pos *cpp_get_line PARAMS ((cpp_reader *));
extern const unsigned char *cpp_macro_definition PARAMS ((cpp_reader *,
						  const cpp_hashnode *));

extern void cpp_define PARAMS ((cpp_reader *, const char *));
extern void cpp_assert PARAMS ((cpp_reader *, const char *));
extern void cpp_undef  PARAMS ((cpp_reader *, const char *));
extern void cpp_unassert PARAMS ((cpp_reader *, const char *));

extern cpp_buffer *cpp_push_buffer PARAMS ((cpp_reader *,
					    const unsigned char *, long));
extern cpp_buffer *cpp_pop_buffer PARAMS ((cpp_reader *));
extern int cpp_defined PARAMS ((cpp_reader *, const unsigned char *, int));

/* N.B. The error-message-printer prototypes have not been nicely
   formatted because exgettext needs to see 'msgid' on the same line
   as the name of the function in order to work properly.  Only the
   string argument gets a name in an effort to keep the lines from
   getting ridiculously oversized.  */

extern void cpp_ice PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_fatal PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_error PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_warning PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_pedwarn PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_notice PARAMS ((cpp_reader *, const char *msgid, ...))
  ATTRIBUTE_PRINTF_2;
extern void cpp_error_with_line PARAMS ((cpp_reader *, int, int, const char *msgid, ...))
  ATTRIBUTE_PRINTF_4;
extern void cpp_warning_with_line PARAMS ((cpp_reader *, int, int, const char *msgid, ...))
  ATTRIBUTE_PRINTF_4;
extern void cpp_pedwarn_with_line PARAMS ((cpp_reader *, int, int, const char *msgid, ...))
  ATTRIBUTE_PRINTF_4;
extern void cpp_pedwarn_with_file_and_line PARAMS ((cpp_reader *, const char *, int, int, const char *msgid, ...))
  ATTRIBUTE_PRINTF_5;
extern void cpp_error_from_errno PARAMS ((cpp_reader *, const char *));
extern void cpp_notice_from_errno PARAMS ((cpp_reader *, const char *));

/* In cpplex.c */
extern int cpp_ideq			PARAMS ((const cpp_token *,
						 const char *));
extern void cpp_output_line		PARAMS ((cpp_reader *, FILE *));
extern void cpp_output_token		PARAMS ((const cpp_token *, FILE *));
extern const char *cpp_type2name	PARAMS ((enum cpp_ttype));

/* In cpphash.c */
extern cpp_hashnode *cpp_lookup		PARAMS ((cpp_reader *,
						 const unsigned char *, size_t));
extern void cpp_forall_identifiers	PARAMS ((cpp_reader *,
						 int (*) PARAMS ((cpp_reader *,
								  cpp_hashnode *,
								  void *)),
						 void *));

/* In cppmacro.c */
extern void cpp_scan_buffer_nooutput	PARAMS ((cpp_reader *));
extern void cpp_start_lookahead		PARAMS ((cpp_reader *));
extern void cpp_stop_lookahead		PARAMS ((cpp_reader *, int));

/* In cppfiles.c */
extern int cpp_included	PARAMS ((cpp_reader *, const char *));
extern int cpp_read_file PARAMS ((cpp_reader *, const char *));
extern void cpp_make_system_header PARAMS ((cpp_reader *, cpp_buffer *, int));
extern const char *cpp_syshdr_flags PARAMS ((cpp_reader *, cpp_buffer *));

/* These are inline functions instead of macros so we can get type
   checking.  */
typedef unsigned char U_CHAR;
#define U (const U_CHAR *)  /* Intended use: U"string" */

static inline int ustrcmp	PARAMS ((const U_CHAR *, const U_CHAR *));
static inline int ustrncmp	PARAMS ((const U_CHAR *, const U_CHAR *,
					 size_t));
static inline size_t ustrlen	PARAMS ((const U_CHAR *));
static inline U_CHAR *uxstrdup	PARAMS ((const U_CHAR *));
static inline U_CHAR *ustrchr	PARAMS ((const U_CHAR *, int));
static inline int ufputs	PARAMS ((const U_CHAR *, FILE *));

static inline int
ustrcmp (s1, s2)
     const U_CHAR *s1, *s2;
{
  return strcmp ((const char *)s1, (const char *)s2);
}

static inline int
ustrncmp (s1, s2, n)
     const U_CHAR *s1, *s2;
     size_t n;
{
  return strncmp ((const char *)s1, (const char *)s2, n);
}

static inline size_t
ustrlen (s1)
     const U_CHAR *s1;
{
  return strlen ((const char *)s1);
}

static inline U_CHAR *
uxstrdup (s1)
     const U_CHAR *s1;
{
  return (U_CHAR *) xstrdup ((const char *)s1);
}

static inline U_CHAR *
ustrchr (s1, c)
     const U_CHAR *s1;
     int c;
{
  return (U_CHAR *) strchr ((const char *)s1, c);
}

static inline int
ufputs (s, f)
     const U_CHAR *s;
     FILE *f;
{
  return fputs ((const char *)s, f);
}

#ifdef __cplusplus
}
#endif
#endif /* __GCC_CPPLIB__ */
