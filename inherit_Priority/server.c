// server.c
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>

typedef struct {
    int a;
    int b;
} my_msg_t;

typedef struct {
    int sum;
} reply_t;

int main(void)
{
    int chid, rcvid;
    my_msg_t msg;
    reply_t reply;
    struct sched_param param;
    int policy;
    struct _msg_info64 info;

    // policy = sched_getscheduler(0);
    chid = ChannelCreate(0);
    
    printf("Server started (LOW priority)\n");
    printf("Server PID=%d, CHID=%d\n", getpid(), chid);
    
    
    while (1) {
        memset(&info, 0, sizeof(info));
        sched_getparam(0, &param);
        printf("Realtime priority before   = %d\n", param.sched_priority);
        rcvid = MsgReceive(chid, &msg, sizeof(msg), &info);
        sched_getparam(0, &param);
        printf("Realtime priority during   = %d\n", param.sched_priority);
        printf("Server running with inherited priority!\n");
        
        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        if (rcvid == 0) {
            // Đây là pulse
            printf("Received pulse\n");
            continue;
        }

        // printf("Processing: %d + %d\n", msg.a, msg.b);
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
        
        if (rcvid > 0) {
            sleep(2); // giả lập xử lý lâu

            reply.sum = msg.a + msg.b;
            MsgReply(rcvid, 0, &reply, sizeof(reply));
            sched_getparam(0, &param);
            printf("Realtime priority after reply= %d\n", param.sched_priority);
        }
    }
}