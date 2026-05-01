#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include <sys/iofunc.h>
#include <sys/resmgr.h>
#include <unistd.h>

#define DEVICE_NAME "hello"
#define BUF_SIZE 256

typedef struct {
    iofunc_attr_t attr;
    char buffer[BUF_SIZE];
} hello_dev_t;

static hello_dev_t dev;

/* READ handler */
int io_read_handler(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
    int nbytes;
    int status;

    /* check read permission */
    if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK)
        return status;

    /* calculate bytes to read */
    nbytes = strlen(dev.buffer) - ocb->offset;
    if (nbytes > msg->i.nbytes)
        nbytes = msg->i.nbytes;

    if (nbytes > 0) {
        SETIOV(ctp->iov, dev.buffer + ocb->offset, nbytes);
        _IO_SET_READ_NBYTES(ctp, nbytes);
        ocb->offset += nbytes;
    } else {
        _IO_SET_READ_NBYTES(ctp, 0);
    }

    return _RESMGR_NPARTS(1);
}

/* WRITE handler */
int io_write_handler(resmgr_context_t *ctp, io_write_t *msg, iofunc_ocb_t *ocb)
{
    int status;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return status;

    int nbytes = msg->i.nbytes;

    if (nbytes > BUF_SIZE - 1)
        nbytes = BUF_SIZE - 1;

    resmgr_msgread(ctp, dev.buffer, nbytes, sizeof(msg->i));
    dev.buffer[nbytes] = '\0';

    printf("Received from client: %s\n", dev.buffer);

    _IO_SET_WRITE_NBYTES(ctp, nbytes);

    return EOK;
}

int main(void)
{
    dispatch_t *dpp;
    resmgr_io_funcs_t io_funcs;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_attr_t resmgr_attr;
    dispatch_context_t *ctp;

    /* init dispatch */
    dpp = dispatch_create();

    memset(&resmgr_attr, 0, sizeof(resmgr_attr));
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* init functions */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                     &connect_funcs,
                     _RESMGR_IO_NFUNCS,
                     &io_funcs);

    io_funcs.read = io_read_handler;
    io_funcs.write = io_write_handler;

    /* init device */
    iofunc_attr_init(&dev.attr, S_IFCHR | 0666, NULL, NULL);
    strcpy(dev.buffer, "Hello from QNX resource manager\n");

    /* attach to /dev/hello */
    resmgr_attach(dpp, &resmgr_attr, "/dev/" DEVICE_NAME,
                  _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &dev);

    printf("Resource Manager running: /dev/%s\n", DEVICE_NAME);

    /* loop */
    ctp = dispatch_context_alloc(dpp);

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL)
            continue;
        dispatch_handler(ctp);
    }

    return 0;
}