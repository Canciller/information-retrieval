#ifndef IR_READ_FILE_H
#define IR_READ_FILE_H

void read_file(const char *path, char **buffer, long *size = nullptr, bool zeroTerminated = false);

#endif