/**[txh]********************************************************************

  Copyright (c) 2004 by Salvador E. Tropea.
  Covered by the GPL license.

  Comments:
  Main header for libmigdb.
  
***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#define MI_OK                      0
#define MI_OUT_OF_MEMORY           1
#define MI_PIPE_CREATE             2
#define MI_FORK                    3
#define MI_DEBUGGER_RUN            4
#define MI_PARSER                  5
#define MI_UNKNOWN_ASYNC           6
#define MI_UNKNOWN_RESULT          7
#define MI_FROM_GDB                8
#define MI_LAST_ERROR              8

#define MI_R_NONE                  0 /* We are no waiting any response. */
#define MI_R_SKIP                  1 /* We want to discard it. */
#define MI_R_FE_AND_S              2 /* Wait for done. */
#define MI_R_E_ARGS                3

enum mi_val_type { t_const, t_tuple, t_list };

/* Types and subtypes. */
/* Type. */
#define MI_T_OUT_OF_BAND   0
#define MI_T_RESULT_RECORD 1
/* Out of band subtypes. */
#define MI_ST_ASYNC        0
#define MI_ST_STREAM       1
/* Async sub-subtypes. */
#define MI_SST_EXEC        0
#define MI_SST_STATUS      1
#define MI_SST_NOTIFY      2
/* Stream sub-subtypes. */
#define MI_SST_CONSOLE     3
#define MI_SST_TARGET      4
#define MI_SST_LOG         5
/* Classes. */
/* Async classes. */
#define MI_CL_UNKNOWN      0
#define MI_CL_STOPPED      1
/* Result classes. */
#define MI_CL_DONE         2
#define MI_CL_RUNNING      3
#define MI_CL_CONNECTED    4
#define MI_CL_ERROR        5
#define MI_CL_EXIT         6

#define MI_VERSION_STR "0.8.1"
#define MI_VERSION_MAJOR  0
#define MI_VERSION_MIDDLE 8
#define MI_VERSION_MINOR  1

struct mi_results_struct
{
 char *var; /* Result name or NULL if just a value. */
 enum mi_val_type type;
 union
 {
  char *cstr;
  struct mi_results_struct *rs;
 } v;
 struct mi_results_struct *next;
};
typedef struct mi_results_struct mi_results;

struct mi_output_struct
{
 /* Type of output. */
 char type;
 char stype;
 char sstype;
 char tclass;
 /* Content. */
 mi_results *c;
 /* Always modeled as a list. */
 struct mi_output_struct *next;
};
typedef struct mi_output_struct mi_output;

typedef void (*stream_cb)(const char *, void *);
typedef void (*async_cb)(mi_output *o, void *);

/* Values of this structure shouldn't be manipulated by the user. */
struct mi_h_struct
{
 /* Pipes connected to gdb. */
 int to_gdb[2];
 int from_gdb[2];
 /* Streams for the pipes. */
 FILE *to, *from;
 /* PID of child gdb. */
 pid_t pid;
 /* Which rensponse we are waiting for. */
 /*int response;*/
 /* The line we are reading. */
 char *line;
 int   llen, lread;
 /* Parsed output. */
 mi_output *po, *last;
 /* Tunneled streams callbacks. */
 stream_cb console;
 void *console_data;
 stream_cb target;
 void *target_data;
 stream_cb log;
 void *log_data;
 /* Async responses callback. */
 async_cb async;
 void *async_data;
 /* Callbacks to get echo of gdb dialog. */
 stream_cb to_gdb_echo;
 void *to_gdb_echo_data;
 stream_cb from_gdb_echo;
 void *from_gdb_echo_data;
};
typedef struct mi_h_struct mi_h;

#define MI_TO(a) ((a)->to_gdb[1])

enum mi_bkp_type { t_unknown=0, t_breakpoint=1, t_hw=2 };
enum mi_bkp_disp { d_unknown=0, d_keep=1, d_del=2 };
enum mi_bkp_mode { m_file_line=0, m_function=1, m_file_function=2, m_address=3 };

struct mi_bkpt_struct
{
 int number;
 enum mi_bkp_type type;
 enum mi_bkp_disp disp; /* keep or del if temporal */
 char enabled;
 void *addr;
 char *func;
 char *file;
 int line;
 int ignore;
 int times;

 /* For the user: */
 char *cond;
 char *file_abs;
 int thread;
 enum mi_bkp_mode mode;
 struct mi_bkpt_struct *next;
};
typedef struct mi_bkpt_struct mi_bkpt;

