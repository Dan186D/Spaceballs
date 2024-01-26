#include "io.h"

bool _io_int8(io_t* io, int8_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
		if(fprintf(io->fp, "%d; %s %s = %d;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		int idatum; const int scan_n = sscanf(io->line, "%d", &idatum); *datum = (int8_t)idatum; IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_int16(io_t* io, int16_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%d; %s %s = %d;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		int idatum; const int scan_n = sscanf(io->line, "%d", &idatum); *datum = (int16_t)idatum; IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_int(io_t* io, int32_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%d; %s %s = %d;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%d", datum); IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_uint8(io_t* io, uint8_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%u; %s %s = %u;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		uint32_t udatum; const int scan_n = sscanf(io->line, "%u", &udatum); *datum = (uint8_t)udatum; IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_uint16(io_t* io, uint16_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%u; %s %s = %u;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		uint32_t udatum; const int scan_n = sscanf(io->line, "%u", &udatum); *datum = (uint16_t)udatum; IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_uint(io_t* io, uint32_t* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%u; %s %s = %u;%s%s\n", *datum, type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%u", datum); IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_float(io_t* io, float* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%08X; %s %s = %.08f;%s%s\n", *((uint32_t*)(datum)), type, datum_str, *datum, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%08X", (uint32_t*)datum); IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
	}
	return true;
}

bool _io_float2(io_t* io, float* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%08X %08X; %s %s = { %.08f, %.08f };%s%s\n", *((uint32_t*)(&datum[0])), *((uint32_t*)(&datum[1])), type, datum_str, datum[0], datum[1], (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%08X %08X", (uint32_t*)&datum[0], (uint32_t*)&datum[1]); IO_READ_EXTRA;
		if(scan_n != 2) { IO_ERROR; }
	}
	return true;
}

bool _io_float3(io_t* io, float* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%08X %08X %08X; %s %s = { %.08f, %.08f, %.08f };%s%s\n", *((uint32_t*)(&datum[0])), *((uint32_t*)(&datum[1])), *((uint32_t*)(&datum[2])), type, datum_str, datum[0], datum[1], datum[2], (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%08X %08X %08X", (uint32_t*)&datum[0], (uint32_t*)&datum[1], (uint32_t*)&datum[2]); IO_READ_EXTRA;
		if(scan_n != 3) { IO_ERROR; }
	}
	return true;
}

bool _io_float4(io_t* io, float* datum, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE;
	if(fprintf(io->fp, "%08X %08X %08X %08X; %s %s = { %.08f, %.08f, %.08f, %.08f };%s%s\n", *((uint32_t*)(&datum[0])), *((uint32_t*)(&datum[1])), *((uint32_t*)(&datum[2])), *((uint32_t*)(&datum[3])), type, datum_str, datum[0], datum[1], datum[2], datum[3], (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		const int scan_n = sscanf(io->line, "%08X %08X %08X %08X", (uint32_t*)&datum[0], (uint32_t*)&datum[1], (uint32_t*)&datum[2], (uint32_t*)&datum[3]); IO_READ_EXTRA;
		if(scan_n != 4) { IO_ERROR; }
	}
	return true;
}

//this has an issue with newlines, when we write out strings with newlines
//it formats them accordingly, but this reads in one line only, thus skipping
//a bunch of the line and returning an error.
//how to get this to work:

//change all newlines to some other char we never use
void change_char(char* str, char change, char to) {
	int i = 0;
	while(str[i] != '\0') { 
		if(str[i] == change) {
			str[i] = to;
		}
		i++;
	}
	return;
}
#define unused_char '\11'

int mini(const int x, const int y) { return x > y ? y : x; }
bool _io_string(io_t* io, char* datum, const int datum_n, const char* datum_str, const char* type, const char* file, const int line, const char* fmt, ...) { IO_SHARED;
	if(io->write) { IO_WRITE; //do my transform here
	char out[1024] = { 0 };
	strncpy(out, datum, 1024);
	change_char(out, '\n', unused_char);
	if(fprintf(io->fp, "%zd; %s_strlen\n", strlen(out), datum_str) <= 0) { IO_ERROR; }
	if(fprintf(io->fp, "%s; %s %s;%s%s\n", out, type, datum_str, (io->line[0] != '\0' ? " // " : ""), io->line) <= 0) { IO_ERROR; }
	} else { IO_READ;
		int input_datum_str_len;
		const int scan_n = sscanf(io->line, "%d", &input_datum_str_len); IO_READ_EXTRA;
		if(scan_n != 1) { IO_ERROR; }
		IO_READ; //do my transform here
		change_char(io->line, unused_char, '\n');
		strncpy(datum, io->line, input_datum_str_len); datum[input_datum_str_len] = '\0'; IO_READ_EXTRA;
		if(strlen(datum) != input_datum_str_len) { IO_ERROR; }
	}
	return true;
}