// server.c
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int a;
    int b;
} my_msg_t;

typedef struct {
    int sum;
} my_reply_t;

int main(void)
{
    int chid, rcvid;
    my_msg_t msg;
    my_reply_t reply;

    struct _msg_info64 info;

    // Create channel
    chid = ChannelCreate(0);
    if (chid == -1) {
        perror("ChannelCreate");
        return -1;
    }

    printf("Server PID=%d, CHID=%d\n", getpid(), chid);

    while (1) {
        memset(&info, 0, sizeof(info));

        // truyền &info vào đây
        rcvid = MsgReceive(chid, &msg, sizeof(msg), &info);

        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        if (rcvid == 0) {
            // Đây là pulse
            printf("Received pulse\n");
            continue;
        }

        printf("---- MSG INFO ----\n");
        printf("pid        : %d\n", info.pid);
        printf("tid        : %d\n", info.tid);
        printf("chid       : %d\n", info.chid);
        printf("scoid      : %d\n", info.scoid);
        printf("coid       : %d\n", info.coid);
        printf("msglen     : %d\n", info.msglen);
        printf("srcmsglen  : %d\n", info.srcmsglen);
        printf("priority   : %d\n", info.priority);
        printf("flags      : 0x%x\n", info.flags);
        printf("-------------------\n");
        
        if (info.msglen < sizeof(msg)) {
            printf("Message too small\n");
            MsgError(rcvid, EINVAL);
            continue;
        }

        printf("Server received: %d + %d rcvid=%d\n", msg.a, msg.b, rcvid);

        reply.sum = msg.a + msg.b;
        if (MsgReply(rcvid, 0, &reply, sizeof(reply)) == -1) {
            perror("MsgReply failed");
        }
    }

    return 0;
}