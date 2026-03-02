#include "codexion.h"

void    cleanup_simulation(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->num_coders)
    {
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        pthread_cond_destroy(&sim->dongles[i].cond);
        i++;
    }

    pthread_mutex_destroy(&sim->stop_mutex);
    pthread_mutex_destroy(&sim->print_mutex);
    pthread_mutex_destroy(&sim->start_mutex);
    pthread_cond_destroy(&sim->start_cond);

    if (sim->coders)
        free(sim->coders);
    if (sim->dongles)
        free(sim->dongles);
    if (sim->heap)
        free(sim->heap);
}