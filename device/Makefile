# QNX cross compiler
CC = E:\QNX\SDK\host\win64\x86_64\usr\bin\aarch64-unknown-nto-qnx8.0.0-gcc

# Sources
SRCS = hello_resrc_mng.c 

# Objects
OBJS = $(SRCS:.c=.o)

# Dependency files
DEPS = $(SRCS:.c=.d)

# Targets
TARGETS = hello_resrc_mng

# Flags
CFLAGS = -Wall -O0 -g -MMD -MP
LDFLAGS =

# Default
all: $(TARGETS)

# Build rules
hello_resrc_mng: hello_resrc_mng.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> .o (auto generate .d)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean
clean:
	del /Q *.o *.d hello_resrc_mng  2>nul || rm -f *.o *.d hello_resrc_mng 


# +-------------+        +-------------+        +---------------------+
# |   Client    |        |   Kernel    |        | Resource Manager    |
# | (user app)  |        | (microkernel)|       |  (/dev/hello)       |
# +-------------+        +-------------+        +---------------------+
#        |                      |                         |
#        | read(fd, buf, n)    |                         |
#        |-------------------->|                         |
#        |                     |                         |
#        |                     | MsgSend()               |
#        |                     |------------------------>|
#        |                     |                         |
#        |                     |                 io_read_handler()
#        |                     |                         |
#        |                     |<------------------------|
#        |                     |     MsgReply(data)      |
#        |<--------------------|                         |
#        |   return data       |                         |


# Client Thread
#     |
#     | read(fd)
#     v
# libc (wrap)
#     |
#     | MsgSend()
#     v
# ================= KERNEL =================
#     |
#     | lookup fd → channel id
#     | build message (_IO_READ)
#     |
#     | block client thread (STATE: SEND BLOCKED)
#     |
#     v
# =========== RESOURCE MANAGER (user-space) ===========
#     |
#     | dispatch_block()
#     | → nhận message
#     |
#     | dispatch_handler()
#     | → io_read_handler()
#     |
#     | xử lý data
#     |
#     | MsgReply()
#     v
# ================= KERNEL =================
#     |
#     | wakeup client thread
#     | copy data to user buffer
#     |
#     v
# Client Thread
#     |
#     | return from read()



# Client (echo "hi" > /dev/hello)
#       |
#       | write()
#       v
# Kernel
#       |
#       | MsgSend(_IO_WRITE)
#       v
# Resource Manager
#       |
#       | io_write_handler()
#       | (resmgr_msgread lấy data)
#       |
#       | MsgReply()
#       v
# Client return