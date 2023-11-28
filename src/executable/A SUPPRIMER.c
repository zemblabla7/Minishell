/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_CARO.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casomarr <casomarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 17:02:19 by kquerel           #+#    #+#             */
/*   Updated: 2023/11/28 18:37:55 by casomarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/* Being on the middle pipe(s), both fds's are sent to dup2 */
void	middle_dup(t_element *cmd, t_env **env, t_pipe *exec)
{
	if (dup2(exec->fd_temp, STDIN_FILENO) < 0)
	{
		perror("dup2");
		exit(0);
	}
	close(exec->fd_temp);
	if (dup2(exec->fd[1], STDOUT_FILENO) < 0)
	{
		perror("dup2");
		exit(0);
	}
	close(exec->fd[1]);
	handle_command(cmd, env, exec);
}

/* Being on the middle pipe(s), only stdin fd is sent to dup2 */
void last_dup(t_element *cmd, t_env **env, t_pipe *exec)
{
	if (dup2(exec->fd_temp, STDIN_FILENO) < 0)
		perror("dup last");
	close(exec->fd_temp);
	handle_command(cmd, env, exec);
}

/* Joins every splitted string from the PATH= env variable with the command
	then tests its validity with access */
char	*ft_get_command(char **path, char *argument)
{
	char	*to_free;
	char	*to_return;
	int		i;

	i = 0;
	if (argument && path)
	{
		while (path[i])
		{
			to_free = ft_strjoin(path[i], "/");
			to_return = ft_strjoin(to_free, argument);
			free(to_free);
			if (access(to_return, F_OK) == 0)
				return (to_return);
			free(to_return);
			i++;
		}
	}
	return (NULL);
}

/* Redirects command based on its input
--- if a redirection is detected, ft_redirect is called
--- if a builtin is detected, ft_builtins is called
---	if the cmd is not empty, exec_command is called
*/
void	handle_command(t_element *cmd, t_env **env, t_pipe *exec)
{
	t_env	*exit_status;
	int	exit_nb;

	exit_nb = 0;
	if (!ft_redirect(cmd, exec))
	{
		// free
		exit(1);
	}
	exit_status = find_value_with_key_env(*env, "EXIT_STATUS");
	if (cmd->builtin == true)
		return (ft_builtins(cmd, env, exec), close(exec->fd[0]), exit(ft_atoi(exit_status->value)));
	if (exec->cmd_tab[0] != NULL)
		exit_nb = add_exit_status_in_env(env, exec_command(cmd, *env, exec));
	exit(exit_nb);
}

/* Executes the command
---	split_path returns the "PATH=" variable from env, split the string by :
	and assigns it to cmd_path.
---	ft_get_command tests all paths variable from cmd_path, joins it with
	the command cmd_tab[i] and tests its validity with access.
---	Execve replaces the existing process by the program within his first argument
	1st arg: cmd->content, pointer to a string that specifies the path
		to the exec file, can be binary or script file.
	2nd arg: array of strings (char **) representing the command-line of arguments
		of the new program.
	3st arg: array of strings (char **) representing the environment variables
		for the new program.
--- Waitpid waits for the process to end before continuing.
 */
int	exec_command(t_element *cmd, t_env *env, t_pipe *exec)
{
	if (ft_strchr(exec->cmd_tab[0], '/'))
	{
		execve(cmd->content, exec->cmd_tab, env->env);
		perror("bash");
		//free_cmd_list(cmd);
		free_cmd_arr(exec);
		add_exit_status_in_env(&env, 127);
		return(127);
	}
	exec->cmd_path = split_path(env);
	// if (!exec->cmd_path)
	// 	exec_command_continued(exec, 0);
	cmd->content = ft_get_command(exec->cmd_path, exec->cmd_tab[0]);
	if (!cmd->content)
	{
		// free_cmd_list(cmd);
		if (!exec->cmd_tab[0])
			ft_putstr_fd("\n", STDERR_FILENO);
		else
		{
			//free(exec->cmd_path); //caro
			//free(exec->cmd_tab); //caro
			//free(env->env); //caro
			add_exit_status_in_env(&env, 127);
			command_not_found(exec);
		}
	}
	else
	{
		execve(cmd->content, exec->cmd_tab, env->env);
		// printf("wtf je passe par ici\n");
		perror("bash");
		// free_cmd_list(cmd);
	}
	add_exit_status_in_env(&env, 127);
	return (127);
}

/* exec continued */
int	command_not_found(t_pipe *exec)
{
	// !!!!!!!! leaks a fix
	ft_putstr_fd("bash: ", STDERR_FILENO);
	ft_putstr_fd(exec->cmd_tab[0], STDERR_FILENO);
	ft_putendl_fd(": command not found", STDERR_FILENO);
	// free_cmd_arr(exec);
	// free(exec->cmd_tab[0]);
	return (127);
}