enum mi_wp_mode { wm_unknown=0, wm_write=1, wm_read=2, wm_rw=3 };

struct mi_wp_struct
{
 int number;
 char *exp;
 enum mi_wp_mode mode;

 /* For the user: */
 struct mi_wp_struct *next;
 char enabled;
};
typedef struct mi_wp_struct mi_wp;

struct mi_frames_struct
{
 int level;  /* The frame number, 0 being the topmost frame, i.e. the innermost
                function. */
 void *addr; /* The `$pc' value for that frame. */
 char *func; /* Function name. */
 char *file; /* File name of the source file where the function lives. */
 int line;   /* Line number corresponding to the `$pc'. */
 /* When arguments are available: */
 mi_results *args;
 /* When more than one is provided: */
 struct mi_frames_struct *next;
};
typedef struct mi_frames_struct mi_frames;

struct mi_aux_term_struct
{
 pid_t pid;
 char *tty;
};
typedef struct mi_aux_term_struct mi_aux_term;

enum mi_gvar_fmt { fm_natural=0, fm_binary, fm_decimal, fm_hexadecimal, fm_octal };
enum mi_gvar_lang { lg_unknown=0, lg_c, lg_cpp, lg_java };

#define MI_ATTR_NONEDITABLE 0
#define MI_ATTR_EDITABLE    1

struct mi_gvar_struct
{
 char *name;
 int   numchild;
 char *type;
 enum mi_gvar_fmt format;
 enum mi_gvar_lang lang;
 char *expression;
 int   attr;
};
typedef struct mi_gvar_struct mi_gvar;

struct mi_gvar_chg_struct
{
 char *name;
 int   in_scope;  /* if true the other fields apply. */
 char *new_type;  /* NULL if type_changed==false */
 int   new_num_children; /* only when new_type!=NULL */

 struct mi_gvar_chg_struct *next;
};
typedef struct mi_gvar_chg_struct mi_gvar_chg;

struct mi_gvar_child_struct
{
 char *name;
 char *exp;
 char *type;  /* Optional. */
 int numchild;
 char *value; /* Only MI v2, not yet implemented here. */
};
typedef struct mi_gvar_child_struct mi_gvar_child;

struct mi_gvar_children_struct
{
 int numchild;
 mi_gvar_child *c;
};
typedef struct mi_gvar_children_struct mi_gvar_children;

/*
 Examining gdb sources and looking at docs I can see the following "stop"
reasons:

Breakpoints:
a) breakpoint-hit (bkptno) + frame
Also: without reason for temporal breakpoints.

Watchpoints:
b) watchpoint-trigger (wpt={number,exp};value={old,new}) + frame
c) read-watchpoint-trigger (hw-rwpt{number,exp};value={value}) + frame
d) access-watchpoint-trigger (hw-awpt{number,exp};value={[old,]new}) + frame
e) watchpoint-scope (wpnum) + frame

Movement:
f) function-finished ([gdb-result-var,return-value]) + frame
g) location-reached + frame
h) end-stepping-range + frame

Exit:
i) exited-signalled (signal-name,signal-meaning)
j) exited (exit-code)
k) exited-normally

Signal:
l) signal-received (signal-name,signal-meaning) + frame

Plus: thread-id
*/
enum mi_stop_reason
{
 sr_unknown=0,
 sr_bkpt_hit,
 sr_wp_trigger, sr_read_wp_trigger, sr_access_wp_trigger, sr_wp_scope,
 sr_function_finished, sr_location_reached, sr_end_stepping_range,
 sr_exited_signalled, sr_exited, sr_exited_normally,
 sr_signal_received
};

struct mi_stop_struct
{
 enum mi_stop_reason reason; /* If more than one reason just the last. */
 /* Flags indicating if non-pointer fields are filled. */
 char have_thread_id;
 char have_bkptno;
 char have_exit_code;
 char have_wpno;
 /* Where stopped. Doesn't exist for sr_exited*. */
 int thread_id;
 mi_frames *frame;
 /* sr_bkpt_hit */
 int bkptno;
 /* sr_*wp_* no scope */
 mi_wp *wp;
 char *wp_old;
 char *wp_val;
 /* sr_wp_scope */
 int wpno;
 /* sr_function_finished. Not for void func. */
 char *gdb_result_var;
 char *return_value;
 /* sr_exited_signalled, sr_signal_received */
 char *signal_name;
 char *signal_meaning;
 /* sr_exited */
 int exit_code;
};
typedef struct mi_stop_struct mi_stop;

