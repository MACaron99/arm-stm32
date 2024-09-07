
#include <system.h>

#include "pt.h"



static pt_t  *pt_list;



static pt_t
*pt_self_get( void )
{
    //
    // Search for the currently running thread
    //
    for( pt_t *pt = pt_list; pt; pt = pt->next )
    {
        if(pt->stage == PT_STAGE_RUNNING)
        {
            return pt;
        }
    }

    return 0;
}


static void
pt_list_item_push( pt_t **head, pt_t *item )
{
    assert( head != 0 );
    assert( item != 0 );

    item->next = *head;

    *head = item;
}


static void
pt_list_item_remove( pt_t **head, pt_t *item )
{
    assert( head != 0 );
    assert( item != 0 );


    if( *head == item )
    {
        *head = item->next;
    }
    else
    {
        for( pt_t *p = *head; p->next; p = p->next )
        {
            if( p->next == item )
            {
                p->next = item->next;

                break;
            }
        }
    }
}


int8_t
pt_mutex_acquire( pt_mutex_t *mutex )
{
    pt_t *pt = pt_self_get();
    pt_t *desired = pt;
    pt_t *expected = 0;

    assert( pt != 0 );

    if(mutex->owner == pt)
    {
        return 1;
    }

    return __atomic_compare_exchange(&(mutex->owner), &expected, &desired, 0, 0, 0);
}


void
pt_mutex_release( pt_mutex_t *mutex )
{
    pt_t *pt = pt_self_get();

    assert( pt != 0 );

    if(mutex->owner == pt)
    {
        mutex->owner = 0;
    }
}


void
pt_schedule( void )
{
    //
    // Loop through all threads
    //
    for( pt_t *pt = pt_list; pt; pt = pt->next )
    {
        pt->stage = PT_STAGE_RUNNING;

        if( pt->function() != PT_STATUS_DONE )
        {
            pt->stage = PT_STAGE_READY;
        }
        else
        {
            pt->stage = PT_STAGE_DONE;

            //
            // Delete thread if it done
            //
            pt_list_item_remove(&pt_list, pt);
        }
    }
}


void
pt_create( pt_t *pt, pt_status_t (*func)() )
{
    assert( pt != 0 );
    assert( func != 0 );

    pt->function = func;
    pt->stage = PT_STAGE_READY;

    sys_barrier()
    {
        pt_list_item_push(&pt_list, pt);
    }
}
