# File2Hex2File
A simple collection of tools for converting any filetype to its hex value, or a hex to a file (C++).

```
g++ File2Hex-CLI.cpp -o F2H_CLI -static
g++ Hex2File-CLI.cpp -o H2F_CLI -static
```

CLI Usage
- ./exe <originalFile> <outputFile>
```
./F2H_CLI payload-original.exe hexedFileData
./H2F_CLI hexedFileData payload-restored.exe
```
