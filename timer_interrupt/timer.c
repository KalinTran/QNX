#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <time.h>

int main()
{
    int chid, coid;
    struct sigevent event;
    struct _pulse pulse;
    timer_t timerid;
    struct itimerspec itime;

    // 1. Create channel
    chid = ChannelCreate(0);
    if (chid == -1) {
        perror("ChannelCreate");
        return -1;
    }

    // 2. Attach connection
    coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        perror("ConnectAttach");
        return -1;
    }

    // 3. Setup pulse event
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, 1, 0);

    // 4. Create timer
    if (timer_create(CLOCK_REALTIME, &event, &timerid) == -1) {
        perror("timer_create");
        return -1;
    }

    // 5. Set timer (1s interval)
    itime.it_value.tv_sec = 1;
    itime.it_value.tv_nsec = 0;
    itime.it_interval.tv_sec = 1;
    itime.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &itime, NULL) == -1) {
        perror("timer_settime");
        return -1;
    }

    printf("Timer started...\n");

    // 6. Wait for pulse
    while (1) {
        MsgReceive(chid, &pulse, sizeof(pulse), NULL);

        if (pulse.code == 1) {
            printf(">>> Timer tick!\n");
        }
    }

    return 0;
}

// +----------------------+
// |   Hardware Timer     |
// |   (IRQ 0x1b)         |
// +----------+-----------+
//             |
//             v
// +----------------------+
// |     Kernel (procnto) |
// |  - Handle IRQ        |
// |  - Update tick       |
// |  - Manage timers     |
// +----------+-----------+
//             |
//             v
// +----------------------+
// |   Timer Subsystem    |
// | (timer_create/set)   |
// +----------+-----------+
//             |
//             v
// +----------------------+
// |   SIGEV_PULSE        |
// | (event generation)   |
// +----------+-----------+
//             |
//             v
// +----------------------+
// |   Message Passing    |
// |  (Channel/MsgRecv)   |
// +----------+-----------+
//             |
//             v
// +----------------------+
// |    User Process      |
// |  ">>> Timer tick!"   |
// +----------------------+
