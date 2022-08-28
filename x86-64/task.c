#include <x86-64/cpu.h>
#include <kernel/libc.h>
#include <x86-64/mm.h>
#include <x86-64/syscall.h>
#include <x86-64/task.h>
task_list* tasks;
int next_pid = 1;
void* tty_handle;

context switch_task(size_t rsp, size_t cr3) {
    tasks->c.rsp = rsp;
    tasks->c.cr3 = cr3;
    tasks = tasks->next;
    char c=0;
    while(read_handler(tty_handle, &c, 1) > 0) printf("%c",c);
    return tasks->c;
}

extern void context_switch();
extern void pit_context_switch();
extern void init_pit();
void init_task() {
    tasks = (task_list*)heap_allocate(sizeof(task_list));
    tasks->pid = next_pid++;
    tasks->next = tasks;
    create_fifo("tty");
    tty_handle = open_handler("tty");
    context_switch();
    init_pit();
    install_irq_handler(0,(void*)pit_context_switch,0x8F);
    printf("TASK\n");
}

int add_task(size_t rsp, size_t cr3, size_t* r) {
    task_list* task = (task_list*)heap_allocate(sizeof(task_list));
    task->c.rsp = rsp;
    task->c.cr3 = cr3;
    task->pid = next_pid++;
    for(int i=0;i<16;i++) {
        task->r[i] = r[i];
    }
    task->next = tasks->next;
    tasks->next = task;
    return task->pid;
}

extern void context_switch_nosave();
void exit_task() {
    task_list* task = tasks;
    while(task->next != tasks) task = task->next;
    task->next = tasks->next;
    heap_deallocate(tasks);
    tasks = task->next;
    context_switch_nosave(tasks->c.rsp,tasks->c.cr3);
    for(;;) idle();
}

void save_register(size_t data, int which) {
    tasks->r[which] = data;
}

size_t load_register(int which) {
    return tasks->r[which];
}