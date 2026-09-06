#ifndef BUFFER
#define BUFFER

struct buffer {
  char *name;
  int fd;

  char **lines;
  int line_count;
  int row_offset;
  int file_x;
  int file_y;
};

struct buffer *add_buffer(struct buffer *buff);
int remove_buffer(struct buffer *buff);
int exists_buffer(struct buffer *buff);
struct buffer *find_buffer_by_name(char *name);
void free_buffer_lines(struct buffer *buff);

extern struct buffer **buffer_list;
extern int buffers_counter;

#endif
