/* 
   Copyright (c) 2007-2008 Luca Bruno

   This file is part of Smalltalk YX.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell   
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER    
   DEALINGS IN THE SOFTWARE.
*/

#include "syx/syx.h"
#include <gtk/gtk.h>
#include "syx-gobject.h"

static SyxOop _syx_gtk_process, _syx_gtk_semaphore;

static syx_bool
_syx_gtk_iteration (void)
{
  gtk_main_iteration_do (FALSE);
  return FALSE;
}

EXPORT void syx_g_closure_marshal (GClosure *closure,
                                   GValue *return_value,
                                   guint n_param_values,
                                   const GValue *param_values,
                                   gpointer invocation_hint,
                                   gpointer marshal_data)
{
  syx_memory_gc_begin ();
  SyxOop array = syx_array_new_size (n_param_values);
  SyxOop context;
  SyxOop callback = (SyxOop) closure->data;
  syx_uint32 i;
  const GValue *v;
  syx_symbol name;
  SyxOop value;
  SyxOop oop;
  GType t;

  for (i=0; i < n_param_values; i++)
    {
      v = param_values+i;
      t = G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (v));

      switch (t)
	{
	case G_TYPE_INVALID:
	case G_TYPE_NONE:
	  SYX_OBJECT_DATA(array)[i] = syx_nil;
	  break;
	case G_TYPE_CHAR:
	case G_TYPE_UCHAR:
	  SYX_OBJECT_DATA(array)[i] = syx_character_new (g_value_get_char (v));
	  break;
	case G_TYPE_BOOLEAN:
	  SYX_OBJECT_DATA(array)[i] = syx_boolean_new (g_value_get_boolean (v));
	  break;
	case G_TYPE_INT:
	case G_TYPE_UINT:
	case G_TYPE_LONG:
	case G_TYPE_ULONG:
	case G_TYPE_INT64:
	case G_TYPE_ENUM:
	case G_TYPE_FLAGS:
	  SYX_OBJECT_DATA(array)[i] = syx_small_integer_new (g_value_get_int (v));
	  break;
	case G_TYPE_FLOAT:
	case G_TYPE_DOUBLE:
	  SYX_OBJECT_DATA(array)[i] = syx_float_new (g_value_get_double (v));
	  break;
	case G_TYPE_STRING:
	  SYX_OBJECT_DATA(array)[i] = syx_string_new (g_value_get_string (v));
	  break;
	case G_TYPE_POINTER:
	case G_TYPE_PARAM:
	  SYX_OBJECT_DATA(array)[i] = SYX_POINTER_CAST_OOP (g_value_get_pointer (v));
	  break;	  
	case G_TYPE_BOXED:
	  value = SYX_POINTER_CAST_OOP (g_value_get_boxed (v));
	case G_TYPE_OBJECT:
	  value = SYX_POINTER_CAST_OOP (g_value_get_object (v));
	  /* get the class name */
	  name = G_VALUE_TYPE_NAME (v);
	  /* lookup the class and create the object */
	  oop = syx_object_new (syx_globals_at (name));
	  /* set the first instance variable (handle) to hold the CPointer */
	  SYX_OBJECT_VARS(oop)[0] = value;
	  
	  SYX_OBJECT_DATA(array)[i] = oop;
	  break;
	}
    }

  context = syx_send_binary_message (callback, "invoke:", array);
  syx_interp_enter_context (_syx_gtk_process, context);
  syx_semaphore_signal (_syx_gtk_semaphore);
  syx_memory_gc_end ();

  while (syx_scheduler_iterate () && SYX_IS_FALSE (SYX_PROCESS_SUSPENDED (_syx_gtk_process)));

  return;
}

SYX_FUNC_PRIMITIVE(Gtk_main)
{
  static syx_bool registered = FALSE;
  SYX_PRIM_ARGS (2);

  if (!registered)
    {
      syx_scheduler_poll_register_source (_syx_gtk_iteration, syx_nil);
      _syx_gtk_process = es->message_arguments[0];
      _syx_gtk_semaphore = es->message_arguments[1];
      registered = TRUE;
    }

  SYX_PRIM_RETURN (es->message_receiver);
}

SYX_FUNC_PRIMITIVE(Gtk_mainQuit)
{
  syx_scheduler_poll_unregister_source (_syx_gtk_iteration, syx_nil);
  syx_scheduler_remove_process (_syx_gtk_process);

  SYX_PRIM_YIELD(es->message_receiver);
}

EXPORT syx_bool
syx_plugin_initialize (void)
{
  static syx_bool _syx_gtk_initialized = FALSE;
  syx_symbol *filename;
  syx_string full_filename;
  static syx_symbol gtk_filenames[] = {
    "GObject.st", "Gtk.st",
    "GtkWidget.st", "GtkLabel.st", "GtkContainer.st",
    "GtkWindow.st", "GtkButton.st", "GtkTools.st", "GtkBox.st",
    "GtkAdjustment.st", "GtkScrolledWindow.st",
    "GtkTextIter.st", "GtkTextView.st", "GtkTextBuffer.st", "GtkTextMark.st",
    "GtkTextTag.st", "GtkTextTagTable.st",
    NULL
  };

  if (_syx_gtk_initialized)
    return TRUE;

  _syx_gtk_initialized = TRUE;

  for (filename = gtk_filenames; *filename; filename++)
    {
      full_filename = syx_find_file ("st", "gtk", *filename);
      syx_file_in_blocking (full_filename);
      syx_free (full_filename);
    }

  gtk_init (0, NULL);

  return TRUE;
}

EXPORT void
syx_plugin_finalize (void)
{
}
