#! /bin/bash -x

${1} -f ${3} \
	-c "flash protect 0 64 last off; program ${2} verify; resume 0x20400000; exit" \
	2>&1 | tee openocd_upload.log
