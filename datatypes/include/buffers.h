#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#define ARRAY_LENGTH(array, size) (size / sizeof(*array))
#define ARRAY_SIZE(array, length) (length * sizeof(*array))

#endif // __BUFFERS_H__
