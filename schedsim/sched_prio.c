#include "sched.h"

static task_t* pick_next_task_prio(runqueue_t* rq) {
	
	task_t* t = head_slist(&rq->tasks);
	if (t){
		remove_slist(&rq->tasks, t);
    }
	return t;
}

static int compare_tasks_cpu_burst(void *t1, void *t2) {
	task_t* task2 = (task_t*)t2;
	task_t* task1 = (task_t*)t1;
	return task1->prio - task2->prio;
}

static void enqueue_task_prio(task_t* t, runqueue_t* rq, int preempted) {

	if (t->on_rq || is_idle_task(t))
		return;

	if (t->flags & TF_INSERT_FRONT)
    {
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_cpu_burst);
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_cpu_burst);

	if (!preempted)
    {
		task_t* current = rq->cur_task;
		if (preemptive_scheduler && t->prio < current->prio)
        {
			rq->need_resched = TRUE;
			current->flags|=TF_INSERT_FRONT;
		}
	}
}


static task_t* steal_task_prio(runqueue_t* rq) {
	task_t* t = tail_slist(&rq->tasks);
	if (t) 
		remove_slist(&rq->tasks,t);
	return t;
}

sched_class_t prio_sched = {
	.pick_next_task = pick_next_task_prio,
	.enqueue_task = enqueue_task_prio,
	.steal_task = steal_task_prio
};
