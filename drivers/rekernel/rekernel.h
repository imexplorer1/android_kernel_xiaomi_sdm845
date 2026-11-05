#ifndef __REKERNEL_H
#define __REKERNEL_H

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/freezer.h>
#include <uapi/linux/android/binder.h>

enum report_type {
	BINDER,
	SIGNAL,
#ifdef CONFIG_REKERNEL_NETWORK
	NETWORK,
#endif /* CONFIG_REKERNEL_NETWORK */
};
enum binder_type {
	REPLY,
	TRANSACTION,
	OVERFLOW,
};

/*
 * JOBCTL_TRAP_FREEZE only exists on kernels with the upstream cgroup freezer
 * v2 support.  Linux 4.9 does not have it (it only has JOBCTL_TRAP_STOP and
 * JOBCTL_TRAP_NOTIFY), so fall back to the cgroup freezer state alone.
 */
#ifdef JOBCTL_TRAP_FREEZE
static inline bool jobctl_frozen(struct task_struct* task) {
	return ((task->jobctl & JOBCTL_TRAP_FREEZE) != 0);
}
#else
static inline bool jobctl_frozen(struct task_struct* task) {
	return false;
}
#endif /* JOBCTL_TRAP_FREEZE */

static inline bool frozen_task_group(struct task_struct* task) {
	return (jobctl_frozen(task) || cgroup_freezing(task));
}

extern void rekernel_report(int reporttype, int type, pid_t src_pid, struct task_struct* src, pid_t dst_pid, struct task_struct* dst, bool oneway, struct binder_transaction_data* tr);
extern void binder_reply_handler(pid_t src_pid, struct task_struct* src, pid_t dst_pid, struct task_struct* dst, bool oneway, struct binder_transaction_data* tr);
extern void binder_trans_handler(pid_t src_pid, struct task_struct* src, pid_t dst_pid, struct task_struct* dst, bool oneway, struct binder_transaction_data* tr);
extern void binder_overflow_handler(pid_t src_pid, struct task_struct* src, pid_t dst_pid, struct task_struct* dst, bool oneway, struct binder_transaction_data* tr);

#endif /* __REKERNEL_H */
