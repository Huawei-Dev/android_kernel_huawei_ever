#ifndef __FRAME_EXTERN_H
#define __FRAME_EXTERN_H

static inline bool is_frame_task(struct task_struct *task) { return false; }
static inline int set_frame_rate(int rate) { return 0; }
static inline int set_frame_margin(int margin) { return 0; }
static inline int set_frame_status(unsigned long status) { return 0; }
static inline int set_frame_max_util(int max_util) { return 0; }
static inline int set_frame_timestamp(unsigned long timestamp) { return 0; }
static inline void set_frame_sched_state(bool enable) { return 0; }
static inline int set_frame_min_util(int min_util) { return 0; }
static inline int set_frame_min_util_and_margin(int min_util, int margin) { return 0; }
static inline void update_frame_thread(int pid, int tid) { return 0; }
static inline int update_frame_isolation(void) { return 1; }

#define FRAME_SYSTRACE(format, ...)

#endif
