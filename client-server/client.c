#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    int a;
    int b;
} my_msg_t;

typedef struct {
    int sum;
} my_reply_t;

int main(int argc, char *argv[])
{
    int coid;
    my_msg_t msg;
    my_reply_t reply;

    if (argc < 2) {
        printf("Usage: %s <server_pid>\n", argv[0]);
        return -1;
    }

    int server_pid = atoi(argv[1]);  // lấy PID từ argument

    coid = ConnectAttach(ND_LOCAL_NODE, server_pid, 1, 0, 0);
    if (coid == -1) {
        perror("ConnectAttach");
        return -1;
    }

    msg.a = 10;
    msg.b = 20;

    printf("Client sending: %d + %d , coid = %d\n", msg.a, msg.b, coid);

    MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));

    printf("Client received reply: sum = %d\n", reply.sum);

    return 0;
}