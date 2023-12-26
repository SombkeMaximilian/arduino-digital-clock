# ifndef REGISTERMACROS_H
# define REGISTERMACROS_H

// ------------------------------------------------------------ //
// macros for bit manipulation of IO registers

# define set_io_bit(port, bit) (port |= (1 << bit))
# define clear_io_bit(port, bit) (port &= ~(1 << bit))
# define change_io_bit(port, bit, value) (value == 1) ? set_io_bit(port, bit) : clear_io_bit(port, bit);

# endif