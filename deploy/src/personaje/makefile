#######	MAKEFILE DE (src)PERSONAJE	#######

RM := rm -rf

C_SRCS := \
./personaje.c 

OBJS := \
./personaje.o 

C_DEPS := \
./personaje.d 

LIBS := -lcommons -lbuscanding -lpthread

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: personaje

%.o: ./%.c
	@echo 'Armando: $<'
	@gcc -I"../../libs/lib_commons" -I"../../libs/lib_buscanding" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

dependents:
	-cd ../../libs/lib_buscanding && $(MAKE) all
	-cd ../../libs/lib_commons && $(MAKE) all

# Tool invocations
personaje: $(OBJS) $(USER_OBJS) ../../libs/lib_buscanding/libbuscanding.so ../../libs/lib_commons/libcommons.so
	@echo 'Armando targets: $@'
	@gcc -L"../../libs/lib_commons" -L"../../libs/lib_buscanding" -o "personaje" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) personaje *.log
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
../../libs/lib_buscanding/libbuscanding.so:
../../libs/lib_commons/libcommons.so:
