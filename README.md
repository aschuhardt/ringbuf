# ringbuf

A small, general-purpose ring buffer structure 

## Usage

Initialization:
```c
#define CAPACITY 32     // the total number records in the data structure
#define BLOCK_SIZE 1024 // the maximum size of each record

rb_buffer_t* buffer = rb_create(CAPACITY, BLOCK_SIZE);
```

Write:
```c
// copy a block of data into the tail of the structure
rb_insert(buffer, (void*)some_data, data_length);
```

Read:
```c
unsigned char* result;
size_t length;

// get a pointer to the most recently-inserted data (i.e. the 'tail')...
rb_get_tail(buffer, (void*)&result, &length);

// ...or to the data at some offset from the tail
size_t offset = 4;
rb_get_at(buffer, offset, (void*)&result, &length);
```

Cleanup:
```c
// free any resources allocated by the data structure
rb_destroy(buffer);
```
