// client.c
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <unistd.h>
#include <sched.h>

typedef struct {
    int a;
    int b;
} my_msg_t;

typedef struct {
    int sum;
} reply_t;

int main(int argc, char *argv[])
{
    int coid;
    int status;
    my_msg_t msg;
    reply_t reply;

    // set HIGH priority
    struct sched_param param;
    param.sched_priority = 50;
    sched_setscheduler(0, SCHED_RR, &param);

    printf("Client running with HIGH priority\n");

    coid = ConnectAttach(0, atoi(argv[1]), 1, 0, 0);

    msg.a = 10;
    msg.b = 20;

    status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));

    if (status == -1) {
        ConnectDetach(coid);
        return -1;
    }

    printf("Client received reply: sum = %d\n", reply.sum);

    
    ConnectDetach(coid);
/***************************************************************************************/
    printf("Client running with HIGH priority 9\n");
    param.sched_priority = 9;
    sched_setscheduler(0, SCHED_RR, &param);

    coid = ConnectAttach(0, atoi(argv[1]), 1, 0, 0);

    msg.a = 10;
    msg.b = 20;

    status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));

    if (status == -1) {
        ConnectDetach(coid);
        return -1;
    }

    printf("Client received reply: sum = %d\n", reply.sum);

    
    ConnectDetach(coid);

    return 0;
}