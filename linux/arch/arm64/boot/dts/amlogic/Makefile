dtb-y += axg_sue_s1832_l1_prime.dtb
dtb-y += axg_sue_s1832_l2_prime.dtb
dtb-y += axg_sue_s1832_l1_factory.dtb
dtb-y += axg_sue_s1832_l2_factory.dtb

targets += dtbs

DTB_NAMES := $(subst $\",,$(CONFIG_BUILD_ARM64_APPENDED_DTB_IMAGE_NAMES))
ifneq ($(DTB_NAMES),)
DTB_LIST := $(addsuffix .dtb,$(DTB_NAMES))
else
DTB_LIST := $(dtb-y)
endif
targets += $(DTB_LIST)

dtbs: $(addprefix $(obj)/, $(DTB_LIST))

always := $(dtb-y)
clean-files := *.dtb
