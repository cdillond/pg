# About
pg is a very basic CLI utility for generating a random string of ASCII characters. The returned string is guaranteed to contain at least 1 lowercase, 1 uppercase, 1 numeric, and 1 special character. It takes 2 optional arguments. The first argument is the desired length of the returned string, which must be between 4 and 1024 characters. If the length argument is not supplied, the output string defaults to 12 characters. The second argument is the newline flag, 'n'. If this argument is provided, then a linefeed character is appended to the program's output.