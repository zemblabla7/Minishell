/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casomarr <casomarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:55:33 by casomarr          #+#    #+#             */
/*   Updated: 2023/10/10 18:03:39 by casomarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"

/*
1. Checks if the $ is followed by alphanumeric and preceded by a space
2. Puts what follows the $ into the variable key.
3. Cheks that the key exists in env_list (and node "takes its rank").
4. Replaces the key ($...) by its value in the line (returns new_line).
*/
char	*dollar(char *line, t_env *env_list)
{
	int		i;
	int		j;
	int		len;
	char	*key;
	char	*new_line;
	t_env	*node;

	i = where_is_cmd_in_line(line, "$");
	if (i == 0 || ft_isalpha(line[i + 1]) == 0 || line[i - 1] != ' ')
		return ("\n"); //error
	i ++; //now i = beggining of the key
	j = 0;
//trouver la key
	key = malloc(sizeof(char) * size_of_command(line, i, KEY) + 1);
	if (!key)
		return (NULL);
	while(line[i] && (line[i + 1] != ' '))
		key[j++] = line[i++];
	key[j] = '\0';
//check si la key existe dans env
	if (is_key_in_env(env_list, key) == false)
		return ("\n"); // meme si autres commandes dans la ligne, ca n imprime rien, juste une ligne vide!!
//chercher la key dans env
	node = find_value_with_key_env(env_list, key);
// remplacer key par sa value a l'interieur de la ligne (pour rendre new_line)
	i = where_is_cmd_in_line(line, "$");
	j = 0;
	len = 0;
	new_line = malloc(sizeof(char) * (ft_strlen(line) - size_of_command(line, i, CMD) + ft_strlen(node->value)) + 1);
	if (!new_line)
		return (NULL);
	while(line[i + 1] != '$')
		new_line[len++] = line[i++];
	i++;
	while(line[i] != ' ') // pret pour la suite de la ligne
		i++;
	i++;
	while(node->value[j])
		new_line[len++] = node->value[j++];
	while(line[i])
		new_line[len++] = line[i++];
	new_line[len] = '\0';
	free(key);
	return(new_line);
	//free (new_line)
}

/* i = c (= the beggining of the command "cd")*/
char	*cd(char *line, t_env *env_list, char *home_path)
{
	int		i;
	int		j;
	char	*path;
	t_env	*current;
	
	i = where_is_cmd_in_line(line, "cd");
	if (i == 0)
		return (line); //error : cd pas trouve
	if (size_of_command(line, i, CMD) == 1 || line[i + 1] == '|' /* || ft_isalnum(line[i + 1]) != 1 */ /*|| size_of_command(line, i, CMD) == 2 || line[i] == '\0' */) // 1 car je rends size + 1 donc si size = 1 c'est que il n'y a rien apres cd / 2 pour le cas "cd | ..."" Plus d'un espace serait efface donc pas plus de 2
	{
		line = cd_home_path(line, home_path); // pas de path apres cd : on met le pwd initial derriere.
		return (line);
	}
	i ++; //now i = beggining of the path
	current = find_value_with_key_env(env_list, "PWD");
	path = malloc(sizeof(char) * (size_of_command(line, i, CMD) + ft_strlen(current->value)) + 2);
	if (!path)
		return (line); //?
	ft_strlcpy(path, current->value, ft_strlen(current->value));
	j = ft_strlen(path);
	path[j] = '/';
	j+=1;
	while(line[i] != ' ' && line[i] != '\0') // plus complique que ca : le path peut avoir des espaces TYPE : tronc \commun (je crois)
		path[j++] = line[i++];
	path[j] = '\0';
	//printf("path = %s\n", path);
/*Le bout suivant de fonction sera appelee par Karl, faudra la mettre dans une 
autre fonction car le reste de cette fonction sert juste a modifier la variable
line pour l'envoyer a l'executable*/
	if (chdir(path) != 0) // pour verifier que ca marche il faut avoir plusieurs dossiers
	{
		//printf errno
		return (NULL); //??
	}
	free(path);
	return (line);
}

