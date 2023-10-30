/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: karl <karl@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 17:11:19 by carolina          #+#    #+#             */
/*   Updated: 2023/10/30 19:40:13 by karl             ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

/*Libraries*/


#ifndef MINISHELL_H
# define MINISHELL_H

# define _XOPEN_SOURCE 700 //sans ça, erreur de signaux

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/uio.h>
# include <fcntl.h>
# include <errno.h>
# include <limits.h>

/*Macros pour signaux*/
# define IN_PROMPT 0
# define IN_COMMAND 1
# define IN_HEREDOC 2

//extern int	global_location;
//extern int	g_exit_status;

/*Macros*/
# define COMMAND 0
# define OPTION 1
# define ARGUMENT 2 //argument (str or char inside single or double quotes)
# define INFILE 3 // < next commands based on infile's contents
# define OUTFILE 4 // > puts result inside the outfile
# define INFILE_DELIMITER 5 // << reads from infile until delimiter found
# define OUTFILE_APPEND 6 // >> redirects output in append mode
# define PIPE 7 //separator

# define KEY 0
# define VALUE 1
# define CMD 2
# define STR 3

# define NO_OPTIONS 0
# define ENV 1
# define ECHO 2
# define HISTORY 3
//# define les autres 3

/* # define HISTORY 0
# define FREE_HISTORY 1 */

# define PRINT 0
# define NO_PRINT 1

# define SPACES 0
# define QUOTES 1

/* # define BEGINING 0
# define END 1
# define MIDDLE 2 */

# define YELLOW	"\033[33m"
# define GREEN	"\033[32m"
# define RESET	"\033[0m"

/* Colors */
# define BYEL "\e[1;33m"
# define BRED "\e[1;31m"
# define BWHT "\e[1;37m"
# define BCYN "\e[1;36m"
# define WHT "\e[0;37m"
# define BGRE "\033[1m\033[32m"

#define PC "             ________________________________________________\n            /                                                \\\n           |    _________________________________________     |\n           |   |                                         |    |\n           |   |  Welcome                                |    |\n           |   |    to                                   |    |\n           |   |      MinisHELL                          |    |\n           |   |        by                               |    |\n           |   |          casomarr                       |    |\n           |   |            and                          |    |\n           |   |              kquerel                    |    |\n           |   |                                         |    |\n           |   |                                         |    |\n           |   |                                         |    |\n           |   |                                         |    |\n           |   |                                         |    |\n           |   |_________________________________________|    |\n           |                                                  |\n            \\_________________________________________________/\n                   \\___________________________________/\n                ___________________________________________\n             _-'    .-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.  --- `-_\n          _-'.-.-. .---.-.-.-.-.-.-.-.-.-.-.-.-.-.-.--.  .-.-.`-_\n       _-'.-.-.-. .---.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-`__`. .-.-.-.`-_\n    _-'.-.-.-.-. .-----.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-----. .-.-.-.-.`-_\n _-'.-.-.-.-.-. .---.-. .-------------------------. .-.---. .---.-.-.-.`-_\n:-------------------------------------------------------------------------:\n`---._.-------------------------------------------------------------._.---'"


/*Structures*/

/* Command list
--> content represents the command
--> type is the command type
--> cmd_tab is an array of all commands (type 0)
*/
typedef struct s_history
{
	int nb;
	char *cmd;
	struct s_history *next;
} t_history;


typedef struct s_element
{
	char	*content;
	int		type;
	bool	builtin;
	struct s_element *prev;
	struct s_element *next;
	struct s_pipe *exec;
	struct s_env *env;
	//int		location; //pour les signaux
}	t_element;

/* Environment
--> key is a substring to find in value
--> value is the env as a whole
--> env is the env (for execve) // apparemment il me faut l'env de bash et non l'env actuel
*/
typedef struct s_env
{
	char	*key;
	char	*value;
	char	**env;
	struct s_env *prev;
	struct s_env *next;
	int		exit_status;
}	t_env;

/* To handles pipes
--> pid is the id process
--> pipe_end is an array of both ends of a pipe
--> av_nb is the arg number
--> here_doc // handles here_doc creation a faire
-->
*/
typedef struct s_pipe
{
	int		here_doc;
	int		pipe_nb;
	int		cmd_nb;
	pid_t		*pid;
	char	**cmd_tab;
	char	**cmd_path;
	// int		fd_infile;
	// int		fd_outfile;
	int		*fd_temp;
	int		fd[2];
	char 	**env;
	struct s_element *cmd;
	struct s_env *env_s;
}	t_pipe;

/*-------------------MAIN FOLDER-------------------*/

/*Main*/
void	ft_welcome(void);
char	*home_path_simplified(char *absolute_path, t_env *env_list);

/*------------------PARSING FOLDER------------------*/

/*Commands*/
void	ft_builtins(t_element *cmd, t_env *env_list, t_history *entries);
bool	is_this_command(char *buffer, char* command);
int		size_of_command(char *command, int len, int type);
bool	is_cmd_in_line(char *line, char *cmd);
// int 	where_is_cmd_in_line(char *line, char *cmd);
bool	line_errors_and_fix(char **line);

/*Parsing*/
void	printlist_test(t_element   *head); //A EFFACER A LA FIN
int		determine_command_type(char *line, size_t i, size_t start);
t_element	*parsing(char *command, t_env *env_list);
t_element	*parsing_fix(t_element *cmd_list, t_env *env_list);
t_element	*builtin_fix(t_element *cmd_list);

/*Env_list*/
t_env   *put_env_in_list(char **env);
t_env   *find_value_with_key_env(t_env *env_list, char *key);
bool	is_key_in_env(t_env *env_list, char *key);

