/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <sched.h>

int epoch_time;
int index;
int max_goodness_index;

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

          int i;
          for(i=0; i<index;i++)
          {
            if(epoch_list[i].pid==pid)
            {
              epoch_time-=epoch_list[i].counter;
              epoch_list[i].counter=0;
              epoch_list[i].goodness=0;
              break; 
            }
          }

          for(i=0; i<queue_index;i++)
          {
            if(queue_list[i].pid==pid)
            {
              queue_list[i].counter=0;
              queue_list[i].goodness=0;
              break;
            }
          } 

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
//         kprintf("\nBAD pid kill pid is : <%d> , state is : <%d>\n",pid,proctab[pid].pstate);
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
//             kprintf("IN PRCURR kill pid is : <%d> , state is : <%d>\n",pid,proctab[pid].pstate);
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
               //         kprintf("In PRREADY pid is: <%d> state : <%d>, name is : <%s> \n",pid,pptr->pstate, pptr->pname);
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
  
	return(OK);
}
