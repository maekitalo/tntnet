include $(SDK_DIRECTORY)/make/tntnet-top.mk
include $(CXXTOOLS_INSTALL_DIR)/make/cxxtools.mk

LIBROOT          =tntnet
SHOBJ            =$(LIBROOT).so

LOCAL_LDFLAGS = -ltntnet -lecpp

INSTALL_PRIVATE_INC=

INSTALL_PUBLIC_INC=

INSTALL_CMP=$(TEMP_OUTPUT)/$(SHOBJ)


SRCS =error.cpp redirect.cpp static.cpp
OBJS =$(EP_CXX_OBJS)

all: $(TEMP_OUTPUT)/$(SHOBJ)

clean:
	rm -rf $(TEMP_OUTPUT) 

include $(SDK_DIRECTORY)/make/tntnet-bottom.mk

include $(EC_SRC_DEPS)
