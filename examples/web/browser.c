/* Thanks to Dominic Letz for this example */

#include <stdlib.h>
#include "../../syx/syx.h"

int main (int argc, char *argv[])
{
   SyxOop context;
   SyxOop process;
   SyxOop instance;

   /* initialize Syx */
   syx_init (argc, argv, NULL);

   /* load the default image */
   syx_memory_load_image (NULL);

   /* now file in class and method declarations from our ST file */
   syx_file_in_blocking ("coldbrowser.st");

   /* create a WebBrowser instance */
   instance = syx_object_new(syx_globals_at("WebBrowser"));

   /* create a Process */
   process = syx_process_new ();

   /* create a MethodContext which sends the #with:and: message */
   context = syx_send_binary_message(instance, // the receiver
				     "run:", // the selector
				     syx_string_new(getenv("QUERY_STRING")));

   /* setup the new frame in the process */
   syx_interp_enter_context (process, context);

   /* execute the process in blocking mode */
   syx_process_execute_blocking (process);

   /* cleanup Syx */
   syx_quit ();

   return 0;
} 
