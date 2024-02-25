# File2Hex2File
A simple collection of tools for converting any filetype to its hex value, or a hex to a file (C++).

![Screenshot of File2Hex GUI](dev/Screenshot%202024-02-25%20095202.png)



## File2Hex GUI
- Simply compile this program and use it!
- Comes complete with menu options Open (CTRL + O), New Window (CTRL + N), Reset Window (CTRL + R), Save Hex (CTRL + S) and Quit (CTRL + Q)
- Easily convert ANY filetype to its hex value
```
g++ -o F2H_GUI.exe File2Hex-GUI.cpp -mwindows -lcomdlg32
```

## File2Hex, Hex2File CLI Programs
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
