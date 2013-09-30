#######	MAKEFILE DE (lib)MEMORIA	#######

RM := rm -rf

C_SRCS := \
./memoria/memoria.c 

OBJS := \
./memoria/memoria.o 

C_DEPS := \
./memoria/memoria.d 

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: libmemoria.so

memoria/%.o: ./memoria/%.c
	@echo 'Armando: $<'
	@gcc -I"../lib_commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

dependents:
	-cd ../lib_commons && $(MAKE) all

libmemoria.so: $(OBJS) $(USER_OBJS) ../lib_commons/libcommons.so
	@echo 'Armando targets: $@'
	@gcc -L"../lib_commons" -shared -o "libmemoria.so" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libmemoria.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
../lib_commons/libcommons.so:
