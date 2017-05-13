#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "bfd.h"
#include "bucomm.h"
#include "demangle.h"

# define gettext(Msgid) (Msgid)
# define _(String) gettext (String)
// #define N_(String) (String)

static int exit_status = 0;

struct objdump_private_desc
{
  /* Help displayed for --help.  */
  void (*help)(FILE *stream);

  /* Return TRUE if these options can be applied to ABFD.  */
  int (*filter)(bfd *abfd);

  /* Do the actual work: display whatever is requested according to the
     options whose SELECTED field is set.  */
  void (*dump)(bfd *abfd);

  /* List of options.  Terminated by a NULL name.  */
  struct objdump_private_option *options;
};

/* The dynamic symbol table.  */
static asymbol **dynsyms;

/* Number of symbols in `dynsyms'.  */
static long dynsymcount = 0;

/* The symbol table.  */
static asymbol **syms;

/* Number of symbols in `syms'.  */
static long symcount = 0;

static bfd_boolean dump_file_header;	/* -f */
static int dump_symtab;			/* -t */
static int dump_ar_hdrs;		/* -a */
static int dump_private_headers;	/* -p */
static char *dump_private_options;	/* -P */
static int do_demangle;			/* -C, --demangle */
static int dump_debugging_tags;		/* --debugging-tags */
static int suppress_bfd_header;
static int dump_special_syms = 0;	/* --special-syms */

struct only
{
  const char * name; /* The name of the section.  */
  bfd_boolean  seen; /* A flag to indicate that the section has been found in one or more input files.  */
  struct only * next; /* Pointer to the next structure in the list.  */
};
  
static struct only * only_list = NULL;

/* Handlers for -P/--private.  */
static const struct objdump_private_desc * const objdump_private_vectors[] =
  {
    //    OBJDUMP_PRIVATE_VECTORS
    NULL
  };

void nonfatal (const char *msg)
{
  bfd_nonfatal (msg);
  exit_status = 1;
}

static bfd_boolean
process_section_p (asection * section)
{
  struct only * only;

  if (only_list == NULL)
    return TRUE;

  for (only = only_list; only; only = only->next)
    if (strcmp (only->name, section->name) == 0)
      {
	only->seen = TRUE;
	return TRUE;
      }

  return FALSE;
}

static void
dump_symbols (bfd *abfd ATTRIBUTE_UNUSED, bfd_boolean dynamic)
{
  asymbol **current;
  long max_count;
  long count;

  if (dynamic)
    {
      current = dynsyms;
      max_count = dynsymcount;
      printf ("DYNAMIC SYMBOL TABLE:\n");
    }
  else
    {
      current = syms;
      max_count = symcount;
      printf ("SYMBOL TABLE:\n");
    }

  if (max_count == 0)
    printf (_("no symbols\n"));

  for (count = 0; count < max_count; count++)
    {
      bfd *cur_bfd;

      if (*current == NULL)
	printf (_("no information for symbol number %ld\n"), count);

      else if ((cur_bfd = bfd_asymbol_bfd (*current)) == NULL)
	printf (_("could not determine the type of symbol number %ld\n"),
		count);

      else if (process_section_p ((* current)->section)
	       && (dump_special_syms
		   || !bfd_is_target_special_symbol (cur_bfd, *current)))
	{
	  const char *name = (*current)->name;

	  if (do_demangle && name != NULL && *name != '\0')
	    {
	      char *alloc;

	      /* If we want to demangle the name, we demangle it
		 here, and temporarily clobber it while calling
		 bfd_print_symbol.  FIXME: This is a gross hack.  */
	      alloc = bfd_demangle (cur_bfd, name, DMGL_ANSI | DMGL_PARAMS);
	      if (alloc != NULL)
		(*current)->name = alloc;
	      bfd_print_symbol (cur_bfd, stdout, *current,
				bfd_print_symbol_all);
	      if (alloc != NULL)
		{
		  (*current)->name = name;
		  free (alloc);
		}
	    }
	  else
	    bfd_print_symbol (cur_bfd, stdout, *current,
			      bfd_print_symbol_all);
	  printf ("\n");
	}

      current++;
    }
  printf ("\n\n");
}

static void
adjust_addresses (bfd *abfd ATTRIBUTE_UNUSED,
		  asection *section,
		  void *arg)
{
  if ((section->flags & SEC_DEBUGGING) == 0) {
      bfd_boolean *has_reloc_p = (bfd_boolean *) arg;
    }
}

static void dump_bfd_header (bfd *abfd)
{
  char *comma = "";
  printf (_("architecture: %s, "),
	  bfd_printable_arch_mach(bfd_get_arch (abfd),
				  bfd_get_mach (abfd)));
  printf (_("flags 0x%08x:\n"),
	  abfd->flags & ~BFD_FLAGS_FOR_BFD_USE_MASK);

#define PF(x, y)    if (abfd->flags & x) {printf("%s%s", comma, y); comma=", ";}
  PF (HAS_RELOC, "HAS_RELOC");
  PF (EXEC_P, "EXEC_P");
  PF (HAS_LINENO, "HAS_LINENO");
  PF (HAS_DEBUG, "HAS_DEBUG");
  PF (HAS_SYMS, "HAS_SYMS");
  PF (HAS_LOCALS, "HAS_LOCALS");
  PF (DYNAMIC, "DYNAMIC");
  PF (WP_TEXT, "WP_TEXT");
  PF (D_PAGED, "D_PAGED");
  PF (BFD_IS_RELAXABLE, "BFD_IS_RELAXABLE");
  printf (_("\nstart address 0x"));
  bfd_printf_vma (abfd, abfd->start_address);
  printf ("\n");
  fflush(stdout);
}

static void
dump_bfd_private_header (bfd *abfd)
{
  bfd_print_private_bfd_data (abfd, stdout);
}

static void
dump_target_specific (bfd *abfd)
{
  const struct objdump_private_desc * const *desc;
  struct objdump_private_option *opt;
  char *e, *b;

  /* Find the desc.  */
  for (desc = objdump_private_vectors; *desc != NULL; desc++)
    if ((*desc)->filter (abfd))
      break;

  if (*desc == NULL)
    {
      non_fatal (_("option -P/--private not supported by this file"));
      return;
    }

  /* Clear all options.  */
  /* for (opt = (*desc)->options; opt->name; opt++) */
  /*   opt->selected = FALSE; */

  /* Decode options.  */
  b = dump_private_options;
  do
    {
      e = strchr (b, ',');

      if (e)
        *e = 0;

      /* for (opt = (*desc)->options; opt->name; opt++) */
      /*   if (strcmp (opt->name, b) == 0) */
      /*     { */
      /*       opt->selected = TRUE; */
      /*       break; */
      /*     } */
      /* if (opt->name == NULL) */
      /*   non_fatal (_("target specific dump '%s' not supported"), b); */

      if (e)
        {
          *e = ',';
          b = e + 1;
        }
    }
  while (e != NULL);

  /* Dump.  */
  (*desc)->dump (abfd);
}

static asymbol ** slurp_symtab (bfd *abfd) {
  asymbol **sy = NULL;
  long storage;
  if (!(bfd_get_file_flags(abfd) & HAS_SYMS)) {
    
      printf("what no symbols? %x, %d\n", abfd, abfd->flags);
    symcount = 0;
    return NULL;
  }
  storage = bfd_get_symtab_upper_bound (abfd);
  if (storage < 0) {
    non_fatal (_("failed to read symbol table from: %s"), bfd_get_filename (abfd));
    bfd_fatal (_("error message was"));
  }
  if (storage) {
    sy = (asymbol **) xmalloc (storage);
  }
  symcount = bfd_canonicalize_symtab (abfd, sy);
  if (symcount < 0) {
    bfd_fatal (bfd_get_filename (abfd));
  }
  return sy;
}

