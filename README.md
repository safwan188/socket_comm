# STNC: Student's Network Communication & Performance Test Utility 🚀

## Description 📝

STNC is a versatile communication tool developed in C, enabling basic chat functionality and network performance testing across various communication styles and protocols.

### Part A: Basic Chat Functionality 🗣️

Implement a chat tool that facilitates 2-way communication over the network using IPv4 and TCP protocol.

### Part B: Performance Testing 📊

Extend the chat tool to execute network performance tests, involving data generation, checksum creation, data transmission, and time measurement, reported to stdOut.

## Key Features 🗝️

- **Chat Tool:** Enable bilateral communication via network using a custom chat tool.
- **Network Performance Testing:** Evaluate network performance through data transmission tests across diverse communication styles and protocols.
- **Various Communication Styles:** Support for tcp/udp ipv4/ipv6, mmap file, named pipe, and Unix Domain Socket (UDS) in stream and datagram variants.
- **Automated Testing & Reporting:** Automatically conduct performance tests and report results.

## Usage 🛠️

### Client Side

```sh
stnc -c IP PORT -p <type> <param>
-p: Perform the test.
<type>: Communication type. Options: ipv4, ipv6, mmap, pipe, uds.
<param>: Type parameter. Options: udp, tcp or dgram, stream or filename.
Server Side
stnc -s PORT -p -q
-p: Perform the test.
-q: Enable quiet mode.
Communication Styles & Parameters 🔄
1. ipv4 tcp
2. ipv4 udp
3. ipv6 tcp
4. ipv6 udp
5. uds dgram
6. uds stream
7. mmap filename
8. pipe filename
Installation & Compilation 🖥️
Ensure you have a C compiler installed. Use the provided Makefile to compile the code:
make

