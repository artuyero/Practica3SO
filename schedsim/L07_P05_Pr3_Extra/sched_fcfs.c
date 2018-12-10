#include "sched.h"


static task_t* pick_next_task_fcfs(runqueue_t* rq)
{
	/* Se coge la base de la pila porque es la tarea con mas antiguedad,
	 * es decir la primera que llegó a la cola */
	task_t* t = head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

/*static int compare_tasks_cpu_burst(void *t1,void *t2)
{
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->runnable_ticks_left-tsk2->runnable_ticks_left;
}*/

static void enqueue_task_fcfs(task_t* t, runqueue_t* rq, int preempted)
{
	/* Aqui se utiliza la insercion ordenada para asegurar que la primera
	tarea sea la mas corta en tiempo de ejecucion, para luego en la rutina
	de extraer siguiente tarea, al coger head, sea efectivamente la rutina
	adecuada segun sjf */

	if (t->on_rq || is_idle_task(t))
		return;

	insert_slist(&rq->tasks, t);
}

static task_t* steal_task_fcfs(runqueue_t* rq)
{
	/* Robamos la ultima tarea de la cola porque es la ultima en llegar y la que menos
	 * prioridad tiene segun el fcfs, es decir la cima de la pila */
	task_t* t = tail_slist(&rq->tasks);

	if (t) 
		remove_slist(&rq->tasks,t);
	
	return t;
}


sched_class_t fcfs_sched= {
	.pick_next_task = pick_next_task_fcfs,
	.enqueue_task = enqueue_task_fcfs,
	.steal_task = steal_task_fcfs
};

