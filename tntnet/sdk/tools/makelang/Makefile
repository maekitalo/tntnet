include $(SDK_DIRECTORY)/make/tntnet-top.mk
include $(CXXTOOLS_INSTALL_DIR)/make/cxxtools.mk

PROGRAM     =makelang
INSTALL_SDK_BIN  =$(TEMP_OUTPUT)/$(PROGRAM)

LOCAL_LDFLAGS  = -ltntnet -lecpp -ltntnet_sdk -lz

SRCS =makelang.cpp
OBJS =$(EP_CXX_OBJS)

all: $(TEMP_OUTPUT)/$(PROGRAM)

clean:
	rm -rf $(TEMP_OUTPUT) core

include $(SDK_DIRECTORY)/make/tntnet-bottom.mk

include $(EC_SRC_DEPS)
