include $(SDK_DIRECTORY)/make/tntnet-top.mk
include $(CXXTOOLS_INSTALL_DIR)/make/cxxtools.mk
include $(SDK_DIRECTORY)/make/tntnet-ecpp.mk

LIBROOT          =hello
SHOBJ            =$(LIBROOT).so

INSTALL_SDK_CMP=$(TEMP_OUTPUT)/$(SHOBJ)


ECPPSRCS =hello.ecpp
ESRCS =$(EP_ECPP_SRCS)
OBJS =$(EP_ECXX_OBJS) $(EP_CXX_OBJS)

all: $(ESRCS) $(TEMP_OUTPUT)/$(SHOBJ)

clean:
	rm -rf $(TEMP_OUTPUT) 

include $(SDK_DIRECTORY)/make/tntnet-bottom.mk

include $(EC_SRC_DEPS)
