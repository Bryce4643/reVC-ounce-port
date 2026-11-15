// Switch 2 (kaiHEN) on-device logging + crash handler for reVC.
//
// Goal: when the app crashes on boot or shows a black screen, the player can
// pull a single text file for analysis.
//
//  * stdout and stderr are redirected (unbuffered) to revc_log.txt, so all of
//    reVC's existing printf()/debug()/re3_debug() output is captured live. A
//    hang/black screen therefore ends with the last thing the game logged.
//  * A CPU exception handler dumps the fault type and registers, so a hard
//    crash (which otherwise only shows the Switch error applet) leaves a trace.
//
// The log is written next to the NRO / game data as "revc_log.txt" (the same
// working directory reVC reads its game files from), falling back to the SD
// root "sdmc:/reVC_log.txt" if that is not writable.

#ifdef __SWITCH__

#include <switch.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "switch_log.h"

static FILE *s_logFile = NULL;
static int   s_logFd   = -1;

// Enlarge libnx's weak-default exception stack (0x1000) so the crash handler
// has room to format text and call write(). These override libounce's weak
// __nx_exception_stack / __nx_exception_stack_size.
extern "C" {
alignas(16) u8 __nx_exception_stack[0x4000];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
}

// ---- low-level, allocation-free writers (safe to use in the crash handler) --

static void logRaw(const char *s, size_t n)
{
	if (s_logFd >= 0)
		write(s_logFd, s, n);
}

static void logStr(const char *s)
{
	logRaw(s, strlen(s));
}

static void logHex64(u64 v)
{
	char buf[18];
	buf[0] = '0';
	buf[1] = 'x';
	for (int i = 0; i < 16; i++) {
		int nyb = (int)((v >> ((15 - i) * 4)) & 0xF);
		buf[2 + i] = (char)(nyb < 10 ? ('0' + nyb) : ('a' + nyb - 10));
	}
	logRaw(buf, sizeof(buf));
}

// Two-digit decimal register index, e.g. "X07".
static void logRegLabel(int i)
{
	char buf[4];
	buf[0] = 'X';
	buf[1] = (char)('0' + (i / 10));
	buf[2] = (char)('0' + (i % 10));
	buf[3] = '=';
	logRaw(buf, sizeof(buf));
}

// ---- CPU exception handler --------------------------------------------------

extern "C" void __libnx_exception_handler(ThreadExceptionDump *ctx)
{
	logStr("\n\n==== reVC CPU EXCEPTION ====\n");

	logStr("error_desc=");
	logHex64(ctx->error_desc);
	switch (ctx->error_desc) {
	case ThreadExceptionDesc_InstructionAbort: logStr(" (instruction abort)");  break;
	case ThreadExceptionDesc_MisalignedPC:     logStr(" (misaligned PC)");      break;
	case ThreadExceptionDesc_MisalignedSP:     logStr(" (misaligned SP)");      break;
	case ThreadExceptionDesc_SError:           logStr(" (SError)");             break;
	case ThreadExceptionDesc_BadSVC:           logStr(" (bad SVC)");            break;
	case ThreadExceptionDesc_Trap:             logStr(" (trap)");               break;
	default:                                   logStr(" (data abort / other)"); break;
	}
	logStr("\n");

	logStr("PC ="); logHex64(ctx->pc.x);  logStr("   LR ="); logHex64(ctx->lr.x);  logStr("\n");
	logStr("SP ="); logHex64(ctx->sp.x);  logStr("   FP ="); logHex64(ctx->fp.x);  logStr("\n");
	logStr("FAR="); logHex64(ctx->far.x); logStr("   ESR="); logHex64(ctx->esr);   logStr("\n");

	for (int i = 0; i < 29; i++) {
		logRegLabel(i);
		logHex64(ctx->cpu_gprs[i].x);
		logStr((i % 3) == 2 ? "\n" : "  ");
	}
	logStr("\n");
	logStr("Subtract the NRO load base (see the hbloader/crash dump) from PC "
	       "to map it back to reVC.elf with addr2line.\n");
	logStr("==== end exception ====\n");

	if (s_logFd >= 0)
		fsync(s_logFd);
	// Returning lets libnx terminate the process (the error applet still shows).
}

// ---- init + milestones ------------------------------------------------------

extern "C" void SwitchLog_Init(void)
{
	if (s_logFile != NULL)
		return;

	s_logFile = fopen("revc_log.txt", "w");
	if (s_logFile == NULL)
		s_logFile = fopen("sdmc:/reVC_log.txt", "w");
	if (s_logFile == NULL)
		return;

	s_logFd = fileno(s_logFile);

	// Route stdout/stderr (hence all of reVC's printf/debug output) to the log,
	// unbuffered so nothing is lost on a crash or hang.
	fflush(stdout);
	fflush(stderr);
	dup2(s_logFd, STDOUT_FILENO);
	dup2(s_logFd, STDERR_FILENO);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	logStr("==== reVC Switch 2 log ====\n");
	logStr("built " __DATE__ " " __TIME__ "\n");

	char cwd[512];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		logStr("cwd: ");
		logStr(cwd);
		logStr("\n");
	}
	logStr("---------------------------\n");
}

extern "C" void SwitchLog_Milestone(const char *msg)
{
	if (s_logFd < 0)
		return;
	logStr("[boot] ");
	logStr(msg ? msg : "(null)");
	logStr("\n");
	fsync(s_logFd);
}

#endif // __SWITCH__
