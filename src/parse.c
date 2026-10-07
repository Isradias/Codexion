/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: icaldas- <icaldas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 00:00:00 by icaldas-          #+#    #+#             */
/*   Updated: 2026/10/15 00:00:00 by icaldas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_number(char *text, int *value)
{
	long	number;
	int		index;

	if (!text || !*text)
		return (0);
	number = 0;
	index = 0;
	while (text[index])
	{
		if (text[index] < '0' || text[index] > '9')
			return (0);
		number = number * 10 + text[index] - '0';
		if (number > INT_MAX)
			return (0);
		index++;
	}
	*value = (int)number;
	return (1);
}

static int	parse_values(char **argv, t_config *config)
{
	return (parse_number(argv[1], &config->coders)
		&& config->coders > 0 && parse_number(argv[2], &config->burnout_ms)
		&& parse_number(argv[3], &config->compile_ms)
		&& parse_number(argv[4], &config->debug_ms)
		&& parse_number(argv[5], &config->refactor_ms)
		&& parse_number(argv[6], &config->required_compiles)
		&& parse_number(argv[7], &config->cooldown_ms));
}

int	parse_config(int argc, char **argv, t_config *config)
{
	if (argc != 9 || !parse_values(argv, config))
		return (0);
	if (argv[8][0] == 'f' && argv[8][1] == 'i' && argv[8][2] == 'f'
		&& argv[8][3] == 'o' && argv[8][4] == '\0')
		config->scheduler = FIFO;
	else if (argv[8][0] == 'e' && argv[8][1] == 'd' && argv[8][2] == 'f'
		&& argv[8][3] == '\0')
		config->scheduler = EDF;
	else
		return (0);
	return (1);
}
