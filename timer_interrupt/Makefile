# QNX cross compiler
CC = E:\QNX\SDK\host\win64\x86_64\usr\bin\aarch64-unknown-nto-qnx8.0.0-gcc

# Sources
SRCS = timer.c 

# Objects
OBJS = $(SRCS:.c=.o)

# Dependency files
DEPS = $(SRCS:.c=.d)

# Targets
TARGETS = timer

# Flags
CFLAGS = -Wall -O0 -g -MMD -MP
LDFLAGS =

# Default
all: $(TARGETS)

# Build rules
timer: timer.o
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> .o (auto generate .d)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean
clean:
	del /Q *.o *.d timer  2>nul || rm -f *.o *.d timer 
