## Usage:
```
make && make clean
./ac [string-file] [pattern-file] [result-file]
```
note that `[string-file]`, `[pattern-file]`, `[result-file]` are all file names


## File Format:
Note that all encodings are supported as long as string-file and pattern-file are of same encoding.

For `[pattern-file]`, each pattern contains one line, example:
```
abc
ab
c
avcd
```

For `[result-file]`, each result will contain one line, including the pattern and the offset of the pattern, example:
```
abc 0
a 0
b 1
```
