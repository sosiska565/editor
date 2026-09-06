#ifndef BUFFER
#define BUFFER

struct buffer {
  char *name;
  int fd;
};

struct buffer *add_buffer(struct buffer *buff);
int remove_buffer(struct buffer *buff);
int exists_buffer(struct buffer *buff);
struct buffer *find_buffer_by_name(char *name);

extern struct buffer **buffer_list;
extern int buffers_counter;

#endif
