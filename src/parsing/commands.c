/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casomarr <casomarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 17:42:36 by carolina          #+#    #+#             */
/*   Updated: 2023/10/12 16:44:30 by casomarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"

char	*commands(char *line, t_env *env_list, char *home_path)
{
	/*a la base j avais fait " cd " etc donc avec espace apres le 
	nom du builtin pour eviter les erreur du type "holacdhey" et 
	que ca compte comme cd mais pb : cd tout seul sans espace avant
	car premiere cmd de la ligne et sans espace apres car c est a 
	moi de remplacer par le home_path alors ca ne marche pas*/
	if (is_cmd_in_line(line, "$") == true)
		line = dollar(line, env_list);
	else if (is_this_command(line, "history") == true)
		history(HISTORY);
	else if (is_this_command(line, "pwd") == true)
		pwd();
	else if (is_cmd_in_line(line, "cd") == true)
		line = cd(line, env_list, home_path);
	else if (is_cmd_in_line(line, "echo") == true)
		echo(line);
	else if (is_cmd_in_line(line, ">") == true || is_cmd_in_line(line, "<") == true)
		redirecters_error(line);
	else if (line[0] == '<' || line[0] == '>' || \
	line[0] == '/' || line[0] == '|' || line[0] == '&' || \
	line[0] == '\'' || line[0] == '\"')
		first_character_error(line);
	return (line);
}

/*Checks if what is written in the command line corresponds to a command*/
bool	is_this_command(char *buffer, char* command)
{
	size_t	i;

	i = 0;
	if (!buffer)
		return (false);
	while (buffer[i])
	{
		if (buffer[i] != command[i])
			return (false);
		i++;
	}
	if (ft_strlen(buffer) == ft_strlen(command) && i == ft_strlen(buffer))
		return (true);
	else
		return (false);
}

/*returns the size of a cmd, key, value or str*/
int	size_of_command(char *command, int len, int type)
{
	int	size;

	//printf("%sSIZEOFCOMMAND\n%s", BCYN, RESET);
	size = 0;
	if (type == CMD)
	{
		// if (command[len] != '\0') /*j'ai rajoute ca l'autre jour qui a regler 
		// mon pb de valgrind qd j'ecrivais juste "cd" mais j'ai tjrs une erreur 
		// a cette ligne qd j essayes de faire "cd src"*/
		// {
		// printf("%slen = %d, command[len] = %c\n%s", YELLOW, len, command[len], RESET);
		while (command[len] != ' ' && command[len] != '\0')
		{
			//printf("%scommand[len] = %c\n%s", YELLOW, command[len], RESET);
			//printf("%ssize = %d, command[len] = %c\n%s", YELLOW, size, command[len], RESET);
			size++;
			len++;
		}
		// printf("%ssize_of_command dans sizeofcmd = %d\n%s", YELLOW, size + 1, RESET);
		//printf("%sTOTAL LEN IS %d\n%s", GREEN, size, RESET);
		// }
		//size = size - 1; // (si je vois que erreur de valgrind qd type est KEY, VALUE et STR aussi alors peut etre qu'il faut enlever le + 1 dans la rturn final et c'est tout)
	}
	else if (type == KEY)
	{
		while (command[len] != '=')
		{
			size++;
			len++;
		}
	}
	else if (type == VALUE)
	{
		while (command[len] != '=')
			len++;
		len++;
		while (command[len])
		{
			size++;
			len++;
		}
	}
	else if (type == STR)
	{
		while(command[len])
		{
			if (command[len + 1] == '\0' && (command[len + 1] != '\'' || command[len + 1] != '\"'))
				break;
			size++;
			len++;
		}
	}
	return(size + 1);
}

/*Checks if a given commands is in the command line*/
bool	is_cmd_in_line(char *line, char *cmd)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!cmd || !line)
		return (false);
	while (line[i] != '\0')
	{
		j = 0;
		while (line[i + j] == cmd[j] && cmd[j] != '\0' && line[i + j] != '\0')
		{
			j++;
			if (cmd[j] == '\0')
				return (true);
		}
		i++;
	}
	return (false);
}

/*Returns the position of the end of the cmd found in the cmd line
Returns 0 in case of error since the end of a cmd can't be at the
beggining of the cmd line*/
int	where_is_cmd_in_line(char *line, char *cmd)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!cmd || !line)
		return (false);
	while (line[i] != '\0')
	{
		j = 0;
		while (line[i + j] == cmd[j] && cmd[j] != '\0' && line[i + j] != '\0')
		{
			j++;
			if (cmd[j] == '\0')
				return (i + j); // i + j = fin de cmd
		}
		i++;
	}
	return (0);
}