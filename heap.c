#include "codexion.h"

int    is_more_urgent(t_sim *sim, t_coder *a, t_coder *b)
{
    if (sim->scheduler == 0)
        return fifo_check(sim, a, b);

    else
        return edf_check(sim, a, b);
}

void    heap_push(t_sim *sim, t_coder *new_coder)
{
    int index;

    pthread_mutex_lock(&sim->queue_mutex);

    sim->heap[sim->heap_size] = new_coder;
    index = sim->heap_size;
    
    while (index > 0)
    {
        if (is_more_urgent(sim, new_coder, sim->heap[(index - 1)/2]) == 1)
        {
            swap_coders(sim->heap, index, (index - 1)/2);
            index = (index - 1) / 2;
        }
        else
        break;
    }
    sim->heap_size++;
    pthread_cond_broadcast(&sim->condition);
    pthread_mutex_unlock(&sim->queue_mutex);

}

void    heap_pop(t_sim *sim)
{
    int i;

    pthread_mutex_lock(&sim->queue_mutex);
    if (sim->heap_size == 0)
    {
        pthread_mutex_unlock(&sim->queue_mutex);
        return ;
    }
    sim->heap[0] = sim->heap[sim->heap_size - 1];
    sim->heap_size--;
    i = 0;
    while ((2 * i) + 1 < sim->heap_size)
    {
        int left;
        int right;
        int target;
        left = (i * 2) + 1;
        right = (i * 2) + 2;
        target = left;
        if (right < sim->heap_size && is_more_urgent(sim, sim->heap[right], sim->heap[left]))
            target = right;

        if (is_more_urgent(sim, sim->heap[target], sim->heap[i]))
        {
            swap_coders(sim->heap, target, i);
            i = target;
        }
        else
            break ;
    }
    pthread_mutex_unlock(&sim->queue_mutex);
}