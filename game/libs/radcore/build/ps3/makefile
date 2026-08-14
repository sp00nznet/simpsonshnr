#####################################################################
# Note: The order of the include does seem to matter.  Please do not
#	modify this file
#
#####################################################################

#================================
# files to be compiled and linked
#================================
include files.txt

#================================
# Our global makefile
#================================
include $(ROOTDIR)/radbuild/makefile

#===========================================
# Include dependencies, **must be included**
#===========================================
ifeq ($(DOCLEAN),false)
-include $(DEPS)
endif
