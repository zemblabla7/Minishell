/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octonaute <octonaute@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 17:43:38 by carolina          #+#    #+#             */
/*   Updated: 2023/10/21 13:49:09 by octonaute        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"

int	ft_lstsize_history(t_history *lst)
{
	int		len;
	t_history	*content;

	len = 0;
	content = lst;
	while (content != NULL)
	{
		content = content -> next;
		len++;
	}
	return (len);
}

t_history	*ft_lstlast_history(t_history *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next)
	{
		lst = lst->next;
	}
	return (lst);
}

void	lstadd_back_history(t_history *entries, char *line)
{
	t_history	*temp;

	temp = ft_lstlast_history(entries);
	temp->next = lstnew_history(temp, line, ft_lstsize_history(entries));
}


t_history	*ft_add_history(t_history *entries, char *line)
{
	t_history	*head;

	if (entries == NULL)
	{
		entries = lstnew_history(entries, line, 1);
		return (entries);
	}
	head = entries;
	lstadd_back_history(entries, line);
	return (head);
}

void	history(t_history *current_entry)
{
	while(current_entry != NULL)
	{
		printf("%d %s\n", current_entry->nb, current_entry->cmd);
		current_entry = current_entry->next;
	}
}

void	free_history(t_history *current_entry)
{
	while(current_entry != NULL)
	{
		free(current_entry->cmd);
		current_entry = current_entry->next;
	}
	free(current_entry);
}