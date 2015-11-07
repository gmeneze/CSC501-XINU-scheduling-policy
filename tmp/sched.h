struct process_info{
	int pid;
	int counter;
	int goodness;
	};

struct process_info epoch_list[NPROC]; /* Process list for epoch */
struct process_info queue_list[NPROC]; /* Process list of queue */
struct process_info round_robin_queue[NPROC];

extern int current_scheduler;

extern int epoch_time;

extern int multi_sched_switch_time;

extern int index;

extern int queue_index;

extern int max_goodness_index;

extern int flag;

void setschedclass(int sched_class);
int getschedclass();

#define LINUXSCHED 1
#define MULTIQSCHED 2 
