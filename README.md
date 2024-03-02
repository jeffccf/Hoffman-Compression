# Hoffman-Compression
Implement Hoffman compression and decompression on txt files using C.

The following commands compiles the files:

gcc -o Encode Hoffman_Encode.c

gcc -o Encode_text Encode_text.c

gcc -o Decode Decompression_Text.c


The following commands runs the program, assume the original text to be compressed is called news.txt

Encode news.txt codebook.txt

Encode_text news.txt codebook.txt compressed.txt

Decode compressed.txt codebook.txt output.txt

The output.txt should be the same as news.txt
