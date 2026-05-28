#include <stdint.h>

#define AF_INET 2
#define SOCK_STREAM 1

void health_check();

// BAD implementation of memcpy! ARM64 requires memcpy to copy from .data to the stack.
void *memcpy(void *dest, const void *src, unsigned int n)
{
	char *d = dest;
	const char *s = src;
	while (n--)
		*d++ = *s++;
	return dest;
}

// Multi platform syscalls
#if defined(__x86_64__)
// x86_64 Syscall Numbers
#define SYS_WRITE 1
#define SYS_CLOSE 3
#define SYS_SOCKET 41
#define SYS_ACCEPT 43
#define SYS_BIND 49
#define SYS_LISTEN 50
#define SYS_TIME 201
#define SYS_EXIT 60
#define SYS_CONNECT 42

static inline long syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	long ret;
	__asm__ volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
											 "d"(a3), "r"(a4), "r"(a5), "r"(a6) : "rcx", "r11", "memory");
	return ret;
}

static inline int64_t syscall3(int64_t nr, int64_t a1, int64_t a2, int64_t a3)
{
	int64_t ret;
	__asm__ volatile(
		"syscall"
		: "=a"(ret)
		: "0"(nr), "D"(a1), "S"(a2), "d"(a3)
		: "rcx", "r11", "memory");
	return ret;
}

#elif defined(__aarch64__)
// ARM64 Syscall Numbers (Generic Table)
#define SYS_WRITE 64
#define SYS_CLOSE 57
#define SYS_SOCKET 198
#define SYS_ACCEPT 202
#define SYS_BIND 200
#define SYS_LISTEN 201
#define SYS_EXIT 93
#define SYS_GETTIME 113
#define SYS_CONNECT 203

static inline long syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	register long x8 __asm__("x8") = n;
	register long x0 __asm__("x0") = a1;
	register long x1 __asm__("x1") = a2;
	register long x2 __asm__("x2") = a3;
	register long x3 __asm__("x3") = a4;
	register long x4 __asm__("x4") = a5;
	register long x5 __asm__("x5") = a6;
	__asm__ volatile("svc #0" : "=r"(x0) : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5) : "memory");
	return x0;
}

static inline int64_t syscall3(int64_t nr, int64_t a1, int64_t a2, int64_t a3)
{
	register int64_t x8 __asm__("x8") = nr;
	register int64_t x0 __asm__("x0") = a1;
	register int64_t x1 __asm__("x1") = a2;
	register int64_t x2 __asm__("x2") = a3;
	__asm__ volatile(
		"svc #0"
		: "+r"(x0)
		: "r"(x8), "r"(x1), "r"(x2)
		: "memory");
	return x0;
}
#else
// If anyone wants support for more architectures ($$$) email me at: s101553@pollub.edu.pl
#error "Unsupported Architecture"
#endif

// Shrunk down syscalls
static inline int64_t syscall2(int64_t nr, int64_t a1, int64_t a2)
{
	return syscall3(nr, a1, a2, 0);
}

static inline int64_t syscall1(int64_t nr, int64_t a1)
{
	return syscall3(nr, a1, 0, 0);
}

// Arm has a little different syscall for getting unix timestamps
static inline int64_t get_timestamp()
{
#if defined(__x86_64__)
	return syscall1(SYS_TIME, 0);
#elif defined(__aarch64__)
	int64_t tv[2];
	// syscall 169 is gettimeofday
	// x0 = pointer to struct, x1 = timezone (0)
	register int64_t x8 __asm__("x8") = 169;
	register int64_t x0 __asm__("x0") = (int64_t)tv;
	register int64_t x1 __asm__("x1") = 0;
	__asm__ volatile("svc #0" : "+r"(x0) : "r"(x8), "r"(x1) : "memory");
	return tv[0];
#endif
}

struct in_addr
{
	unsigned int s_addr;
};

struct sockaddr_in
{
	unsigned short sin_family; // AF_INET = 2
	unsigned short sin_port;   // Must be Big-Endian (Network Byte Order)
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
};

void health_check()
{
	// Create socket
	int sock = syscall6(SYS_SOCKET, AF_INET, SOCK_STREAM, 0, 0, 0, 0);
	if (sock < 0)
		syscall1(SYS_EXIT, 1);

	// Setup Address (127.0.0.1 : 3000)
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = 0xB80B,			  // 3000 in Big-Endian (0x1F90)
		.sin_addr.s_addr = 0x0100007F // 127.0.0.1 in Big-Endian
	};

	// Connect
	long ret_connect = syscall6(SYS_CONNECT, sock, (long)&addr, sizeof(addr), 0, 0, 0);
	if (ret_connect < 0)
		syscall1(SYS_EXIT, 1);

	// Send a message
	char *msg = "Hello!";
	long ret_write = syscall6(SYS_WRITE, sock, (long)msg, 6, 0, 0, 0);
	if (ret_write < 0)
		syscall1(SYS_EXIT, 1);

	syscall1(SYS_EXIT, 0);
}