/*Lstnew*/
t_element	*lstnew(char *line, int i, int type);
t_env		*lstnew_env(char *line, int i);
t_history	*lstnew_history(char *line, int size_of_list);
int			ft_lstsize_history(t_history *lst);

/*Free*/
void	final_free(char *line, t_env *env_list, t_history *entries);
void	free_cmd_list(t_element *cmd_list);
void	exit_free(t_element *cmd_list, t_env *env_list, t_history *entries);

/*Checks*/
bool		quotes_can_close(char *line);
bool		is_builtin(char *cmd_content);
bool		is_user_in_path(char *path, t_env *env_list);

/*Signal*/
void		signal_handler(int signal);
void		ctrlD(char *line);

/*Errors*/
bool	first_character_error(char *line);
bool	redirecters_error(char *line);
void	slash_error(char *line);
void	pipe_error(char *line);
void	and_error(char *line);
void	str_error(char *line);
bool	pipe_double_or_eof(char *line);

/*Utils*/
char	*ft_joinstr_minishell(char *line, int len, char *str, char type);
char	*ft_join_pour_cd(char *line_begining, char *path);
char	*erase_spaces(char *line);
char 	*strlcpy_middle(char *dst, const char *src, size_t start, size_t end);

/*Malloc*/
char	*erase_spaces_malloc(char *line);
char	*joinstr_minishell_malloc(char *line, int len, char *str, char type);


/*Cmd_types*/
int		cmd_type(char *command, int len);
int		key_and_value_type(char *command, int len, int type);
int		str_type(char *command, int len);

/*Prompt*/
void	prompt(t_env *env_list);

/*------------------BUILT-INS FOLDER------------------*/

/*Cd*/
char	*split_at_user(char *big_path, char *user);
void	cd_home(t_env *env_list);
char	*fix_path_if_spaces(char *path);
void	cd_directory(char *path, t_env *env_list);
void	cd(t_element *current, t_env *env_list);
void	go_forward_until_user(char *current_path, char *home_value);
void	go_backwards_until_user(char *current_path, char *home_value);
size_t	size_of_word(char *path, int i);

/*Dollar*/
char	*dollar(char *content, t_env *env_list);
void	ft_dollar_question_mark(t_env *env);

/*Echo*/
//int 	skip(char *line, int i, int option);
char	type_of_separator(char *line, int i);
int		type_of_str(char *cmd);
void	echo(t_element *current);

/*Env*/
void	ft_env(t_env *env, t_element *cmd, int option);

/*Exit*/
int	ft_exit();

/*Export*/
int		ft_export(t_element *cmd_list, t_env *env);
bool	ft_is_valid_key_var(char *s);
char 	**new_env_var(char *s);
int		join_new_var(t_env *env, char *key, char *content);
void	init_new_var(t_env *env, char *new_var);

/*History*/
t_history	*ft_add_history(t_history *entries, char *line);
void		history(t_history *current_entry, int len);
void		free_history(t_history *current_entry);
void		lstadd_back_history(t_history *entries, char *line);
t_history	*ft_lstlast_history(t_history *lst);

/*Pwd*/
char	*pwd(/* t_element *cmd, */int option);
t_env	*pwd_update_in_env(/* t_element *cmd, */t_env *env_list);
//void	pwd_update_in_env(t_env **env_list);

/*Unset*/
int		ft_unset(t_element *cmd_list, t_env *env);
void	ft_delete_node(t_env **head, t_env *to_delete);

/*Builtins_errors*/
bool	check_next_node_builtin(t_element *cmd, int option);
bool	ft_is_num(char *s);
bool	ft_atoi_check(char *str);

/*-----------------EXECUTABLE FOLDER ------------------*/

/*Utils*/
char	*ft_strcpy(char *dst, char *src);
void	ft_print_array(char **arr);

/*Exec*/
void	ft_execute(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
void	single_command(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
void	multiple_commands(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
void	ft_set_exit_status(t_env *env, int status);
// void	middle_pipes(int *fd, int *fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
// void	last_pipe(int *fd, int *fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);

void	middle_pipes(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
void	last_pipe(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);

/*Exec_continued*/
// void	middle_dup(int *fd, int fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
// void	last_dup(int *fd, int fd_temp, t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);

void	middle_dup(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
void	last_dup(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);

void	handle_command(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
int		exec_command(t_element *cmd, t_env *env, t_pipe *exec);
char	*ft_get_command(char **path, char *argument);

/*Exec_errors*/
void	msg_error(int err);

/*Exec_utils*/
char	**split_path(t_env *env_list);
void	fill_array(t_element *cmd, t_pipe *exec);
int		get_size_cmd(t_element *cmd);
int		ft_count_pipes(t_element *cmd);

/*Redirect*/
int		ft_redirect(t_element *s);
int		ft_infile(char *file);
int		ft_outfile(t_element *cmd);

// void	ft_remove_var(t_element *cmd_list, t_env *env, char *to_remove);

/* NE PAS EFFACER
void	ft_close(int *fd);
void	ft_close_pipe(int pip[2]);
void	ft_close_all_pipes(t_pipe *exec);
bool	ft_is_a_pipe_after(t_element *cmd);
bool	ft_is_a_pipe_before(t_element *cmd);
bool	ft_redir(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries, int i);
bool	init_pipes(t_pipe *exec);
void	ft_fork(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries, int end[2], int fd_in);
void	ft_dup(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries, int end[2], int fd_in);
int		executor(t_element *cmd, t_env *env, t_pipe *exec, t_history *entries);
int		pipe_wait(int *pid, int amount);
*/

#endif