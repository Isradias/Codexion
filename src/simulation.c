/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: icaldas- <icaldas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 00:00:00 by icaldas-          #+#    #+#             */
/*   Updated: 2026/10/15 00:00:00 by icaldas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

typedef struct s_dongle { pthread_mutex_t lock; int busy; long ready; } t_dongle;
typedef struct s_coder { pthread_t thread; t_game *game; int id; int count; long last; } t_coder;
typedef struct s_data { t_game game; t_coder *coders; t_dongle *dongles; long start; int stop; } t_data;

static long now(void) { struct timeval v; gettimeofday(&v, NULL); return (v.tv_sec * 1000L + v.tv_usec / 1000L); }
static t_data *data(t_game *game) { return ((t_data *)game); }
static void print(t_coder *c, char *s) { t_data *d=data(c->game); pthread_mutex_lock(&d->game.log); if (!d->stop) printf("%ld %d %s\n", now()-d->start,c->id,s); pthread_mutex_unlock(&d->game.log); }
static void nap(t_data *d, int ms) { long end=now()+ms; while (!d->stop && now()<end) usleep(500); }
static int take(t_coder *c) { t_data *d=data(c->game); int l=c->id-1,r=c->id%d->game.config.coders; pthread_mutex_lock(&d->game.state); while (!d->stop && (d->dongles[l].busy || d->dongles[r].busy || now()<d->dongles[l].ready || now()<d->dongles[r].ready)) { pthread_mutex_unlock(&d->game.state); usleep(500); pthread_mutex_lock(&d->game.state); } if(d->stop){pthread_mutex_unlock(&d->game.state);return 0;} d->dongles[l].busy=1; d->dongles[r].busy=1; pthread_mutex_unlock(&d->game.state); print(c,"has taken a dongle"); print(c,"has taken a dongle"); return 1; }
static void give(t_coder *c) { t_data *d=data(c->game); int l=c->id-1,r=c->id%d->game.config.coders; pthread_mutex_lock(&d->game.state); d->dongles[l].busy=0; d->dongles[r].busy=0; d->dongles[l].ready=now()+d->game.config.cooldown_ms; d->dongles[r].ready=d->dongles[l].ready; pthread_cond_broadcast(&d->game.changed); pthread_mutex_unlock(&d->game.state); }
static void *worker(void *arg) { t_coder *c=arg; t_data *d=data(c->game); while(!d->stop&&take(c)){ pthread_mutex_lock(&d->game.state); c->last=now(); c->count++; pthread_mutex_unlock(&d->game.state); print(c,"is compiling"); nap(d,d->game.config.compile_ms); give(c); print(c,"is debugging"); nap(d,d->game.config.debug_ms); print(c,"is refactoring"); nap(d,d->game.config.refactor_ms); } return NULL; }
static void *monitor(void *arg) { t_data *d=arg; int i,all; while(!d->stop){i=0;all=1;pthread_mutex_lock(&d->game.state);while(i<d->game.config.coders){if(d->coders[i].count<d->game.config.required_compiles)all=0;if(now()>=d->coders[i].last+d->game.config.burnout_ms){pthread_mutex_lock(&d->game.log);printf("%ld %d burned out\n",now()-d->start,d->coders[i].id);pthread_mutex_unlock(&d->game.log);d->stop=1;break;}i++;}if(all)d->stop=1;pthread_cond_broadcast(&d->game.changed);pthread_mutex_unlock(&d->game.state);usleep(500);}return NULL;}
int run_simulation(t_config config) { t_data d; pthread_t mon; int i; memset(&d,0,sizeof(d));d.game.config=config;d.coders=malloc(sizeof(*d.coders)*config.coders);d.dongles=malloc(sizeof(*d.dongles)*config.coders);if(!d.coders||!d.dongles)return(free(d.coders),free(d.dongles),1);pthread_mutex_init(&d.game.state,NULL);pthread_mutex_init(&d.game.log,NULL);pthread_cond_init(&d.game.changed,NULL);d.start=now();i=0;while(i<config.coders){pthread_mutex_init(&d.dongles[i].lock,NULL);d.coders[i].game=&d.game;d.coders[i].id=i+1;d.coders[i].last=d.start;pthread_create(&d.coders[i].thread,NULL,worker,&d.coders[i]);i++;}pthread_create(&mon,NULL,monitor,&d);i=0;while(i<config.coders)pthread_join(d.coders[i++].thread,NULL);pthread_join(mon,NULL);i=0;while(i<config.coders)pthread_mutex_destroy(&d.dongles[i++].lock);pthread_mutex_destroy(&d.game.state);pthread_mutex_destroy(&d.game.log);pthread_cond_destroy(&d.game.changed);free(d.coders);free(d.dongles);return 0;}