/* Variable containing the last error. */
extern int mi_error;
extern char *mi_error_from_gdb;
const char *mi_get_error_str();

/* Indicate the name of gdb exe. Default is /usr/bin/gdb */
void mi_set_gdb_exe(char *name);
/* Connect to a local copy of gdb. */
mi_h *mi_connect_local();
/* Close connection. You should ask gdb to quit first. */
void  mi_disconnect(mi_h *h);
/* Parse gdb output. */
mi_output *mi_parse_gdb_output(const char *str);
/* Functions to set/get the tunneled streams callbacks. */
void mi_set_console_cb(mi_h *h, stream_cb cb, void *data);
void mi_set_target_cb(mi_h *h, stream_cb cb, void *data);
void mi_set_log_cb(mi_h *h, stream_cb cb, void *data);
stream_cb mi_get_console_cb(mi_h *h, void **data);
stream_cb mi_get_target_cb(mi_h *h, void **data);
stream_cb mi_get_log_cb(mi_h *h, void **data);
/* The callback to deal with async events. */
void mi_set_async_cb(mi_h *h, async_cb cb, void *data);
async_cb mi_get_async_cb(mi_h *h, void **data);
/* Callbacks to "see" the dialog with gdb. */
void mi_set_to_gdb_cb(mi_h *h, stream_cb cb, void *data);
void mi_set_from_gdb_cb(mi_h *h, stream_cb cb, void *data);
stream_cb mi_get_to_gdb_cb(mi_h *h, void **data);
stream_cb mi_get_from_gdb_cb(mi_h *h, void **data);
/* Sends a message to gdb. */
int mi_send(mi_h *h, const char *format, ...);
/* Wait until gdb sends a response. */
mi_output *mi_get_response_blk(mi_h *h);
/* Check if gdb sent a complete response. Use with mi_retire_response. */
int mi_get_response(mi_h *h);
/* Get the last response. Use with mi_get_response. */
mi_output *mi_retire_response(mi_h *h);
/* Look for a result record in gdb output. */
mi_output *mi_get_rrecord(mi_output *r);
/* Look if the output contains an async stop.
   If that's the case return the reason for the stop.
   If the output contains an error the description is returned in reason. */
int mi_get_async_stop_reason(mi_output *r, char **reason);
mi_frames *mi_get_async_frame(mi_output *r);
/* Wait until gdb sends a response.
   Then check if the response is of the desired type. */
int mi_res_simple_exit(mi_h *h);
int mi_res_simple_done(mi_h *h);
int mi_res_simple_running(mi_h *h);
int mi_res_simple_connected(mi_h *h);
/* It additionally extracts an specified variable. */
mi_results *mi_res_done_var(mi_h *h, const char *var);
/* Extract a frames list from the response. */
mi_frames *mi_res_frames_array(mi_h *h, const char *var);
mi_frames *mi_parse_frame(mi_results *c);
mi_frames *mi_res_frame(mi_h *h);
/* Create an auxiliar terminal using xterm. */
mi_aux_term *gmi_start_xterm();
/* Indicate the name of xterm exe. Default is /usr/bin/X11/xterm */
void mi_set_xterm_exe(char *name);
/* Kill the auxiliar terminal and release the structure. */
void gmi_end_aux_term(mi_aux_term *t);
/* Look for a free Linux VT for the child. */
mi_aux_term *gmi_look_for_free_vt();
/* Look for a free and usable Linux VT. */
int mi_look_for_free_vt();
/* Extract a list of thread IDs from response. */
int mi_res_thread_ids(mi_h *h, int **list);
int mi_get_thread_ids(mi_output *res, int **list);
/* A variable response. */
mi_gvar *mi_res_gvar(mi_h *h, mi_gvar *cur, const char *expression);
enum mi_gvar_fmt mi_format_str_to_enum(const char *format);
const char *mi_format_enum_to_str(enum mi_gvar_fmt format);
enum mi_gvar_lang mi_lang_str_to_enum(const char *lang);
const char *mi_lang_enum_to_str(enum mi_gvar_lang lang);
int mi_res_changelist(mi_h *h, mi_gvar_chg **changed);
mi_gvar_children *mi_res_children(mi_h *h);
mi_bkpt *mi_res_bkpt(mi_h *h);
mi_wp *mi_res_wp(mi_h *h);
char *mi_res_value(mi_h *h);
mi_stop *mi_res_stop(mi_h *h);
enum mi_stop_reason mi_reason_str_to_enum(const char *s);
const char *mi_reason_enum_to_str(enum mi_stop_reason r);

