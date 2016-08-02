#include <schedule_rr.c>
#include <schedule_rbed.c>
#include <limits.h>
#include <startup.c>
#ifndef SCHEDULER_SIMULATOR
#       include <kernel.h>
#       include <dispatch.h>
#       include <trace/trace.h>
#       include <trace_definitions/trace_defs.h>
#       include <timer.h> // update_sched_timer
#       include <kcb.h>
#endif

#define SPECTRUM        1000000



void scheduler_convert(void)
{
    enum sched_state from = kcb_current->sched;
    
    switch (from) {
        case SCHED_RBED:
        if(((state_counter++)%4)<2)
        {
            // initialize RR ring
            struct dcb *last = NULL;
            for (struct dcb *i = kcb_current->queue_head; i; i = i->next)
            {
                i->prev = last;
                last = i;
            }
            // at this point: we have a dll, but need to close the ring
            kcb_current->queue_head->prev = kcb_current->queue_tail;
            kcb_current->queue_tail->next = kcb_current->queue_head;
         }
         else
          {
           //do nothing
          }
          state_counter=(state_counter+1)%4;   
         break;
        
        case SCHED_RR:
            if((state_counter++)%4>1)
            {
	 struct dcb *tmp = NULL;
            printf("kcb_current: %p\n", kcb_current);
            printf("kcb_current->ring_current: %p\n", kcb_current->ring_current);
            printf("kcb_current->ring_current->prev: %p\n", kcb_current->ring_current->prev);
            struct dcb *i = kcb_current->ring_current;
            do {
                printf("converting %p\n", i);
                i->type = TASK_TYPE_BEST_EFFORT;
                tmp = i->next;
                i->next = i->prev = NULL;
                make_runnable(i);
                i = tmp;
            } while (i != kcb_current->ring_current);
            for (i = kcb_current->queue_head; i; i=i->next) {
                printf("%p (-> %p)\n", i, i->next);
            }
	  }
             else
             {
               //do nothing
             }
             state_counter=(state_counter + 1)%4;
            break;

           
        default:
            printf("don't know how to convert %d to RBED state\n", from);
            break;
    }
    kcb_current->ring_current = kcb_current->queue_head;
    for (struct dcb *i = kcb_current->ring_current; i != kcb_current->ring_current; i=i->next) {
        printf("dcb %p\n  prev=%p\n  next=%p\n", i, i->prev, i->next);
    }
}

