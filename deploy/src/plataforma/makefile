#######	MAKEFILE DE (src)PLATAFORMA	#######

RM := rm -rf

C_SRCS := \
./plataforma.c 

OBJS := \
./plataforma.o 

C_DEPS := \
./plataforma.d 


LIBS := -lcommons -lbuscanding -lpthread

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: plataforma

%.o: ./%.c
	@echo 'Armando: $<'
	@gcc -I"../../libs/lib_commons" -I"../../libs/lib_buscanding" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

dependents:
	-cd ../../libs/lib_buscanding && $(MAKE) all
	-cd ../../libs/lib_commons && $(MAKE) all

plataforma: $(OBJS) $(USER_OBJS) ../../libs/lib_buscanding/libbuscanding.so ../../libs/lib_commons/libcommons.so
	@echo 'Armando targets: $@'
	@gcc -L"../../libs/lib_commons" -L"../../libs/lib_buscanding" -o "plataforma" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) plataforma *.log
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
../../libs/lib_buscanding/libbuscanding.so:
../../libs/lib_commons/libcommons.so:
