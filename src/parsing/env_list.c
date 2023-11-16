/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octonaute <octonaute@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 17:00:17 by casomarr          #+#    #+#             */
/*   Updated: 2023/11/16 15:35:47 by octonaute        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"

int	put_env_in_list_loop(char **env, t_env **current, int line)
{
	int	letter;
	int	i;

	letter = 0;
	i = 0;
	while (env[line][letter] != '=')
		(*current)->key[i++] = env[line][letter++];
	(*current)->key[i] = '\0';
	letter++;
	i = 0;
	while (env[line][letter] != '\0')
		(*current)->value[i++] = env[line][letter++];
	(*current)->value[i] = '\0';
	line++;
	if (env[line] != NULL)
	{
		(*current)->next = lstnew_env(env[line], 0);
		(*current)->next->prev = (*current);
		(*current) = (*current)->next;
	}
	return (line);
}

/*Cuts each line of **env into key and value in a t_env list*/
t_env	*put_env_in_list(char **env)
{
	t_env	*head;
	t_env	*current;
	int		line;

	current = lstnew_env(env[0], 0);
	head = current;
	line = 0;
	while (env[line] != NULL)
		line = put_env_in_list_loop(env, &current, line);
	current->next = NULL;
	return (head);
}

/*Parses all the list until it finds a matching key*/
t_env	*find_value_with_key_env(t_env *env_list, char *key)
{
	int	i;

	i = 0;
	while (env_list != NULL)
	{
		i = 0;
		while (env_list->key[i] == key[i])
		{
			i++;
			if (key[i] == '\0')
				return (env_list);
		}
		env_list = env_list->next;
	}
	return (NULL);
}

/*Checks if the arg following $ in the command line has a 
matching key in **env*/
bool	is_key_in_env(t_env *env_list, char *key)
{
	t_env	*tmp;

	if (!key || !env_list)
		return (false);
	tmp = env_list;
	while (tmp)
	{
		if (strncmp(key, tmp->key, ft_strlen(key)) == 0 && \
		ft_strlen(key) == ft_strlen(tmp->key))
			return (true);
		tmp = tmp->next;
	}
	return (false);
}
