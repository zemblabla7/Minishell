/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kquerel <kquerel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 14:46:12 by kquerel           #+#    #+#             */
/*   Updated: 2023/10/25 18:07:33 by kquerel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"


void	ft_execute(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	i;
	i = 0;

	// if (!cmd)
	// 	return ;
	exec->pipe_nb = ft_count_pipes(cmd);
	printf("%spipe_nb = %d\n%s", YELLOW, exec->pipe_nb, RESET);
    exec->cmd_tab = ft_calloc(exec->pipe_nb + 2, sizeof(char *)); // +1 pour pour le nombre de commandes + 1 pour le NULL
	if (!exec->cmd_tab)
		return ;
	fill_cmd_tab(cmd, exec);
	if (exec->pipe_nb == 0)
		single_command(cmd, env, exec, entries);
	else
		mult_commands(cmd, env, exec, entries);
}

void	mult_commands(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	i = 0;
	int	fd_temp;
	int	fd[2];
	
	while (i <= exec->pipe_nb)
	{
		if ( i < exec->pipe_nb)
		{
			middle_pipes(fd, &fd_temp, cmd, env, exec, entries);
			if (cmd->next && cmd->next->next)
				cmd = cmd->next->next;
		}
		else
			last_pipe(fd, &fd_temp, cmd, env, exec, entries);
		i++;
	}
	wait(NULL);
}

void	middle_pipes(int *fd, int *fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	pid;

	if (pipe(fd) < 0)
		perror("Pipe");
	pid = fork();
	if (pid < 0)
		perror("Fork");
	if (pid == 0)
		ft_child(fd, *fd_temp, cmd, env, exec, entries);
	else
	{
		if (*fd_temp)
			close(*fd_temp);
		*fd_temp = dup(fd[0]);
		close(fd[0]);
		close(fd[1]); // nouveau
		// waitpid(pid, NULL, 0);
	}
}

void	last_pipe(int *fd, int *fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	pid;

	pid = fork();
	if (pid < 0)
		perror("fork");
	if (pid == 0)
		exec_last_cmd(fd, *fd_temp, cmd, env, exec, entries);
	else
	{
		if (*fd_temp)
			close(*fd_temp);
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
}

void exec_last_cmd(int *fd, int fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	if (dup2(fd_temp, STDIN_FILENO) < 0)
		perror("dup");
	(void)fd;
	close(fd_temp);
	handle_command(cmd, env, exec, entries);
}
void	ft_child(int *fd, int fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	if (dup2(fd_temp, STDIN_FILENO) < 0)
	{
		printf("error\n");
		exit(0);
	}
	close(fd[0]);
	if (dup2(fd[1], STDOUT_FILENO) < 0)
	{
		printf("error\n");
		exit(0);
	}
	close(fd_temp);
	close(fd[1]);
	handle_command(cmd, env, exec, entries);
	//execution(cmd, env, exec, entries);
}

// void execution(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
// {
// 	if (cmd && cmd->builtin == true)
// 	{
// 		commands(cmd, env, entries);
// 		return ;
// 	}
	
// }

void	single_command(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	pid;
	int status;

	if (cmd && cmd->builtin == true)
	{
		commands(cmd, env, entries);
		return ;
	}
	pid = fork();
	if (pid < 0)
	{
		printf("error\n");
		exit(127);
	}
	if (pid == 0)
		handle_command(cmd, env, exec, entries);
	waitpid(pid, &status, 0);
	// if (WIFEXITED(status))
	// 	g_global.error_num = WEXITSTATUS(status);
}

void	handle_command(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries)
{
	int	exit_code;
	
	// A VIRER (ne plus envoyer entries)
	if (cmd->builtin == true)
	{
		commands(cmd, env, entries);
		return ;
	}
	exit_code = 0;
	if (exec->cmd_tab[0][0] != '\0')
		exit_code = exec_command(cmd, env, exec);
	exit(exit_code);
}

int	exec_command(t_element *cmd, t_env *env, t_pipe *exec)
{
	exec->cmd_path = split_path(env);
	if (!exec->cmd_path)
	{
		printf("Split_path failed\n");
		// free des trucs
		return (0);
	}
	cmd->content = ft_get_command(exec->cmd_path, exec->cmd_tab[0]);
	if (!cmd->content)
	{
		if (!exec->cmd_tab[0])
			ft_putstr_fd("\n", 2);
		else
		{
			ft_putstr_fd(exec->cmd_tab[0], 2);
			ft_putstr_fd(": command not found\n", 2);
		}
	}
	ft_print_array(exec->cmd_tab);
	if (execve(cmd->content, exec->cmd_tab, env->env) == -1)
		ft_putstr_fd("execve failed\n", STDOUT_FILENO);
	return (1); //return a exit code, faire une fonction cmd not found
}

int	ft_count_pipes(t_element *cmd)
{
	int	pipe_nb;
	pipe_nb = 0;
	
	if (!cmd)
		return (0);
	while (cmd)
	{
		if (cmd->type == PIPE)
			pipe_nb++;
		cmd = cmd->next;
	}
	return (pipe_nb);
}

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
			// printf("path[%d] = %s\n", i, to_return);
			if (access(to_return, F_OK) == 0)
				return (to_return);
			free(to_return);
			i++;
		}
	}
	return (NULL);
}

/* Checks if there is a pipe before */
bool	ft_is_a_pipe_before(t_element *cmd)
{
	while (cmd)
	{
		if (cmd->type == PIPE)
			return (true);
		cmd = cmd->prev;
	}
	return (false);
}

/* Checks if there is a pipe after */
bool	ft_is_a_pipe_after(t_element *cmd)
{
	while (cmd)
	{
		if (cmd->type == PIPE)
			return (true);
		cmd = cmd->next;
	}
	return (false);
}

/* Malloc all the pipes */
bool	init_pipes(t_pipe *exec)
{
	int	i;

	i = 0;
	if (exec->cmd_nb == 1)
	{
		exec->my_pipes = NULL;
		return (true);
	}
	exec->my_pipes = ft_calloc(2, sizeof(int *));
	if (!exec->my_pipes)
	{	
		ft_putendl_fd("Malloc failed\n", STDERR_FILENO);
		// free
		return (false);
	}
	exec->my_pipes[0] = ft_calloc (2, sizeof(int)); // on malloc 2 fd pour chaque pipe
	if (!exec->my_pipes[0])
	{
		ft_putendl_fd("Malloc failed\n", STDERR_FILENO);
		// free
		return (false);
	}
	exec->my_pipes[1] = ft_calloc (2, sizeof(int)); // on malloc 2 fd pour chaque pipe
	if (!exec->my_pipes[1])
	{
		ft_putendl_fd("Malloc failed\n", STDERR_FILENO);
		// free
		return (false);
	}
	while (i < exec->cmd_nb)
	{
		//exec->my_pipes[0];
		exec->my_pipes[i][0] = 0; // fd[0]
		exec->my_pipes[i][1] = 0; // fd[1]
		i++;
	}
	return (true);
}
