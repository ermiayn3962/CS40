struct three_unsigned_longs {
        unsigned long a, b, c;
};

/*
 * Uses sscanf() to read three unsigned long integers from
 * inputline, and places the results in the fields of the
 * structure pointed to by p. If sscanf() fails to read these 
 * three values, explode the bomb!   
 */
void read_three_unsigned_longs(const char *inputline,
                               struct three_unsigned_longs *p);
