#######	MAKEFILE DE (lib)COMMONS	#######

RM := rm -rf

C_SRCS := \
./commons/collections/dictionary.c \
./commons/collections/list.c \
./commons/collections/queue.c \
./commons/bitarray.c \
./commons/config.c \
./commons/error.c \
./commons/log.c \
./commons/process.c \
./commons/string.c \
./commons/temporal.c \
./commons/txt.c 

OBJS := \
./commons/collections/dictionary.o \
./commons/collections/list.o \
./commons/collections/queue.o  \
./commons/bitarray.o \
./commons/config.o \
./commons/error.o \
./commons/log.o \
./commons/process.o \
./commons/string.o \
./commons/temporal.o \
./commons/txt.o 

C_DEPS := \
./commons/collections/dictionary.d \
./commons/collections/list.d \
./commons/collections/queue.d \
./commons/bitarray.d \
./commons/config.d \
./commons/error.d \
./commons/log.d \
./commons/process.d \
./commons/string.d \
./commons/temporal.d \
./commons/txt.d 


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: libcommons.so libso-commons-library.so

commons/collections/%.o: ../commons/collections/%.c
	@echo 'Armando: $<'
	@gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

commons/%.o: ../commons/%.c
	@echo 'Armando: $<'
	@gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

libcommons.so: $(OBJS) $(USER_OBJS)
	@echo 'Armando targets: $@'
	@gcc -shared -o "libcommons.so" $(OBJS) $(USER_OBJS) $(LIBS)

libso-commons-library.so:
	-@cp ./libcommons.so ./libso-commons-library.so
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libcommons.so libso-commons-library.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
