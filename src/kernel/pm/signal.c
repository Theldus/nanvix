/*
 * Copyright (C) 2011-2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * signal.c - Signals.
 * 
 * FIXME: sndsig() and issig()
 */

#include <nanvix/const.h>
#include <nanvix/pm.h>
#include <signal.h>

/*
 * Asserts if a process is ignoring the signal sig.
 */
#define IGNORING(p, sig)                                                      \
		((p->handlers[sig] == SIG_IGN) ||                              \
		 ((p->handlers[sig] == SIG_DFL) && (sig_default[sig] == SIG_IGN)))

/* Default signal handlers. */
PUBLIC sighandler_t sig_default[NR_SIGNALS] = {
	NULL,                     /* SIGNULL */ (sighandler_t)&terminate, /* SIGKILL */ 
	(sighandler_t)&stop,      /* SIGSTOP */ SIG_IGN,                  /* SIGURG  */
	(sighandler_t)&abort,     /* SIGABRT */ (sighandler_t)&abort,     /* SIGBUS  */
	SIG_IGN,                  /* SIGCHLD */ (sighandler_t)&resume,    /* SIGCONT */
	(sighandler_t)&terminate, /* SIGFPE  */ (sighandler_t)&terminate, /* SIGHUP  */
	(sighandler_t)&abort,     /* SIGILL  */ (sighandler_t)&terminate, /* SIGINT  */
	(sighandler_t)&abort,     /* SIGPIPE */ (sighandler_t)&abort,     /* SIGQUIT */
	(sighandler_t)&abort,     /* SIGSEGV */ (sighandler_t)&terminate, /* SIGTERM */
	(sighandler_t)&stop,      /* SIGTSTP */ (sighandler_t)&stop,      /* SIGTTIN */
	(sighandler_t)&stop,      /* SIGTTOU */ (sighandler_t)&terminate, /* SIGALRM */
	(sighandler_t)&terminate, /* SIGUSR1 */ (sighandler_t)&terminate, /* SIGUSR2 */
	(sighandler_t)&abort,     /* SIGTRAP */
};

/*
 * Sends a signal to a process.
 */
PUBLIC void sndsig(struct process *proc, int sig)
{
	struct process *p;
	
	/* Null signal. */
	if (sig == SIGNULL)
		return;
	
	/* Default signal. */
	if (proc->handlers[sig] == SIG_DFL)
	{
		/* Process is igoring this signal. */
		if (sig_default[sig] == SIG_IGN)
			return;
		
		/* Continue process. */
		if (sig_default[sig] == (sighandler_t)&resume)
		{
			resume(proc);
			return;
		}
	}
	
	/* Process is igoring this signal. */
	else if (proc->handlers[sig] == SIG_IGN)
		return;
	
	proc->received |= (1 << sig);
	
	/* Wake up process. */
	if (proc->state == PROC_WAITING)
	{
		/* Remove process from the sleeping chain. */
		if (proc == *proc->chain)
			*proc->chain = proc->next;
		else
		{
			for (p = *proc->chain; p->next != proc; p = p->next);
				/* NOOP */ ;
			p->next = proc->next;
		}
		
		sched(proc);
	}
}

/*
 * Checks if a signal has been sent to the calling process.
 */
PUBLIC int issig()
{
	int i;
	struct process *p;
	
	/* Find a signal that has been sent to the process. */
	for (i = 1; i < NR_SIGNALS; i++)
	{
		/* Found. */
		if (curr_proc->received & (1 << i))
		{
			/* SIGCHLD is somewhat special. */
			if (i == SIGCHLD)
			{
				if (IGNORING(curr_proc, SIGCHLD))
				{					
					/* Clear signal flag. */
					curr_proc->received &= ~(1 << i);
				
					/* Bury zombie children... */
					for (p = FIRST_PROC; p <= LAST_PROC; p++)
					{
						/* Zombie child process. */
						if ((p->father == curr_proc) && (p->state == PROC_ZOMBIE))
						{
							bury(p);
							curr_proc->nchildren--;
						}
					}
					
					/* ...and find another signal. */
					if (curr_proc->nchildren)
						continue;
				}
				
				/* sys_waitpid() will do whatever has to be done. */
				return (SIGCHLD);
			}
			
			/* Not ignoring signal. */
			else if (!IGNORING(curr_proc, i))
				return (i);
			
			/* Clear signal flag. */
			curr_proc->received &= ~(1 << i);
		}
	}
	
	return (SIGNULL);
}