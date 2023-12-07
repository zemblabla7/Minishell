/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kquerel <kquerel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:32:19 by casomarr          #+#    #+#             */
/*   Updated: 2023/12/07 18:06:22 by kquerel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	new_key_loop(size_t *i, char *content, int *alpha)
{
	while (content[(*i)])
	{
		if (content[(*i)] == '$' && content[(*i) + 1] == '$')
		{
			while (content[(*i) + 1] == '$')
				(*i)++;
		}
		if (ft_isalpha(content[(*i)]) == 1)
			(*alpha) = 1;
		if (content[(*i)] == '$' && content[(*i) + 1] != '\0')
			return ;
		if (content[(*i)] == '$')
			return ;
		(*i)++;
	}
}

void	new_key(size_t *i, char **key_to_find, char *content)
{
	int	start;

	if (content[(*i)] == '\0')
	{
		if (*key_to_find)
			free(*key_to_find);
		*key_to_find = NULL;
		return ;
	}
	while (content[(*i)] && content[(*i)] != '$')
			(*i)++;
	start = *i;
	(*i)++;
	while (content[(*i)] && (content[(*i)] < 9 || content[(*i)] > 13) \
		&& content[(*i)] != 32 && content[(*i)] != '$')
			(*i)++;
	if (*key_to_find)
		free(*key_to_find);
	*key_to_find = strlcpy_middle(*key_to_find, content, start + 1, (*i) - 1);
	if (compare(*key_to_find, "?") == true)
	{
		free(*key_to_find);
		*key_to_find = ft_strdup("EXIT_STATUS");
	}
}

char	*replace_dollar(char *key_to_find, t_env *env_list)
{
	t_env	*key_in_env;
	char	*ret;

	ret = NULL;
	if (is_key_in_env(env_list, key_to_find) == true)
	{
		key_in_env = find_value_with_key_env(env_list, key_to_find);
		ret = ft_strdup(key_in_env->value);
	}
	return (ret);
}

void	text_before(char *content, char **ret)
{
	int	start;

	start = 0;
	if (content[0] != '$')
	{
		while (content[start] != '$')
			start++;
		(*ret) = strlcpy_middle((*ret), content, 0, start - 1);
	}
}

char	*text_after(char *content, size_t *j)
{
	size_t	after;
	size_t	y;
	char	*text_after;

	text_after = NULL;
	after = (*j);
	while (content[(*j)] && content[(*j)] != '$')
		(*j)++;
	if (content[(*j)] != '\0')
	{
		text_after = ft_calloc(ft_strlen(content) + 1, sizeof(char));
		if (!text_after)
			return (NULL);
		y = 0;
		while (after < *j)
			text_after[y++] = content[after++];
	}
	return (text_after);
}
