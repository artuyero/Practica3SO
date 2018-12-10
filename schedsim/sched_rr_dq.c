#include "sched.h"

typedef enum t_type {CPU_BOUND, INTERACTIVE} tast_type;

// Structure to store RR thread-specific fields
struct rr_data {
    int remaining_ticks_slice;
    tast_type type;
};

// Global RR quantum parameter for INTERACTIVE tasks
int rr_quantum_int=2;
// Global RR quantum parameter for CPU_BOUND tasks
int rr_quantum_cpu=4;

static int sched_init_rr_dq(void) {
    int cpu;
    runqueue_t* cpu_rq;
    slist_t *tasks_cpu_bound;

    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        
        // Use rq_cs_data for cpu_bound queue tasks
        if( (tasks_cpu_bound=malloc(sizeof(slist_t))) == NULL) {
            fprintf(stderr,"Cannot allocate memory for private run queues\n");
            return 1;
        }
        // Init slist
        init_slist(tasks_cpu_bound, offsetof(task_t,rq_links));
        
        // Assign pointer
        cpu_rq->rq_cs_data=tasks_cpu_bound;
    }
    return 0;
}

void sched_destroy_rr_dq(void) {
    int cpu;
    runqueue_t* cpu_rq;

    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        // Free up data structure
        free(cpu_rq->rq_cs_data);
        cpu_rq->rq_cs_data=NULL;
    }    
}

static int task_new_rr_dq(task_t* t)
{
    // New tasks -> INTERACTIVE
    // TODO
}

static void task_free_rr_dq(task_t* t)
{

    // TODO
    
}

static task_t* pick_next_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
    // TODO

    
}

static void enqueue_task_rr_dq(task_t* t,runqueue_t* rq, int runnable)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
    struct rr_data* cs_data=(struct rr_data*) t->tcs_data;

    // TODO
    
}

static void task_tick_rr_dq(runqueue_t* rq)
{
    task_t* current=rq->cur_task;
    struct rr_data* cs_data=(struct rr_data*) current->tcs_data;

    // TODO
    
}

static task_t* steal_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;

    // TODO
    
}

sched_class_t rr_sched_dq= {
    .sched_init=sched_init_rr_dq,
    .sched_destroy=sched_destroy_rr_dq,
	.task_new=task_new_rr_dq,
	.task_free=task_free_rr_dq,
	.pick_next_task=pick_next_task_rr_dq,
	.enqueue_task=enqueue_task_rr_dq,
	.task_tick=task_tick_rr_dq,
	.steal_task=steal_task_rr_dq
};
