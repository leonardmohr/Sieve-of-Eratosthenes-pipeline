#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>


void source(int n)
{
        for(int i = 2; i <= n; i++) {
                write(1, &i, sizeof(int));
        }
}

void redirect(int old, int new[2])
{
        // Either 0 or 1;
        // Redirect stdin, or stdout
        if (old != 0 && old != 1) {
                return;
        }
        close(old);
        dup(new[old]);
        close(new[old]);
}

void found_prime()
{
        int prime;
        if (read(0, &prime, sizeof(int)) <= 0) {
                return;
        }
        printf("prime %d\n", prime);

        // Creat a new pipe to communicate from parent to child.
        int pd[2];
        pipe(pd);

        
        if (fork()) { // Parent Process
                // Close read end of pipe
                close(pd[0]);

                // Redirect stdout to pipe
                redirect(1, pd);

                int n;

                while (read(0, &n, sizeof(int)) > 0) {
                        if (n % prime != 0) {
                                write(1, &n, sizeof(int));
                        }
                }
                close(0);
                close(1);
                wait(NULL);
        } else {      // Child Process
                redirect(0, pd);
                close(pd[1]);
                found_prime();
        }
        return;
}



int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Usage: primes n\n");
                exit(1);
        }
        int pd[2];
        pipe(pd);

        // Parent Process
        if (fork()) {
                // Redirect stdout to write end of pipe
                redirect(1, pd);

                // Close read end of pipe
                close(pd[0]);

                // Get n from terminal argument
                int n = atoi(argv[1]);

                // Let's put stream of numbers into write end of pipe
                source(n);

                // Close write end of pipe
                close(1);

                // Wait for child to do it's thing
                wait(NULL);
        } else {
                // Redirect stdin to read end of pipe
                redirect(0, pd);

                // Close write end of pipe
                close(pd[1]);
                found_prime();

                // Close read end of pipe
                close(0);
        }
        
        return 0;
}
