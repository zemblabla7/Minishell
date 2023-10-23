/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: casomarr <casomarr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:55:33 by casomarr          #+#    #+#             */
/*   Updated: 2023/10/23 16:09:02 by casomarr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "../../libft/libft.h"

/*Gets the current path and prints it if the 
trigger reqiures it.*/
char	*pwd(int option)
{
	char *path;

	path = getcwd(NULL, 0);
	//if (!path)
		//printf errno
	if (option == PRINT)
		printf("%s\n", path);
	return (path);
}

t_env	*pwd_update_in_env(t_env *env_list)
{
	t_env	*temp;
	
	temp = find_value_with_key_env(env_list, "PWD");
	free(temp->value);
	temp->value = ft_calloc(ft_strlen(pwd(NO_PRINT)) + 100, sizeof(char));
	ft_strlcpy(temp->value, pwd(NO_PRINT), ft_strlen(pwd(NO_PRINT)) + 1);
	return(env_list);
}