#include <kernel/fb.h>
#include <kernel/libc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>

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
			if (!FbPrint(format, amount)) {
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
			if (!FbPrint(&c, sizeof(c))) {
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
			if (!FbPrint(str, len)) {
				return -1;
			}
			written += len;
		} else if (*format == 'x') {
			format++;
			size_t xv = va_arg(parameters, size_t);
			for(int i=sizeof(size_t) * 8 - 4;i>=0;i-=4) {
				char dd=hexmake((xv >> i) & 0xF);
				if(!FbPrint(&dd,1)) {
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
				if(!FbPrint(&dd,1)) {
					return -1;
				}
				written++;
			}
		} else if (*format == 'd') {
			format++;
			long long int dvs = va_arg(parameters, long long int);
			char dm='-';
			if(dvs < 0) {if(!FbPrint(&dm,1)) return -1; written++;}
			unsigned long long int dv = (unsigned long long int)(dvs >= 0 ? dvs : -dvs);
			bool first=true;
			if(dv == 0) {
				char dd='0';
				if(!FbPrint(&dd,1)) return -1;
				written++;
			} else {
				size_t mul = 10;
				written++;
				while(dv / mul > 0) {
					mul *= 10;
				}
				while(mul >= 1) {
					char dd='0'+((dv/mul)%10);
					mul /= 10;
					if((first && dd != '0') || (!first)) {
						if(!FbPrint(&dd,1)) return -1;
						written++;
					}
					if(first) first = false;
				}
			}
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!FbPrint(format, len)) {
				return -1;
			}
			written += len;
			format += len;
		}
	}
	va_end(parameters);
	return written;
}