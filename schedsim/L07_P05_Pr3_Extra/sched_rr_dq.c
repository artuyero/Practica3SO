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
	struct rr_data* cs_data = malloc(sizeof (struct rr_data));

	if (!cs_data)
		return 1;
	
	// La asignamos como Interactive nada mas llegar
	cs_data->type = INTERACTIVE;
	
	// Ponemos el quantum de la variable global
	cs_data->remaining_ticks_slice = rr_quantum_int;
	
	t->tcs_data = cs_data;

	return 0;
}

static void task_free_rr_dq(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
	task_t* t=head_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks, t);
	else{
		t = head_slist(cpu_bound_tasks);
		if (t)
			remove_slist(cpu_bound_tasks, t);
	}

	return t;
}

static void enqueue_task_rr_dq(task_t* t,runqueue_t* rq, int runnable)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;
    
    struct rr_data* cs_data=(struct rr_data*) t->tcs_data;

    // Comprobacion
    if (t->on_rq || is_idle_task(t))
	return;

	// Si es tipo INTERACTIVE
    if(cs_data->type == INTERACTIVE) {
    	insert_slist(&rq->tasks, t);
    	cs_data->remaining_ticks_slice = rr_quantum_int;
    }
    // Si es de otro tipo
    else {
    	insert_slist(cpu_bound_tasks, t);
    	cs_data->remaining_ticks_slice = rr_quantum_cpu;
    }
    
}

static void task_tick_rr_dq(runqueue_t* rq)
{
    task_t* current = rq->cur_task;
    struct rr_data* cs_data = (struct rr_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--;

	if (cs_data->remaining_ticks_slice <= 0){
		cs_data->type = CPU_BOUND;
		rq->need_resched = TRUE;
	}
	else{
		cs_data->type = INTERACTIVE;
	}

    
}

static task_t* steal_task_rr_dq(runqueue_t* rq)
{
    // Pointer to the CPU_BOUND tasks list
    slist_t* cpu_bound_tasks = rq->rq_cs_data;

    	task_t* t = tail_slist(cpu_bound_tasks);

    	if (t)
    		remove_slist(cpu_bound_tasks, t);
    	else{
    		t = tail_slist(&rq->tasks);
    		if (t)
    			remove_slist(&rq->tasks, t);
    	}

    	return t;
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
