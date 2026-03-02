#ifndef CODEXION_H
#define CODEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>


typedef struct s_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    long last_release;
} t_dongle;

typedef struct s_coder
{
    int id;
    long    last_compile_start;
    long    request_time;
    int compile_count;
    int finished;
    pthread_t   thread;
    pthread_mutex_t c_mutex;
    struct s_sim    *sim;
} t_coder;

typedef struct s_sim
{
    int start;
    int ready_count;
    long    start_time;
    int stop;
    int num_coders;
    int heap_size;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    long    cooldown;
    int must_compile;
    int scheduler;
    
    pthread_mutex_t stop_mutex;
    pthread_mutex_t print_mutex;
    pthread_mutex_t queue_mutex;
    pthread_mutex_t start_mutex;
    pthread_cond_t  start_cond;
    pthread_cond_t  condition;
    
    t_coder *coders;
    t_dongle *dongles;
    t_coder **heap;
} t_sim;

//----utils----//
void    swap_coders(t_coder **heap, int i, int j);
int fifo_check(t_coder *a, t_coder *b);
int edf_check(t_sim *sim, t_coder *a, t_coder *b);
void    print_status(t_coder *self, char *message);
long    get_current_time();


//----init----//
int init_simulation(t_sim *sim, char **args);

//----heap----//
int    is_more_urgent(t_sim *sim, t_coder *a, t_coder *b);
void    heap_push(t_sim *sim, t_coder *new_coder);
void    heap_pop(t_sim *sim);

//----parser----//
int	is_positive(char *str);
int	parse(char **args, int size);

//----routine----//
void    *routine(void *arg);

//----monitor----//
void    monitor_simulation(t_sim *sim);

//----clean----//
void    cleanup_simulation(t_sim *sim);

#endif