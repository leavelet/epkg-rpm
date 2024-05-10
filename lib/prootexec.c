#include "prootexec.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int prootexecv(const char *path, char *const argv[]){
    // convert execv to $EPKG_PREFIX/bin/proot -S $EPKG_ENVS_DIR/$EPKG_ENV_NAME $path $argv
    
    char *const epkg_prefix = getenv("EPKG_PREFIX");
    char *const epkg_envs_dir = getenv("EPKG_ENVS_DIR");
    char *const epkg_env_name = getenv("EPKG_ENV_NAME");

    // if any of the environment variables are not set, call execv directly
    if(epkg_prefix == NULL || epkg_envs_dir == NULL || epkg_env_name == NULL){
        return execv(path, argv);
    }

    char proot_path[1024];
    snprintf(proot_path, sizeof(proot_path), "%s/bin/proot", epkg_prefix);

    char sandbox_path[1024];
    snprintf(sandbox_path, sizeof(sandbox_path), "-S %s/%s", epkg_envs_dir, epkg_env_name);
    
    int argc;
    for (argc = 0; argv[argc] != NULL; argc++);

    char *new_argv[argc + 4];
    new_argv[0] = proot_path;
    new_argv[1] = strdup("-S");
    new_argv[2] = strdup(sandbox_path);
    new_argv[3] = strdup(path);
    for (int i = 1; i <= argc; i++) {
        new_argv[i + 3] = argv[i];
    }

    // Execute the new command
    int result = execv(proot_path, new_argv);

    free(new_argv[1]);
    free(new_argv[2]);
    free(new_argv[3]);

    return result;
}