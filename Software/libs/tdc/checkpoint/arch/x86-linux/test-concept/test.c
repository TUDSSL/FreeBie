#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGQUIT)
    printf("received SIGKILL\n");
}

int main(void)
{
  if (signal(SIGQUIT, sig_handler) == SIG_ERR)
  printf("\ncan't catch SIGQUIT\n");
  // A long long wait so that we can easily issue a signal to this process
  while(1)
    sleep(1);
  return 0;
}
