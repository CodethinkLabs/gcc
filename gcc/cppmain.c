/* CPP main program, using CPP Library.
   Copyright (C) 1995, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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

#include "config.h"
#include "system.h"
#include "cpplib.h"
#include "intl.h"

/* Encapsulates state used to convert the stream of tokens coming from
   cpp_get_token back into a text file.  */
struct printer
{
  FILE *outf;			/* stream to write to.  */
  const char *last_fname;	/* previous file name.  */
  const char *syshdr_flags;	/* system header flags, if any.  */
  unsigned int lineno;		/* line currently being written.  */
  unsigned char printed;	/* nonzero if something output at lineno.  */
  unsigned char no_line_dirs;	/* nonzero to output no line directives.  */
};

int main		PARAMS ((int, char **));
static void general_init PARAMS ((const char *));
static void setup_callbacks PARAMS ((void));

/* General output routines.  */
static void scan_buffer	PARAMS ((cpp_reader *));
static int printer_init PARAMS ((cpp_reader *));
static int dump_macro PARAMS ((cpp_reader *, cpp_hashnode *, void *));

static void print_line PARAMS ((const char *));
static void maybe_print_line PARAMS ((unsigned int));

/* Callback routines for the parser.   Most of these are active only
   in specific modes.  */
static void cb_define	PARAMS ((cpp_reader *, cpp_hashnode *));
static void cb_undef	PARAMS ((cpp_reader *, cpp_hashnode *));
static void cb_include	PARAMS ((cpp_reader *, const unsigned char *,
				 const cpp_token *));
static void cb_ident	  PARAMS ((cpp_reader *, const cpp_string *));
static void cb_change_file PARAMS ((cpp_reader *, const cpp_file_change *));
static void cb_def_pragma PARAMS ((cpp_reader *));
static void do_pragma_implementation PARAMS ((cpp_reader *));

const char *progname;		/* Needs to be global.  */
static cpp_reader *pfile;
static struct printer print;

int
main (argc, argv)
     int argc;
     char **argv;
{
  int argi = 1;  /* Next argument to handle.  */

  general_init (argv[0]);
  /* Default language is GNU C89.  */
  pfile = cpp_create_reader (CLK_GNUC89);
  
  argi += cpp_handle_options (pfile, argc - argi , argv + argi);
  if (argi < argc && ! CPP_FATAL_ERRORS (pfile))
    cpp_fatal (pfile, "Invalid option %s", argv[argi]);
  if (CPP_FATAL_ERRORS (pfile))
    return (FATAL_EXIT_CODE);

  /* Open the output now.  We must do so even if no_output is on,
     because there may be other output than from the actual
     preprocessing (e.g. from -dM).  */
  if (printer_init (pfile))
    return (FATAL_EXIT_CODE);

  setup_callbacks ();

  if (! cpp_start_read (pfile, CPP_OPTION (pfile, in_fname)))
    return (FATAL_EXIT_CODE);

  if (CPP_BUFFER (pfile))
    {
      if (CPP_OPTION (pfile, no_output))
	cpp_scan_buffer_nooutput (pfile, 1);
      else
	scan_buffer (pfile);
    }

  /* -dM command line option.  */
  if (CPP_OPTION (pfile, dump_macros) == dump_only)
    cpp_forall_identifiers (pfile, dump_macro, NULL);

  cpp_finish (pfile);
  cpp_cleanup (pfile);

  /* Flush any pending output.  */
  if (print.printed)
    putc ('\n', print.outf);
  if (ferror (print.outf) || fclose (print.outf))
    cpp_notice_from_errno (pfile, CPP_OPTION (pfile, out_fname));

  if (pfile->errors)
    return (FATAL_EXIT_CODE);
  return (SUCCESS_EXIT_CODE);
}

/* Store the program name, and set the locale.  */
static void
general_init (const char *argv0)
{
  progname = argv0 + strlen (argv0);

  while (progname != argv0 && ! IS_DIR_SEPARATOR (progname[-1]))
    --progname;

  xmalloc_set_program_name (progname);

#ifdef HAVE_LC_MESSAGES
  setlocale (LC_MESSAGES, "");
#endif
  (void) bindtextdomain (PACKAGE, localedir);
  (void) textdomain (PACKAGE);
}

/* Set up the callbacks and register the pragmas we handle.  */
static void
setup_callbacks ()
{
  /* Set callbacks.  */
  if (! CPP_OPTION (pfile, no_output))
    {
      pfile->cb.ident      = cb_ident;
      pfile->cb.def_pragma = cb_def_pragma;
      if (! CPP_OPTION (pfile, no_line_commands))
	pfile->cb.change_file = cb_change_file;
    }

  if (CPP_OPTION (pfile, dump_includes))
    pfile->cb.include  = cb_include;

  if (CPP_OPTION (pfile, debug_output)
      || CPP_OPTION (pfile, dump_macros) == dump_names
      || CPP_OPTION (pfile, dump_macros) == dump_definitions)
    {
      pfile->cb.define = cb_define;
      pfile->cb.undef  = cb_undef;
      pfile->cb.poison = cb_def_pragma;
    }

  /* Register one #pragma which needs special handling.  */
  cpp_register_pragma(pfile, 0, "implementation", do_pragma_implementation);
  cpp_register_pragma(pfile, "GCC", "implementation", do_pragma_implementation);
}

/* Writes out the preprocessed file.  Alternates between two tokens,
   so that we can avoid accidental token pasting.  */
static void
scan_buffer (pfile)
     cpp_reader *pfile;
{
  unsigned int index, line;
  cpp_token tokens[2], *token;

  do
    {
      for (index = 0;; index = 1 - index)
	{
	  token = &tokens[index];
	  cpp_get_token (pfile, token);

	  if (token->type == CPP_EOF)
	    break;

	  line = cpp_get_line (pfile)->output_line;
	  if (print.lineno != line)
	    {
	      unsigned int col = cpp_get_line (pfile)->col;

	      /* Supply enough whitespace to put this token in its original
		 column.  Don't bother trying to reconstruct tabs; we can't
		 get it right in general, and nothing ought to care.  (Yes,
		 some things do care; the fault lies with them.)  */
	      maybe_print_line (line);
	      if (col > 1)
		{
		  if (token->flags & PREV_WHITE)
		    col--;
		  while (--col)
		    putc (' ', print.outf);
		}
	    }
	  else if (print.printed
		   && ! (token->flags & PREV_WHITE)
		   && CPP_OPTION (pfile, lang) != CLK_ASM
		   && cpp_avoid_paste (pfile, &tokens[1 - index], token))
	    token->flags |= PREV_WHITE;

	  cpp_output_token (token, print.outf);
	  print.printed = 1;
	}
    }
  while (cpp_pop_buffer (pfile) != 0);
}

/* Initialize a cpp_printer structure.  As a side effect, open the
   output file.  */
static int
printer_init (pfile)
     cpp_reader *pfile;
{
  print.last_fname = 0;
  print.lineno = 0;
  print.printed = 0;
  print.no_line_dirs = CPP_OPTION (pfile, no_line_commands);

  if (CPP_OPTION (pfile, out_fname) == NULL)
    CPP_OPTION (pfile, out_fname) = "";
  
  if (CPP_OPTION (pfile, out_fname)[0] == '\0')
    print.outf = stdout;
  else
    {
      print.outf = fopen (CPP_OPTION (pfile, out_fname), "w");
      if (! print.outf)
	{
	  cpp_notice_from_errno (pfile, CPP_OPTION (pfile, out_fname));
	  return 1;
	}
    }
  return 0;
}

/* Newline-terminate any output line currently in progress.  If
   appropriate, write the current line number to the output, or pad
   with newlines so the output line matches the current line.  */
static void
maybe_print_line (line)
     unsigned int line;
{
  /* End the previous line of text (probably only needed until we get
     multi-line tokens fixed).  */
  if (print.printed)
    {
      putc ('\n', print.outf);
      print.lineno++;
      print.printed = 0;
    }

  if (print.no_line_dirs)
    return;

  /* print.lineno is zero if this is the first token of the file.  We
     handle this specially, so that a first line of "# 1 "foo.c" in
     file foo.i outputs just the foo.c line, and not a foo.i line.  */
  if (line >= print.lineno && line < print.lineno + 8 && print.lineno)
    {
      while (line > print.lineno)
	{
	  putc ('\n', print.outf);
	  print.lineno++;
	}
    }
  else
    {
      print.lineno = line;
      print_line ("");
    }
}

static void
print_line (special_flags)
  const char *special_flags;
{
  /* End any previous line of text.  */
  if (print.printed)
    putc ('\n', print.outf);
  print.printed = 0;

  fprintf (print.outf, "# %u \"%s\"%s%s\n",
	   print.lineno, print.last_fname, special_flags, print.syshdr_flags);
}

/* Callbacks */

static void
cb_ident (pfile, str)
     cpp_reader *pfile ATTRIBUTE_UNUSED;
     const cpp_string * str;
{
  maybe_print_line (cpp_get_line (pfile)->output_line);
  fprintf (print.outf, "#ident \"%.*s\"\n", (int) str->len, str->text);
  print.lineno++;
}

static void
cb_define (pfile, node)
     cpp_reader *pfile;
     cpp_hashnode *node;
{
  if (pfile->done_initializing)
    {
      maybe_print_line (cpp_get_line (pfile)->output_line);
      fprintf (print.outf, "#define %s", node->name);

      /* -dD or -g3 command line options.  */
      if (CPP_OPTION (pfile, debug_output)
	  || CPP_OPTION (pfile, dump_macros) == dump_definitions)
	fputs ((const char *) cpp_macro_definition (pfile, node), print.outf);

      putc ('\n', print.outf);
      print.lineno++;
    }
}

static void
cb_undef (pfile, node)
     cpp_reader *pfile;
     cpp_hashnode *node;
{
  if (pfile->done_initializing)
    {
      maybe_print_line (cpp_get_line (pfile)->output_line);
      fprintf (print.outf, "#undef %s\n", node->name);
      print.lineno++;
    }
}

static void
cb_include (pfile, dir, header)
     cpp_reader *pfile ATTRIBUTE_UNUSED;
     const unsigned char *dir;
     const cpp_token *header;
{
  maybe_print_line (cpp_get_line (pfile)->output_line);
  fprintf (print.outf, "#%s %s\n", dir, cpp_token_as_text (pfile, header));
  print.lineno++;
}

static void
cb_change_file (pfile, fc)
     cpp_reader *pfile ATTRIBUTE_UNUSED;
     const cpp_file_change *fc;
{
  const char *flags;

  /* Bring current file to correct line (except first file).  */
  if (fc->reason == FC_ENTER && fc->from.filename)
    maybe_print_line (fc->from.lineno);

  print.last_fname = fc->to.filename;
  if (fc->externc)
    print.syshdr_flags = " 3 4";
  else if (fc->sysp)
    print.syshdr_flags = " 3";
  else
    print.syshdr_flags = "";

  if (print.lineno)
    {
      print.lineno = fc->to.lineno;
      switch (fc->reason)
	{
	case FC_ENTER : flags = " 1"; break;
	case FC_LEAVE : flags = " 2"; break;
	case FC_RENAME: flags = ""; break;
	}

      print_line (flags);
    }
}

static void
cb_def_pragma (pfile)
     cpp_reader *pfile;
{
  maybe_print_line (cpp_get_line (pfile)->output_line);
  fputs ("#pragma ", print.outf);
  cpp_output_line (pfile, print.outf);
  print.lineno++;
}

static void
do_pragma_implementation (pfile)
     cpp_reader *pfile;
{
  /* Be quiet about `#pragma implementation' for a file only if it hasn't
     been included yet.  */
  cpp_token token;

  cpp_start_lookahead (pfile);
  cpp_get_token (pfile, &token);
  cpp_stop_lookahead (pfile, 0);

  /* If it's not a string, pass it through and let the front end complain.  */
  if (token.type == CPP_STRING)
    {
     /* Make a NUL-terminated copy of the string.  */
      char *filename = alloca (token.val.str.len + 1);
      memcpy (filename, token.val.str.text, token.val.str.len);
      filename[token.val.str.len] = '\0';
      if (cpp_included (pfile, filename))
	cpp_warning (pfile,
	     "#pragma GCC implementation for \"%s\" appears after file is included",
		     filename);
    }
  else if (token.type != CPP_EOF)
    {
      cpp_error (pfile, "malformed #pragma GCC implementation");
      return;
    }

  /* Output?  This is nasty, but we don't have [GCC] implementation in
     the buffer.  */
  if (pfile->cb.def_pragma)
    {
      maybe_print_line (cpp_get_line (pfile)->output_line);
      fputs ("#pragma GCC implementation ", print.outf);
      cpp_output_line (pfile, print.outf);
      print.lineno++;
    }
}

/* Dump out the hash table.  */
static int
dump_macro (pfile, node, v)
     cpp_reader *pfile;
     cpp_hashnode *node;
     void *v ATTRIBUTE_UNUSED;
{
  if (node->type == NT_MACRO && !(node->flags & NODE_BUILTIN))
    {
      fprintf (print.outf, "#define %s", node->name);
      fputs ((const char *) cpp_macro_definition (pfile, node), print.outf);
      putc ('\n', print.outf);
      print.lineno++;
    }

  return 1;
}