static void dump_bfd (bfd *abfd) {
  if (! dump_debugging_tags && ! suppress_bfd_header)
    printf (_("\n%s:     file format %s\n"), bfd_get_filename (abfd),
	    abfd->xvec->name);
  if (dump_ar_hdrs)
    print_arelt_descr (stdout, abfd, TRUE);
  if (dump_file_header)
    dump_bfd_header (abfd);
  if (dump_private_headers)
    dump_bfd_private_header (abfd);
  if (dump_private_options != NULL)
    dump_target_specific (abfd);
  if (! dump_debugging_tags && ! suppress_bfd_header)
    putchar ('\n');

  syms = slurp_symtab (abfd);

  /* if (dump_section_headers) */
  /*   dump_headers (abfd); */

  /* if (dump_dynamic_symtab || dump_dynamic_reloc_info */
  /*     || (disassemble && bfd_get_dynamic_symtab_upper_bound (abfd) > 0)) */
  /*   dynsyms = slurp_dynamic_symtab (abfd); */
  /* if (disassemble) */
  /*   { */
  /*     synthcount = bfd_get_synthetic_symtab (abfd, symcount, syms, */
  /* 					     dynsymcount, dynsyms, &synthsyms); */
  /*     if (synthcount < 0) */
  /* 	synthcount = 0; */
  /*   } */

  dump_symbols (abfd, FALSE);
  dump_symbols (abfd, TRUE);
  /* if (dump_dwarf_section_info) */
  /*   dump_dwarf (abfd); */
  /* if (dump_stab_section_info) */
  /*   dump_stabs (abfd); */
  /* if (dump_reloc_info && ! disassemble) */
  /*   dump_relocs (abfd); */
  /* if (dump_dynamic_reloc_info && ! disassemble) */
  /*   dump_dynamic_relocs (abfd); */
  /* if (dump_section_contents) */
  /*   dump_data (abfd); */
  /* if (disassemble) */
  /*   disassemble_data (abfd); */

  /* if (dump_debugging) */
  /*   { */
  /*     void *dhandle; */

  /*     dhandle = read_debugging_info (abfd, syms, symcount, TRUE); */
  /*     if (dhandle != NULL) */
  /* 	{ */
  /* 	  if (!print_debugging_info (stdout, dhandle, abfd, syms, */
  /* 				     bfd_demangle, */
  /* 				     dump_debugging_tags ? TRUE : FALSE)) */
  /* 	    { */
  /* 	      non_fatal (_("%s: printing debugging information failed"), */
  /* 			 bfd_get_filename (abfd)); */
  /* 	      exit_status = 1; */
  /* 	    } */
  /* 	} */
  /*     /\* PR 6483: If there was no STABS or IEEE debug */
  /* 	 info in the file, try DWARF instead.  *\/ */
  /*     else if (! dump_dwarf_section_info) */
  /* 	{ */
  /* 	  dwarf_select_sections_all (); */
  /* 	  dump_dwarf (abfd); */
  /* 	} */
  /*   } */

  if (syms)
    {
      free (syms);
      syms = NULL;
    }

  if (dynsyms)
    {
      free (dynsyms);
      dynsyms = NULL;
    }

  /* if (synthsyms) */
  /*   { */
  /*     free (synthsyms); */
  /*     synthsyms = NULL; */
  /*   } */

  /* symcount = 0; */
  /* dynsymcount = 0; */
  /* synthcount = 0; */
}

static void display_any_bfd (bfd *file, int level) {
  /* If the file is an archive, process all of its elements.  */
  if (bfd_check_format (file, bfd_archive)) {
    bfd *arfile = NULL;
    bfd *last_arfile = NULL;
    if (level == 0) {
      printf (_("In archive %s:\n"), bfd_get_filename (file));
    } else if (level > 100) {
      /* Prevent corrupted files from spinning us into an
	 infinite loop.  100 is an arbitrary heuristic.  */
      fatal (_("Archive nesting is too deep"));
      return;
    } else {
      printf (_("In nested archive %s:\n"), bfd_get_filename (file));
    }
    for (;;) {
      bfd_set_error (bfd_error_no_error);
      arfile = bfd_openr_next_archived_file (file, arfile);
      if (arfile == NULL) {
	if (bfd_get_error () != bfd_error_no_more_archived_files) {
	  nonfatal (bfd_get_filename (file));
	}
	break;
      }
      display_any_bfd(arfile, level + 1);
      if (last_arfile != NULL) {
	bfd_close (last_arfile);
	/* PR 17512: file: ac585d01.  */
	if (arfile == last_arfile) {
	  last_arfile = NULL;
	  break;
	}
      }
      last_arfile = arfile;
    }
    if (last_arfile != NULL) {
      bfd_close (last_arfile);
    }
  } else {
    dump_bfd(file);
  }
}

#include <sys/stat.h>

off_t fsize(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0)
    return st.st_size;
  return -1;
}

static void display_file (char *filename) {
  bfd *file;
  if (get_file_size (filename) < 1) {
    exit_status = 1;
    return;
  }
  file = bfd_openr (filename, NULL);
  if (file == NULL) {
    printf("Unable to open file: %s", filename);
    return;
  }
  display_any_bfd (file, 0);
  bfd_close_all_done (file);
}

void main () {
  char filename [] =
    "/home/rett/dev/xoamorph/xara-lx/libs/x86_64/libCDraw_la-bshadow2-new.o";
  display_file(filename); 
  return;
}
