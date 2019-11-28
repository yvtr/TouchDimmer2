

# Set default board if not defined
ifeq ($(strip $(BOARD_TAG)),)
	BOARD_TAG = attinyx5
	BOARD_SUB = 45
	VARIANT = tinyX5
	ALTERNATE_CORE = ATTinyCore
	F_CPU = 8000000L
endif


#ARDUINO_LIBS =
CPPFLAGS = -U__PROG_TYPES_COMPAT__

UML_SUBDIR = umlgraph
UML_SRC = sm_dimmer.c
UML_CPPSRC =

postbuild: all
	mv -f gitdefs.h $(OBJDIR)/

prebuild: gitdefs.h

all: prebuild

# Extract GIT version info to gitdefs.h
gitdefs.h:
	rm -f gitdefs.h
	git describe --abbrev=8 --dirty --always > gitinfo.txt
	git describe --abbrev=8 --always >> gitinfo.txt
	git rev-parse --abbrev-ref HEAD >> gitinfo.txt
	git describe --always --tags --dirty=-d | gawk 'NR==1{print $0}' >> gitinfo.txt
	git rev-list HEAD | wc -l | tr -d ' ' >> gitinfo.txt
	gawk -f gitinfo.awk < gitinfo.txt > gitdefs.h
	rm -f gitinfo.txt


UML_PNG = $(UML_SRC:%.c=$(UML_SUBDIR)/%.png) $(UML_CPPSRC:%.cpp=$(UML_SUBDIR)/%.png)

# Generate UML state diagram
uml: $(UML_PNG)

# Make state diagram image from Plantuml source
.PRECIOUS: $(UML_SUBDIR)/%.uml
$(UML_SUBDIR)/%.png: $(UML_SUBDIR)/%.uml
	java -jar $(PLANTUML) -charset UTF-8 $<

# Extract Plantuml metadata from c/cpp file with state machine
$(UML_SUBDIR)/%.uml : %.c | $(UML_SUBDIR)
	gawk -f uml.awk $< > $@

$(UML_SUBDIR)/%.uml : %.cpp | $(UML_SUBDIR)
	gawk -f uml.awk $< > $@

$(UML_SUBDIR):
	mkdir $(UML_SUBDIR)


.PHONY: gitdefs.h prebuild postbuild uml

include $(ARDMK_DIR)/Arduino.mk
