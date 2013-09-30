#######	MAKEFILE DE (src)NIVEL	#######

RM := rm -rf

C_SRCS := \
./nivel.c 

OBJS += \
./nivel.o 

C_DEPS += \
./nivel.d 

LIBS := -lgui -lpthread -lcommons -lbuscanding -lcurses

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

all: nivel

%.o: ./%.c
	@echo 'Armando: $<'
	@gcc -I"../../libs/lib_gui" -I"../../libs/lib_commons" -I"../../libs/lib_buscanding" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

dependents:
	-cd ../../libs/lib_buscanding && $(MAKE) all
	-cd ../../libs/lib_commons && $(MAKE) all
	-cd ../../libs/lib_gui && $(MAKE) all

# Tool invocations
nivel: $(OBJS) $(USER_OBJS) ../../libs/lib_buscanding/libbuscanding.so ../../libs/lib_commons/libcommons.so ../../libs/lib_gui/libgui.so
	@echo 'Armando targets: $@'
	@gcc -L../../libs/lib_gui -L../../libs/lib_buscanding -L../../libs/lib_commons -o "nivel" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) nivel *.log
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
../../libs/lib_buscanding/libbuscanding.so:
../../libs/lib_commons/libcommons.so:
../../libs/lib_gui/libgui.so:

