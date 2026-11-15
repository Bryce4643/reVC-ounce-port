// Switch 2 (kaiHEN) on-device logging + crash handler for reVC.
// On non-Switch builds everything here compiles to nothing.
#ifndef SWITCH_LOG_H
#define SWITCH_LOG_H

#ifdef __SWITCH__

#ifdef __cplusplus
extern "C" {
#endif

// Redirect stdout/stderr to the log file (unbuffered) and install the CPU
// exception handler. Call this once, as early as possible in main().
void SwitchLog_Init(void);

// Write a boot/progress milestone line to the log (flushed immediately).
void SwitchLog_Milestone(const char *msg);

#ifdef __cplusplus
}
#endif

#define SWITCH_LOG(msg) SwitchLog_Milestone(msg)

#else // !__SWITCH__

#define SWITCH_LOG(msg) ((void)0)

#endif // __SWITCH__

#endif // SWITCH_LOG_H
