#include "codexion.h"

void    *routine(void *arg)
{
    t_coder *self = (t_coder*)arg;
    t_sim   *sim = self->sim;
    
    pthread_mutex_lock(&sim->stop_mutex);
    self->last_compile_start = get_current_time();
    pthread_mutex_unlock(&sim->stop_mutex);
    while (1)
    {
        pthread_mutex_lock(&sim->stop_mutex);
        if (sim->stop) 
        {
            pthread_mutex_unlock(&sim->stop_mutex);
            break;
        }
        pthread_mutex_unlock(&sim->stop_mutex);

        self->request_time = get_current_time();

        pthread_mutex_lock(&sim->queue_mutex);
        heap_push(sim, self);

        while (sim->heap[0] != self)
            pthread_cond_wait(&sim->condition, &sim->queue_mutex);

        pthread_mutex_lock(&sim->dongles[self->id - 1].mutex);    
        print_status(self, "has taken a dongle");

        pthread_mutex_lock(&sim->dongles[self->id % sim->num_coders].mutex);    
        print_status(self, "has taken a dongle");

        pthread_mutex_lock(&sim->stop_mutex);
        self->last_compile_start = get_current_time(); 
        pthread_mutex_unlock(&sim->stop_mutex);

        print_status(self, "is compiling");
        usleep(sim->time_to_compile * 1000);
        self->compile_count ++;

        pthread_mutex_unlock(&sim->dongles[self->id - 1].mutex);    
        pthread_mutex_unlock(&sim->dongles[self->id % sim->num_coders].mutex);
        
        heap_pop(sim);
        pthread_cond_broadcast(&sim->condition);
        pthread_mutex_unlock(&sim->queue_mutex);
        
        print_status(self, "is debugging");
        usleep(sim->time_to_debug * 1000);

        print_status(self, "is refactoring");
        usleep(sim->time_to_refactor * 1000);
        if(self->compile_count == sim->must_compile)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            self->finished = 1;
            pthread_mutex_unlock(&sim->stop_mutex);
            break;
        }

    }
    return  (NULL);
}