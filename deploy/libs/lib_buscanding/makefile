#######	MAKEFILE DE (lib)BUSCANDING	#######

RM := rm -rf

C_SRCS := \
./buscanding/config.c \
./buscanding/list.c \
./buscanding/log.c \
./buscanding/sockets.c \
./buscanding/test.c 

OBJS := \
./buscanding/config.o \
./buscanding/list.o \
./buscanding/log.o \
./buscanding/sockets.o \
./buscanding/test.o 

C_DEPS := \
./buscanding/config.d \
./buscanding/list.d \
./buscanding/log.d \
./buscanding/sockets.d \
./buscanding/test.d 

LIBS := -lcommons


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: libbuscanding.so

dependents:
	-cd ../lib_commons && $(MAKE) all

buscanding/%.o: ./buscanding/%.c
	@echo 'Armando: $<'
	@gcc -I"../lib_commons" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

libbuscanding.so: $(OBJS) $(USER_OBJS) ../lib_commons/libcommons.so
	@echo 'Armando targets: $@'
	@gcc -L"../lib_commons" -shared -o "libbuscanding.so" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libbuscanding.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
../lib_commons/libcommons.so:
