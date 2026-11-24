/* Mayhem needs an ELF cmd: exec the atheris harness with the system python3,
   forwarding libFuzzer arguments. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char **argv) {
  char **args = calloc((size_t)argc + 2, sizeof *args);
  if (!args) return 1;
  args[0] = "python3";
  args[1] = HARNESS_PY;
  for (int i = 1; i < argc; i++) args[i + 1] = argv[i];
  execvp("python3", args);
  perror("execvp python3");
  return 127;
}
