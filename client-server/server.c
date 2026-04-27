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

    struct _msg_info64 info;   // <-- thêm cái này

    // Create channel
    chid = ChannelCreate(0);
    if (chid == -1) {
        perror("ChannelCreate");
        return -1;
    }

    printf("Server PID=%d, CHID=%d\n", getpid(), chid);

    while (1) {
        memset(&msg, 0, sizeof(msg));
        memset(&info, 0, sizeof(info));

        // truyền &info vào đây
        rcvid = MsgReceive(chid, &msg, sizeof(msg), &info);

        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }
        
        
        // 🔥 In thông tin msg_info64
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
        
        if(info.msglen != sizeof(msg)) {
            // Đây là một pulse, không phải message
            MsgReply(rcvid, 0, NULL, 0);
            continue;
        }
        printf("Server received: %d + %d rcvid=%d\n", msg.a, msg.b, rcvid);

        reply.sum = msg.a + msg.b;
        MsgReply(rcvid, 0, &reply, sizeof(reply));
    }

    return 0;
}