/* Allocation functions: */
void *mi_calloc(size_t count, size_t sz);
void *mi_calloc1(size_t sz);
char *mi_malloc(size_t sz);
mi_results       *mi_alloc_results();
mi_output        *mi_alloc_output();
mi_frames        *mi_alloc_frames();
mi_gvar          *mi_alloc_gvar();
mi_gvar_chg      *mi_alloc_gvar_chg();
mi_gvar_children *mi_alloc_gvar_children();
mi_gvar_child    *mi_alloc_gvar_child(size_t count);
mi_bkpt          *mi_alloc_bkpt();
mi_wp            *mi_alloc_wp();
mi_stop          *mi_alloc_stop();
void mi_free_output(mi_output *r);
void mi_free_output_but(mi_output *r, mi_output *no, mi_results *no_r);
void mi_free_frames(mi_frames *f);
void mi_free_aux_term(mi_aux_term *t);
void mi_free_results(mi_results *r);
void mi_free_results_but(mi_results *r, mi_results *no);
void mi_free_gvar(mi_gvar *v);
void mi_free_gvar_chg(mi_gvar_chg *p);
void mi_free_children(mi_gvar_children *c);
void mi_free_wp(mi_wp *wp);
void mi_free_stop(mi_stop *s);

/* Porgram control: */
/* Specify the executable and arguments for local debug. */
int gmi_set_exec(mi_h *h, const char *file, const char *args);
/* Start running the executable. Remote sessions starts running. */
int gmi_exec_run(mi_h *h);
/* Continue the execution after a "stop". */
int gmi_exec_continue(mi_h *h);
/* Indicate which terminal will use the target program. For local sessions. */
int gmi_target_terminal(mi_h *h, const char *tty_name);
/* Specify what's the local copy that have debug info. For remote sessions. */
int gmi_file_symbol_file(mi_h *h, const char *file);
/* Continue until function return, the return value is included in the async
   response. */
int gmi_exec_finish(mi_h *h);
/* Stop the program using SIGINT. */
int gmi_exec_interrupt(mi_h *h);
/* Next line of code. */
int gmi_exec_next(mi_h *h);
/* Next line of assembler code. */
int gmi_exec_next_instruction(mi_h *h);
/* Next line of code. Get inside functions. */
int gmi_exec_step(mi_h *h);
/* Next line of assembler code. Get inside calls. */
int gmi_exec_step_instruction(mi_h *h);
/* Execute until location is reached. If file is NULL then is until next line. */
int gmi_exec_until(mi_h *h, const char *file, int line);
/* Return to previous frame inmediatly. */
mi_frames *gmi_exec_return(mi_h *h);
/* Just kill the program. Please read the notes in prg_control.c. */
int gmi_exec_kill(mi_h *h);

/* Target manipulation: */
/* Connect to a remote gdbserver using the specified methode. */
int gmi_target_select(mi_h *h, const char *type, const char *params);
/* Attach to an already running process. */
mi_frames *gmi_target_attach(mi_h *h, pid_t pid);
/* Detach from an attached process. */
int gmi_target_detach(mi_h *h);

/* Miscellaneous commands: */
/* Exit gdb killing the child is it is running. */
void gmi_gdb_exit(mi_h *h);
/* Send the version to the console. */
int gmi_gdb_version(mi_h *h);
/* Set a gdb variable. */
int gmi_gdb_set(mi_h *h, const char *var, const char *val);
/* Get a gdb variable. */
char *gmi_gdb_show(mi_h *h, const char *var);

/* Breakpoints manipulation: */
/* Insert a breakpoint at file:line. */
mi_bkpt *gmi_break_insert(mi_h *h, const char *file, int line);
/* Insert a breakpoint, all available options. */
mi_bkpt *gmi_break_insert_full(mi_h *h, int temporary, int hard_assist,
                               const char *cond, int count, int thread,
                               const char *where);
mi_bkpt *gmi_break_insert_full_fl(mi_h *h, const char *file, int line,
                                  int temporary, int hard_assist,
                                  const char *cond, int count, int thread);
