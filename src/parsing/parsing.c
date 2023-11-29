/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octonaute <octonaute@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 17:45:28 by carolina          #+#    #+#             */
/*   Updated: 2023/11/29 19:39:33 by octonaute        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/*Separates each argument in the command line in a t_element list.
Only the redirectors and spaces that separate each command are not
kept in the list. This list is then sent to the executor.
Separator is only freed qhen str_type == str because when
str_type == cmd it is not malloc'ed (in function type_of_separator).*/
t_element	*parsing(char *line, t_env *env_list)
{
	int			i;
	int			start;
	t_element	*current_cmd;
	t_element	*head;
	char		*separator;

	current_cmd = parsing_initialisation(line, &i, &start);
	head = current_cmd;
	while (line[i])
	{
		separator = type_of_separator(line, start, \
		parsing_str_type(line, start));
		if (separator[0] == '\'')
			current_cmd->change = false;
		parsing_fill_content(&current_cmd, line, &i, separator);
		if (parsing_str_type(line, start) == STR)
			free(separator);
		current_cmd->type = determine_command_type(line, i, start);
		if (current_cmd->type == HEREDOC)
		{
			if (ft_heredoc(current_cmd, env_list) == false)
				return (NULL);
		}
		parsing_advance_to_next_word(line, &start, &i);
		parsing_initialize_next(&current_cmd, line, &i);
	}
	if (head)
		parsing_fix(&head, env_list);
	if (head != NULL) /*--> comme pas bien freed dans parsing_fix avec delete_node 
	ça entre dans cette condition et fait une segfault*/
		builtin_fix(&head);
	if (head)
		return (head);
	else
		return (NULL);
}

/* bool	nothing_before(int start, int n, char *line)
{
	if (start - (n + 1) > -1)
	{
		if (line[start - (n + 1)] != ' ')
			return false;
	}
	return (true);
} */

void	type_arg_after_cmd(t_element **current)
{
	t_element	*temp;

	if ((*current)->type == COMMAND && (*current)->next)
	{
		temp = (*current)->next;
		while (temp->type != PIPE && temp != NULL)
		{
			if (temp->type != OPTION && temp->type < 3)
				temp->type = ARGUMENT;
			if (temp->next != NULL)
				temp = temp->next;
			else
				break ;
		}
	}
}

/* To fix the type of the arguments that are not in between quotes
and are therefore considered as a COMMAND instead of an ARGUMENT
in the parsing function. This functions sets all arguments that are
not of type OPTION or redirecter after a cmd to ARGUMENT until a 
type PIPE is found.*/
void	parsing_fix(t_element **cmd_list, t_env *env_list)
{
	t_element	*current;

	current = (*cmd_list);
	while (current != NULL)
	{
		if ((current->prev != NULL && current->prev->type >= 3 && \
		current->type < 3 && no_cmd_before(current) == true) || \
		(current->prev == NULL && current->type < 3))
			current->type = COMMAND;
		if (current->type == COMMAND && current->next)
			type_arg_after_cmd(&current);
		if (current->content[0] == '$' && current->change == true)
		{
			current->content = dollar(current->content, env_list);
			if (current->content == NULL)
			{
				if (ft_delete_node_cmd(cmd_list, current) == 1)
					return ;
			}
		}
		if (current)
			current = current->next; //segfault qd $$$$ : current ne se free pas bien (gdb) donc pas NULL donc entre dans la condition
	}
	return ;
}

/*Sets all arguments encountered between a cmd that is a builtin and
a pipe to "builtin = true" so that the executor skips them.*/
void	builtin_fix(t_element **cmd_list)
{
	t_element	*current;

	current = (*cmd_list);
	while (current)
	{
		if (is_builtin(current->content) == true)
			current->builtin = true;
		current = current->next;
	}
}

bool	no_cmd_before(t_element *current)
{
	while (current->prev && current->prev->type != PIPE)
	{
		if (current->prev->type == COMMAND)
			return (false);
		current = current->prev;
	}
	return (true);
}
