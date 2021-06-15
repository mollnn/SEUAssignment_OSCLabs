#include "types.h"
#include "fcntl.h"
#include "user.h"
#include "pstat.h"

void ps()
{
    struct pstat info;
    getpinfo(&info);
    printf(1, "PID\tTICKETS\tTICKS:");
    for (int i = 0; i < NPROC; ++i)
    {
        if (info.pid[i] == 0)
            continue;
        printf(1, "(%d,%d,%d)\t", info.pid[i], info.tickets[i], info.ticks[i]);
    }
    printf(1, "\n");
}

int main(int argc, char **argv)
{
    settickets(999);
    int pid1 = fork();
    if (pid1 == 0)
    {
        // sub1 - 4
        settickets(300);
        while (1)
            ;
    }
    else
    {
        int pid2 = fork();
        if (pid2 == 0)
        {
            // sub2 - 5
            settickets(200);
            while (1)
                ;
        }
        else
        {
            int pid3 = fork();
            if (pid3 == 0)
            {
                // sub3 - 6
                settickets(100);
                while (1)
                    ;
            }
            else
            {
                // main
                for (int i = 1; i <= 100; i++)
                {
                    ps();
                    sleep(1);
                }
                kill(pid1);
                kill(pid2);
                kill(pid3);
                exit();
            }
        }
    }
}