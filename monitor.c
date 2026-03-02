#include "codexion.h"

void monitor_simulation(t_sim *sim)
{
    int finished_count;
    long current_time;
    int i;

    finished_count = 0;
    while (sim->stop == 0)
    {
        finished_count = 0;
        i = 0;
        while(i < sim->num_coders)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            if (sim->coders[i].finished == 1)
            finished_count += 1;
        
            else if (get_current_time() - sim->coders[i].last_compile_start > sim->time_to_burnout)
            {
                current_time = get_current_time() - sim->start_time;
                sim->stop = 1;
                pthread_mutex_lock(&sim->print_mutex);
                printf("%ld %d burned out\n", current_time, sim->coders[i].id);
                pthread_mutex_unlock(&sim->stop_mutex);
                return ;
            }
            pthread_mutex_unlock(&sim->stop_mutex);
            i++;
        }
        if (finished_count == sim->num_coders)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            sim->stop = 1;
            pthread_mutex_unlock(&sim->stop_mutex);
            return ;
        }
        usleep(500);
     }
}
