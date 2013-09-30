#######	MAKEFILE DE (lib)GUI	#######

RM := rm -rf

C_SRCS := \
./gui/gui.c \
./gui/tad_items.c 

OBJS := \
./gui/gui.o \
./gui/tad_items.o 

C_DEPS := \
./gui/gui.d \
./gui/tad_items.d 

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


all: libgui.so

gui/%.o: ./gui/%.c
	@echo 'Armando: $<'
	@gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

libgui.so: $(OBJS) $(USER_OBJS)
	@echo 'Armando targets: $@'
	@gcc -shared -o "libgui.so" $(OBJS) $(USER_OBJS) $(LIBS)

clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libgui.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

