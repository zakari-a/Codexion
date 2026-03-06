#include "codexion.h"

void monitor_simulation(t_sim *sim)
{
    int     finished_count;
    long    current_time;
    int     i;

    while (1)
    {
        pthread_mutex_lock(&sim->stop_mutex);
        if (sim->stop)
        {
            pthread_mutex_unlock(&sim->stop_mutex);
            return ;
        }
        pthread_mutex_unlock(&sim->stop_mutex);

        pthread_mutex_lock(&sim->start_mutex);
        if (!sim->start)
        {
            pthread_mutex_unlock(&sim->start_mutex);
            usleep(500);
            continue ;
        }
        pthread_mutex_unlock(&sim->start_mutex);

        finished_count = 0;
        i = 0;
        while (i < sim->num_coders)
        {
            pthread_mutex_lock(&sim->coders[i].c_mutex);
            if (sim->coders[i].finished)
                finished_count++;
            else if (get_current_time() - sim->coders[i].last_compile_start
                     > sim->time_to_burnout)
            {
                pthread_mutex_unlock(&sim->coders[i].c_mutex);
                current_time = get_current_time() - sim->start_time;
                pthread_mutex_lock(&sim->print_mutex);
                printf("%ld %d burned out\n", current_time, sim->coders[i].id);
                pthread_mutex_unlock(&sim->print_mutex);
                pthread_mutex_lock(&sim->stop_mutex);
                sim->stop = 1;
                pthread_mutex_unlock(&sim->stop_mutex);
                pthread_mutex_lock(&sim->queue_mutex);
                pthread_cond_broadcast(&sim->condition);
                pthread_mutex_unlock(&sim->queue_mutex);
                return ;
            }
            pthread_mutex_unlock(&sim->coders[i].c_mutex);
            i++;
        }
        if (finished_count == sim->num_coders)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            sim->stop = 1;
            pthread_mutex_unlock(&sim->stop_mutex);
            pthread_mutex_lock(&sim->queue_mutex);
            pthread_cond_broadcast(&sim->condition);
            pthread_mutex_unlock(&sim->queue_mutex);
            return ;
        }
        usleep(500);
    }
}