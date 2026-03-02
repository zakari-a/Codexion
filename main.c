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

    if (init_simulation(&sim, av, ac))
        return (1);

    i = 0;
    while (i < sim.num_coders)
    {
        sim.coders[i].last_compile_start = sim.start_time;        
        if (pthread_create(&sim.coders[i].thread, NULL,
                           routine, &sim.coders[i]) != 0)
        {
            printf("Error creating thread %d\n", i);
            return (1);
        }
        i++;
    }

    monitor_simulation(&sim);

    i = 0;
    i = 0;
    while (i < sim.num_coders)
    {
        pthread_join(sim.coders[i].thread, NULL);
        i++;
    }

    cleanup_simulation(&sim);
}