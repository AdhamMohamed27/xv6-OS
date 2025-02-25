# xv6 OS Enhancement

This repository contains modifications and enhancements to the xv6 operating system. We have added new features, system calls, and improvements to existing functions.

## Features Added

### 1. **Merge Sort Implementation**
- Added a file that implements the **merge sort algorithm**.
- This can be used for sorting arrays efficiently in the kernel.

### 2. **printstate System Implementation**
- A new system call **`printstate`** has been added.
- It takes a **command-based input** and prints the following statistics:
  - **Mean**
  - **Maximum Value**
  - **Minimum Value**
  - **Standard Deviation**
- This is useful for analyzing numerical data within xv6.

### 3. **save System Implementation**
- Introduced a new system call **`save`**.
- Functionality:
  - Takes a **file name** and **text input**.
  - Appends the text to the specified file.
- This allows writing data to files from within the xv6 kernel.

### 4. **atoi Function Improvement**
- The **`atoi` (ASCII to Integer) function** has been modified.
- Now supports **negative numbers**, improving compatibility with various integer-based operations.

## Compilation and Testing
To compile xv6 with these modifications:
```sh
make clean
make qemu
```
To test the new system calls, run the appropriate user-space test programs after booting xv6.

## Future Enhancements
- Implement additional system calls for file handling.
- Add support for floating-point arithmetic in the kernel.
- Optimize existing functions for better performance.

## Contributors
- [Your Name]
- [Other Contributors]

## License
This project follows the original xv6 licensing terms under MIT License.

