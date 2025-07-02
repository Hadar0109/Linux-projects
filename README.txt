README – Ex2 Submission

Authors:
- Hadar Ashkenazi
- Shir Luzon
- Noa Dinbar

## Project Name:
Mta Crypto


## Project Instructions:
Before compiling and running the program, you need to install mta-utils-dev library:

To install the correct mta-utils-dev package based on your CPU architecture:

For x86_64 systems- run:
sudo dpkg -i mta-utils-dev-x86_64.deb

For ARM64/aarch64 systems- run:
sudo dpkg -i mta-utils-dev-aarch64.deb

(To check your system’s architecture, use the command: uname -m
If the result is x86_64, use the x86_64 package.
If the result is aarch64 or arm64, use the aarch64 package.)


## Build & Run Instructions:
1. To compile all required components- run:
make

2. To run the program- run:
./encrypted.out -n <num_of_decrypters> -l <password_length> [-t <timeout_seconds>]

Supported Flags:
-n : Determines how many decrypter threads will be created.
-l : The number of characters to be encrypted. The more characters, the harder it is for the decrypters to crack the password.
-t (optional): Timeout in seconds. If no correct password is received within this time, the server will regenerate a new one.
