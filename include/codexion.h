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
# include <stdio.h>

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

int	main(int argc, char **argv);
int	parse_config(int argc, char **argv, t_config *config);

#endif