void main()
{
	// print date
	{
		int64_t ts = get_timestamp();
		uint64_t days = (uint64_t)ts / 86400;

		uint64_t year = 1970;
		while (1)
		{
			uint64_t leap = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 366 : 365;
			if (days < leap)
				break;
			days -= leap;
			year += 1;
		}

		uint64_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		uint64_t month = 0;
		while (month < 12)
		{
			uint64_t m_len = month_days[month];
			if (month == 1 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
				m_len = 29;
			if (days < m_len)
				break;
			days -= m_len;
			month++;
		}

		uint64_t day = days + 1;
		month += 1;

		char buf[11] = {};
		buf[0] = (char)(48 + day / 10);
		buf[1] = (char)(48 + day % 10);
		buf[2] = '.';
		buf[3] = (char)(48 + month / 10);
		buf[4] = (char)(48 + month % 10);
		buf[5] = '.';
		buf[6] = (char)(48 + year / 1000);
		buf[7] = (char)(48 + (year / 100) % 10);
		buf[8] = (char)(48 + (year / 10) % 10);
		buf[9] = (char)(48 + year % 10);
		buf[10] = '\n';

		syscall3(SYS_WRITE, 1, (int64_t)buf, 11);
	}

	const char *name = "Autor: Bartłomiej Deska\n";
	syscall3(SYS_WRITE, 1, (int64_t)name, 26);

	const char *portMsg = "Aplikacja nasłuchuje na porcie 3000\n";
	syscall3(SYS_WRITE, 1, (int64_t)portMsg, 37);

	// NETWORK SERVER
	int64_t sock = syscall3(SYS_SOCKET, 2, 1, 0); // AF_INET, SOCK_STREAM

	// sockaddr_in manual layout: sin_family (2), sin_port (3000 = 0x0BB8), sin_addr (0) all 0.0.0.0
	uint8_t addr[16] = {0};
	addr[0] = 2;	// AF_INET
	addr[2] = 0x0B; // Port 3000 MSB
	addr[3] = 0xB8; // Port 3000 LSB

	syscall3(SYS_BIND, sock, (int64_t)addr, 16);
	syscall2(SYS_LISTEN, sock, 10);

	// HTTP RESPONSE
	const char msg[] =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html;charset=utf8\r\n"
		"Content-Length: 591\r\n"
		"Connection: close\r\n\r\n"
		"<h1>Autor: Bartłomiej Deska</h1><button onclick=f(51.23,22.55)>Lublin</button>"
		"<button onclick=f(52.22,20.98)>Warszawa</button><button onclick=f(50.06,19.93)>Kraków</button>"
		"<pre id=o></pre><script>function z(e){switch(e){case 0:return\"sunny\";case 1:return\"light overcast\";"
		"case 2:return\"partial overcast\";case 3:return\"overcast\";default:return\"bad\"}}function f(e,t){"
		"fetch(`https://api.open-meteo.com/v1/forecast?latitude=${e}&longitude=${t}&current=temperature_2m,weather_code`)"
		".then(e=>e.json()).then(e=>{o.innerHTML=`${e.current.temperature_2m}°C ${z(e.current.weather_code)}`})}</script>";

	// Server loop
	while (1)
	{
		int64_t client_fd = syscall3(SYS_ACCEPT, sock, 0, 0);
		if (client_fd >= 0)
		{
			syscall3(SYS_WRITE, client_fd, (int64_t)msg, sizeof(msg) - 1);
			syscall1(SYS_CLOSE, client_fd);
		}
	}
}

// Entry point main()
#if defined(__x86_64__)
void __attribute__((naked)) _start()
{
	long *p;
	__asm__("mov %%rsp, %0" : "=r"(p));

	long argc = p[0];
	if (argc > 1)
		health_check();
	main();
}
#elif defined(__aarch64__)
__asm__(
	".global _start\n"
	"_start:\n"
	"ldr x0, [sp]\n"	 // Load argc from [sp] into 1st argument
	"mov x1, sp\n"		 // Move sp into x1
	"and x1, x1, #-16\n" // Align to 16 bytes
	"mov sp, x1\n"		 // Set aligned sp
	"bl main_logic\n"	 // Call our logic
);
void main_logic(long argc)
{
	if (argc > 1)
	{
		health_check();
	}
	main();
	syscall1(SYS_EXIT, 0); // Always exit!
}
#endif