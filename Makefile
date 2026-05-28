all:
	gcc -Os -nostdlib -static -no-pie \
		-Wl,-z,common-page-size=4096 \
		-Wl,-z,max-page-size=4096 \
		-march=native \
		-fno-asynchronous-unwind-tables \
		-fno-stack-protector \
		-fno-ident \
		-ffunction-sections -fdata-sections \
		-Wl,-n \
		-Wl,--nmagic \
		-Wl,--build-id=none \
		-s server.c -o server