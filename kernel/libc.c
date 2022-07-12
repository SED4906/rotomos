#include <kernel/libc.h>
#include <kernel/tty.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

void* memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

int strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char hexmake(unsigned int v) {
	if(v>9) return v+55;
	return v+48;
}

int printf(const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);
	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!tty_write(format, amount)) {
				return -1;
			}
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!tty_write(&c, sizeof(c))) {
				return -1;
			}
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!tty_write(str, len)) {
				return -1;
			}
			written += len;
		} else if (*format == 'x') {
			format++;
			size_t xv = va_arg(parameters, size_t);
			for(int i=sizeof(size_t) * 8 - 4;i>=0;i-=4) {
				char dd=hexmake((xv >> i) & 0xF);
				if(!tty_write(&dd,1)) {
					return -1;
				}
				written++;
			}
		} else if (*format == 'X') {
			format++;
			size_t xv = va_arg(parameters, size_t);
			int i = 0;
			while(xv >> (i + 4) && i < (int)sizeof(size_t) * 8) i+=4;
			for(;i>=0;i-=4) {
				char dd=hexmake((xv >> i) & 0xF);
				if(!tty_write(&dd,1)) {
					return -1;
				}
				written++;
			}
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!tty_write(format, len)) {
				return -1;
			}
			written += len;
			format += len;
		}
	}
	va_end(parameters);
	return written;
}