/* mbinit: main.c ; Author: Marie Eckert
 * mbinit is a simple tool to create projects based 
 * on the mariebuild system.
 *
 * Copyright (c) 2023, Marie Eckert
 * Licensed under the ISC License
 * <https://github.com/FelixEcker/mbinit/blob/master/LICENSE>
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>

#include <sys/stat.h>
#include <unistd.h>

#include <fsutils.h>

#define TEMPLATE_HOME "./build_templates/"
#define BUILD_FILE_EXTENSION ".mb"
#define INIT_SCRIPT_EXTENSION ".init"

#define MBINIT_VESRION "0.0.0"

/*****************************************************************************/
/*                 argp.h argument parsing functionality                     */
/*****************************************************************************/

const char *argp_program_version = "mbinit "MBINIT_VESRION;
const char *argp_program_bug_address =
  "https://github.com/FelixEcker/mbinit/issues";
const char description[] =
  "Project-Generator for mariebuild based projects\n"
  "Author: Marie Eckert";
const char args_doc[] = "";

static struct argp_option options[] = {
  {"template", 't', "TEMPLATE_NAME", 0, "Specify template to be used"}
, {"name", 'n', "PROJECT_NAME", 0, "Specify name of the project"}
, {0, 0, 0, 0}
};

struct arguments {
  char *template;
  char *name;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *args = state->input;
  switch (key) {
  case 't': args->template = arg; break;
  case 'n': args->name     = arg; break;
  default: return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, description };

/*****************************************************************************/
/*                      mbinit core functionality                            */
/*****************************************************************************/

int main(int argc, char **argv) {
  struct arguments args;
  args.template = "c";
  args.name = NULL;

  argp_parse(&argp, argc, argv, 0, 0, &args);

  if (args.name == NULL) {
    printf("Missing 1 required argument: -n PROJECT_NAME\n");
    return 1;
  }

  if (access(args.name, F_OK) == 0) {
    printf("Cannot create project: Directory already exists!\n");
    return 1;
  }

  // Program Result / Exit Code
  int result = 0;

  // Create filepaths for the build file and init script

  char template_home[]      = TEMPLATE_HOME;
  char build_extension[]    = BUILD_FILE_EXTENSION;
  char init_extension[]     = INIT_SCRIPT_EXTENSION;

  size_t bsize = strlen(template_home) + strlen(args.template) + 1;
  char *build_path  = malloc(bsize+strlen(build_extension));
  char *script_path = malloc(bsize+strlen(init_extension));

  /* NOTE: I am aware of the security related bugs of sprintf, however
   * i simply choose to not give a fuck :)
   */
  sprintf(build_path, "%s%s%s", template_home, args.template, build_extension);
  sprintf(script_path, "%s%s%s", template_home, args.template, init_extension);
  
  if (access(build_path, F_OK) != 0 && access(script_path, F_OK) != 0) {
    printf("Could not find template: %s\n", args.template);
    printf("Your template directory is: %s\n", template_home);
    goto mbinit_err_exit;
  }

  if (mkdir(args.name, S_IRUSR | S_IWUSR | S_IXUSR) != 0) {
    printf("Error creating project directory!\n");
    goto mbinit_err_exit;
  }

  // Copy buildfile if exists
  if (access(build_path, F_OK) == 0) {
    char *fdest = malloc(strlen(args.name) + strlen(args.template) 
                       + strlen(build_extension) + 2);
    sprintf(fdest, "%s/%s%s", args.name, args.template, build_extension);

    int cpyres = copy_file(fdest, build_path);
    free(fdest);
    if (cpyres != 0) {
      printf("Error copying buildfile, errno = %d\n", errno);
      goto mbinit_err_exit;
    }
  }

  goto mbinit_exit;
mbinit_err_exit:
  result = 1;
mbinit_exit:
  free(build_path);
  free(script_path);
  return result;
}