/* Remove a breakpoint. */
int gmi_break_delete(mi_h *h, int number);
/* Free the memory used for a breakpoint description. */
void mi_free_bkpt(mi_bkpt *b);
/* Modify the "ignore" count for a breakpoint. */
int gmi_break_set_times(mi_h *h, int number, int count);
/* Associate a condition with the breakpoint. */
int gmi_break_set_condition(mi_h *h, int number, const char *condition);
/* Enable or disable a breakpoint. */
int gmi_break_state(mi_h *h, int number, int enable);
/* Set a watchpoint. It doesn't work for remote targets! */
mi_wp *gmi_break_watch(mi_h *h, enum mi_wp_mode mode, const char *exp);

/* Data Manipulation. */
/* Evaluate an expression. Returns a parsed tree. */
char *gmi_data_evaluate_expression(mi_h *h, const char *expression);
/* Path for sources. */
int gmi_dir(mi_h *h, char *path);

/* Stack manipulation. */
/* List of frames. Arguments aren't filled. */
mi_frames *gmi_stack_list_frames(mi_h *h);
/* List of frames. Indicating a range. */
mi_frames *gmi_stack_list_frames_r(mi_h *h, int from, int to);
/* List arguments. Only level and args filled. */
mi_frames *gmi_stack_list_arguments(mi_h *h, int show);
/* List arguments. Indicating a range. Only level and args filled. */
mi_frames *gmi_stack_list_arguments_r(mi_h *h, int show, int from, int to);
/* Information about the current frame, including args. */
mi_frames *gmi_stack_info_frame(mi_h *h);
/* Stack info depth. error => -1 */
int gmi_stack_info_depth_get(mi_h *h);
/* Set stack info depth. error => -1 */
int gmi_stack_info_depth(mi_h *h, int max_depth);
/* Change current frame. */
int gmi_stack_select_frame(mi_h *h, int framenum);
/* List of local vars. */
mi_results *gmi_stack_list_locals(mi_h *h, int show);

/* Thread. */
/* List available thread ids. */
int gmi_thread_list_ids(mi_h *h, int **list);
/* Select a thread. */
mi_frames *gmi_thread_select(mi_h *h, int id);

/* Variable objects. */
/* Create a variable object. */
mi_gvar *gmi_var_create_nm(mi_h *h, const char *name, int frame, const char *exp);
mi_gvar *gmi_var_create(mi_h *h, int frame, const char *exp);
/* Create the variable and also fill the lang and attr fields. */
mi_gvar *gmi_full_var_create(mi_h *h, int frame, const char *exp);
/* Delete a variable object. Doesn't free the mi_gvar data. */
int gmi_var_delete(mi_h *h, mi_gvar *var);
/* Set the format used to represent the result. */
int gmi_var_set_format(mi_h *h, mi_gvar *var, enum mi_gvar_fmt format);
/* Fill the format field with info from gdb. */
int gmi_var_show_format(mi_h *h, mi_gvar *var);
/* Fill the numchild field with info from gdb. */
int gmi_var_info_num_children(mi_h *h, mi_gvar *var);
/* Fill the type field with info from gdb. */
int gmi_var_info_type(mi_h *h, mi_gvar *var);
/* Fill the expression and lang fields with info from gdb.
   Note that lang isn't filled during creation. */
int gmi_var_info_expression(mi_h *h, mi_gvar *var);
/* Fill the attr field with info from gdb.
   Note that attr isn't filled during creation. */
int gmi_var_show_attributes(mi_h *h, mi_gvar *var);
/* Update variable. Use NULL for all.
   Note that *changed can be NULL if none updated. */
int gmi_var_update(mi_h *h, mi_gvar *var, mi_gvar_chg **changed);
/* Change variable. Returns the new value. */
mi_results *gmi_var_assign(mi_h *h, mi_gvar *var, const char *expression);
/* Get current value for a variable. */
char *gmi_var_evaluate_expression(mi_h *h, mi_gvar *var);
/* List children. It ONLY returns the first level information. :-( */
mi_gvar_children *gmi_var_list_children(mi_h *h, mi_gvar *var);

#ifdef __cplusplus
};

/* C++ interface */

/*
 State                Can:
 disconnected         Connect
  connected           SelectTarget, Disconnect
   target_specified    TargetUnselect, Run, Set breakpoints/watchpoints, etc.
    running            Stop
    stopped            Kill, Restart?, Step, Trace, Continue, etc.
    [auto exit]

  Modes:
  dmX11                 Local debug for X11.
  dmLinux               Local debug for Linux console.
  dmRemote              Remote debug.
*/
class MIDebugger
{
public:
 MIDebugger();
 ~MIDebugger();

