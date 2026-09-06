#ifndef FILE_H
#define FILE_H

struct buffer *open_file(char *filename);
void close_file(struct buffer *buff);

#endif
