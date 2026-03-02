#include "codexion.h"

struct timespec    get_wakeup_time(long remaining)
{
    struct timespec ts;
    long            wakeup_time;

    wakeup_time = get_current_time() + remaining;
    ts.tv_sec = wakeup_time / 1000;
    ts.tv_nsec = (wakeup_time % 1000) * 1000000;
    return (ts);
}


long    dongle_cooldown(t_sim *sim, long last_release)
{
    long    elapsed;
    long    remaining;

    if (last_release == 0)
        return (0);
    elapsed = get_current_time() - last_release;
    if (elapsed < sim->cooldown)
        remaining = sim->cooldown - elapsed;
    else
        remaining = 0;
    return (remaining);
}


void    *routine(void *arg)
{
    t_coder *self = (t_coder*)arg;
    t_sim   *sim = self->sim;
    
    pthread_mutex_lock(&sim->start_mutex);
    while (sim->start == 0)
        pthread_cond_wait(&sim->start_cond, &sim->start_mutex);
    self->last_compile_start = get_current_time();
    sim->ready_count++;
    pthread_cond_broadcast(&sim->start_cond);
    pthread_mutex_unlock(&sim->start_mutex);
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
        pthread_cond_broadcast(&sim->condition);


        while (sim->heap[0] != self && !sim->stop)
            pthread_cond_wait(&sim->condition, &sim->queue_mutex);

        if (sim->stop)
        {
            pthread_mutex_unlock(&sim->queue_mutex);
            return (NULL);
        }
        pthread_mutex_unlock(&sim->queue_mutex);
        
        pthread_mutex_lock(&sim->dongles[self->id - 1].mutex);
        while (dongle_cooldown(sim, sim->dongles[self->id-1].last_release) > 0)
        {
            struct timespec ts = get_wakeup_time(dongle_cooldown(sim, sim->dongles[self->id - 1].last_release));
            pthread_cond_timedwait(&sim->dongles[self->id-1].cond, &sim->dongles[self->id - 1].mutex, &ts);
        }   
        print_status(self, "has taken a dongle");

        pthread_mutex_lock(&sim->dongles[self->id % sim->num_coders].mutex);            
        while (dongle_cooldown(sim, sim->dongles[self->id % sim->num_coders].last_release) > 0)
        {
            struct timespec ts = get_wakeup_time(dongle_cooldown(sim, sim->dongles[self->id % sim->num_coders].last_release));
            pthread_cond_timedwait(&sim->dongles[self->id % sim->num_coders].cond, &sim->dongles[self->id % sim->num_coders].mutex, &ts);
        }
        print_status(self, "has taken a dongle");

        pthread_mutex_lock(&sim->stop_mutex);
        self->last_compile_start = get_current_time(); 
        pthread_mutex_unlock(&sim->stop_mutex);

        print_status(self, "is compiling");
        usleep(sim->time_to_compile * 1000);
        self->compile_count ++;

        sim->dongles[self->id - 1].last_release = get_current_time();
        pthread_cond_signal(& sim->dongles[self->id - 1].cond);
        pthread_mutex_unlock(&sim->dongles[self->id - 1].mutex);
        sim->dongles[self->id % sim->num_coders].last_release = get_current_time();
        pthread_cond_signal(&sim->dongles[self->id % sim->num_coders].cond);
        pthread_mutex_unlock(&sim->dongles[self->id % sim->num_coders].mutex);
        
        pthread_mutex_lock(&sim->queue_mutex);
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