 enum eState { disconnected, connected, target_specified, running, stopped };
 enum dMode  { dmX11, dmLinux, dmRemote, dmPID };

 int Connect(bool remote=false); /* remote is currently ignored. */
 int Disconnect();
 /* SelectTarget* */
 int SelectTargetX11(const char *exec, const char *args=NULL,
                     const char *auxtty=NULL);
 int SelectTargetLinux(const char *exec, const char *args,
                       const char *auxtty=NULL);
 int SelectTargetRemote(const char *exec, const char *rparams,
                        const char *rtype=NULL);
 // TODO: Linux PIDs can be represented as intergers. What should I use?
 // ato_pid_t doesn't exist ;-)
 mi_frames *SelectTargetPID(const char *exec, int pid);
 int TargetUnselect();
 int Run();
 int Stop();
 int Poll(mi_stop *&rs);
 int Continue();
 int RunOrContinue();
 int Kill();
 mi_bkpt *Breakpoint(const char *file, int line);
 mi_bkpt *Breakpoint(const char *where, bool temporary=false, const char *cond=NULL,
                     int count=-1, int thread=-1, bool hard_assist=false);
 mi_bkpt *BreakpointFull(const char *file, int line, bool temporary=false,
                         const char *cond=NULL, int count=-1, int thread=-1,
                         bool hard_assist=false);
 mi_bkpt *Breakpoint(mi_bkpt *b);
 int BreakDelete(mi_bkpt *b);
 mi_wp *Watchpoint(enum mi_wp_mode mode, const char *exp);
 int WatchDelete(mi_wp *w);
 int RunToMain();
 int StepOver(bool inst=false);
 int TraceInto(bool inst=false);
 int GoTo(const char *file, int line);
 int FinishFun();
 mi_frames *ReturnNow();
 mi_frames *CallStack(bool args);
 char *EvalExpression(char *exp);
 char *ModifyExpression(char *exp, char *newVal);
 mi_gvar *AddgVar(const char *exp, int frame=-1)
 {
  if (state!=stopped)
     return NULL;
  return gmi_full_var_create(h,frame,exp);
 }
 int DelgVar(mi_gvar *var)
 {
  if (state!=stopped)
     return 0;
  return gmi_var_delete(h,var);
 }
 int ListChangedgVar(mi_gvar_chg *&changed)
 {
  if (state!=stopped)
     return 0;
  return gmi_var_update(h,NULL,&changed);
 }
 int Send(const char *command);
 int Version()
 {
  if (state==running || state==disconnected)
     return 0;
  return gmi_gdb_version(h);
 }
 int PathSources(char *path)
 {
  if (state==running || state==disconnected)
     return 0;
  return gmi_dir(h,path);
 }

 eState GetState() { return state; }

 /* Some wrappers */
 static void SetGDBExe(char *name) { mi_set_gdb_exe(name); }
 static void SetXTerm(char *name) { mi_set_xterm_exe(name); }
 static const char *GetErrorStr() { return mi_get_error_str(); }
 static const char *GetGDBError() { return mi_error_from_gdb; }
 static int GetErrorNumber() { return mi_error; }
 void SetConsoleCB(stream_cb cb, void *data=NULL)
   { mi_set_console_cb(h,cb,data); }
 void SetTargetCB(stream_cb cb, void *data=NULL)
   { mi_set_target_cb(h,cb,data); }
 void SetLogCB(stream_cb cb, void *data=NULL)
   { mi_set_log_cb(h,cb,data); }
 void SetAsyncCB(async_cb cb, void *data=NULL)
   { mi_set_async_cb(h,cb,data); }
 void SetToGDBCB(stream_cb cb, void *data=NULL)
   { mi_set_to_gdb_cb(h,cb,data); }
 void SetFromGDBCB(stream_cb cb, void *data=NULL)
   { mi_set_from_gdb_cb(h,cb,data); }

 const char *GetAuxTTY()
   { return aux_tty ? aux_tty->tty : NULL; }

protected:
 eState state;
 dMode mode;
 bool  preRun;  // Remote targets starts running but outside main.
 mi_h *h;
 mi_aux_term *aux_tty;
 int waitingTempBkpt;

 int SelectTargetTTY(const char *exec, const char *args, const char *auxtty,
                     dMode m);
};

#endif

