/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: icaldas- <icaldas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 00:00:00 by icaldas-          #+#    #+#             */
/*   Updated: 2026/10/15 00:00:00 by icaldas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_config
{
	int			coders;
	int			burnout_ms;
	int			compile_ms;
	int			debug_ms;
	int			refactor_ms;
	int			required_compiles;
	int			cooldown_ms;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_game
{
	t_config		config;
	void			*coders;
	void			*dongles;
	pthread_mutex_t	state;
	pthread_mutex_t	log;
	pthread_cond_t	changed;
}	t_game;

int	main(int argc, char **argv);
int	parse_config(int argc, char **argv, t_config *config);
int	run_simulation(t_config config);

#endif
