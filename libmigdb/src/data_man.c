/**[txh]********************************************************************

  Copyright (c) 2004 by Salvador E. Tropea.
  Covered by the GPL license.

  Module: Data manipulation.
  Comments:
  GDB/MI commands for the "Data manipulation" section.@p

@<pre>
gdb command:                       Implemented?

-data-disassemble                  No
-data-evaluate-expression          Yes
-data-list-changed-registers       No
-data-list-register-names          No
-data-list-register-values         No
-data-read-memory                  No
-display-delete                    N.A. (delete display)
-display-disable                   N.A. (disable display)
-display-enable                    N.A. (enable display)
-display-insert                    N.A. (display)
-display-list                      N.A. (info display)
-environment-cd                    No
-environment-directory             Yes, MI v1 implementation
-environment-path                  No
@</pre>

Notes:@p

1) -display* aren't implemented. You can use CLI command display, but the
results are sent to the console. So it looks like the best is to manually
use -data-evaluate-expression to emulate it.@p

***************************************************************************/

#include "mi_gdb.h"

/* Low level versions. */

void mi_data_evaluate_expression(mi_h *h, const char *expression)
{
 mi_send(h,"-data-evaluate-expression \"%s\"\n",expression);
}

void mi_dir(mi_h *h, const char *path)
{
 if (0)
   {// MI v2
    if (path)
       mi_send(h,"-environment-directory \"%s\"\n",path);
    else
       mi_send(h,"-environment-directory -r\n");
   }
 else
   {
    mi_send(h,"-environment-directory %s\n",path ? path : "");
   }
}

/* High level versions. */

/**[txh]********************************************************************

  Description:
  Evaluate an expression. Returns a parsed tree.

  Command: -data-evaluate-expression
  Return: The resulting value (as plain text) or NULL on error.
  
***************************************************************************/

char *gmi_data_evaluate_expression(mi_h *h, const char *expression)
{
 mi_data_evaluate_expression(h,expression);
 return mi_res_value(h);
}

/**[txh]********************************************************************

  Description:
  Path for sources. You must use it to indicate where are the sources for
the program to debug. Only the MI v1 implementation is available.

  Command: -environment-directory
  Return: !=0 OK
  
***************************************************************************/

int gmi_dir(mi_h *h, char *path)
{
 mi_dir(h,path);
 return mi_res_simple_done(h);
}
