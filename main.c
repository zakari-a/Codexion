#include "codexion.h"

int main(int ac, char **av)
{
    t_sim sim;
    int i;

    if (parse(av, ac))
    {
        printf("Please provide a valid arguments");
        return (1);
    }

    if (init_simulation(&sim, av))
        return (1);

    i = 0;
    while (i < sim.num_coders)
    {
        if (pthread_create(&sim.coders[i].thread, NULL,
                           routine, &sim.coders[i]) != 0)
        {
            printf("Error creating thread %d\n", i);
            return (1);
        }
        i++;
    }
    pthread_mutex_lock(&sim.start_mutex);
    sim.start = 1;
    pthread_cond_broadcast(&sim.start_cond);
    while (sim.ready_count < sim.num_coders)
        pthread_cond_wait(&sim.start_cond, &sim.start_mutex);
    pthread_mutex_unlock(&sim.start_mutex);

    monitor_simulation(&sim);

    i = 0;
    while (i < sim.num_coders)
    {
        pthread_join(sim.coders[i].thread, NULL);
        i++;
    }

    cleanup_simulation(&sim);
}