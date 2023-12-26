# ifndef REGISTERMACROS_H
# define REGISTERMACROS_H

// ------------------------------------------------------------ //
// macros for bit manipulation of IO registers

# define set_io_bit(port, bit) (port |= (1 << bit))
# define clear_io_bit(port, bit) (port &= ~(1 << bit))
# define change_io_bit(port, bit, value) (value == 1) ? set_io_bit(port, bit) : clear_io_bit(port, bit);


// ------------------------------------------------------------ //
// macros for converting between binary coded decimal and decimal
// IMPORTANT: only works for 0 =< n =< 99

# define bcd_to_dec(bcd) ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F)
# define dec_to_bcd(dec) ((dec / 10)   << 4) + (dec % 10)

# endif