char	*cd_home_path(char *line, char *home_path)
{
	char	*new_line;
	size_t		i;
	size_t		j;
	
	i = where_is_cmd_in_line(line, "cd");
	new_line = malloc(sizeof(char) * (i + 2)); // +2 car espace
	if (!new_line)
	{
		free (line); //car si pas de pb de malloc line serait free dans le joinstr
		return (NULL);
	}
	j = 0;
	while (j < i)
	{
		new_line[j] = line[j];
		j++;
	}
	new_line[j] = ' ';
	new_line[j + 1] = '\0';
	new_line = ft_join_pour_cd(new_line, home_path); //line est free la dedans
	j = ft_strlen(new_line);
	while (i <= ft_strlen(line))
		new_line[j++] = line[i++];
	new_line[j] = '\0';
	//printf("new_line = [%s]\n", new_line);
	return (new_line);
}

void	echo(char *line)
{
	/*GERER L'OPTION -n : 
	if(line[i] == ' ' && line[i + 1] == '-' && line[i + 2] == 'n' && line[i + 3] == ' '
		i+=3 //ou i+=4 ?*/
	int		i;
	int		j;
	char	*str;
	char	type;
	
	str = NULL;
	i = where_is_cmd_in_line(line, "echo");
	if (i == 0)
		return ; //error
	if (quotes_can_close(line) == false)
	{
		// printf("Error : quotes don't close\n"); //bash n'ecrit pas erreur mais je ne peux pas reproduire le > qui apparait
		return ;
	}
	i++;
	while(line[i] == ' ')
		i++;
	while (ft_isprint(line[i]) != 1 && line[i] != '\'' && line[i] != '\"')
		i++;
	// printf("debut des str : [%c]\n", line[i]);
	j = 0;
	while(line[i] && line[i] != '|')
	{
		if (line[i] == '\'')
		{
			type = '\'';
			i++;
		}
		else if (line[i] == '\"')
		{
			type = '\"';
			i++;
		}
		else
			type = ' ';
		// printf("apres avoir passe la premiere apostrophe (si il y en a) : [%c]\n", line[i]);
		if (line[i] == type) //pour sauter les strings vides
			i+=1;
		else
		{
			// printf("type : [%c]\n", type);
			// printf("premiere string commence par : [%c]\n", line[i]);
			str = ft_joinstr_minishell(line, i, str, type);
			if (str[0] == '\0') //premiere mot
			{
				j = 0;
				// printf("%spremier mot%s\n", YELLOW, RESET);
			}
			else //plus d'un mot
			{
				j = ft_strlen(str);
				// printf("%smot suivant%s\n", YELLOW, RESET);
			}
			while(line[i] != type && line[i])
			{
				if (type == ' ' && line[i] == '\\') //pour le test echo hola\ncaro -> doit donner holancaro
					i++;
				else
					str[j++] = line[i++];
			}
			str[j] = '\0';
			// printf("%smot : [%s]%s\n", GREEN, str, RESET);
			if (type == '\'' || type == '\"')
				i++;
			if (line[i] == ' ')
			{
				str[j++] = line[i++];
				while(line[i] == ' ' && line[i])
					i++;
			}
		}
	}
	str[j] = '\0';
	printf("%s\n", str);
	free(str);
	// printf("ligne en entier : [%s]/n", str);
	//AVANT de le print il faut pouvoir appliquer l'option -n
}
/*CARACTERES QUI NECESSITENT UN \ POUR ETRE ECHO CORRECTEMENT
En mettant chaque signe au milieu de ab et en les faisant echo :
# a#b
( syntax error near unexpected token `('
) syntax error near unexpected token `)'
* a*b
; a (\n) b: command not found
< a puis attend
> attend
` (heredoc)
~ a~b
" (heredoc)
\ ab
*/

void	pwd()
{
	char *path;

	path = getcwd(NULL, 0);
	if (path != NULL)
		printf("%s\n", path);
	//else
		//printf errno
	free(path);
}
