README – Ex3 Submission

Authors:
- Hadar Ashkenazi
- Shir Luzon
- Noa Dinbar

## Project Name:
Mta_Crypto_Containers

## Docker Images

- Encrypter Image: [hadarash/mta_crypt_encrypter](https://hub.docker.com/r/hadarash/mta_crypt_encrypter)
- Decrypter Image: [hadarash/mta_crypt_decrypter](https://hub.docker.com/r/hadarash/mta_crypt_decrypter)


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

## Prerequisites
- Docker installed and running
- Bash (on Linux or macOS)
- `mtacrypt.conf` file in the project root

## Build & Run Instructions:
To run the encrypter and a desired number of decrypters, execute the following command and provide the number of decrypters to create:
./launcher.sh <num_of_decrypters>
After running this command, the IDs of the created encrypter and decrypter containers will be printed to the screen.

To manually create a single container of either the encrypter or a decrypter, use the following command (depending on the desired image):

sudo docker run -d -v /tmp/mtacrypt:/mnt/mta mta_crypt_encrypter
(for running first time, instead of running launcher.sh)

or

sudo docker run -d -v /tmp/mtacrypt:/mnt/mta mta_crypt_decrypter
(and for manually adding another decrypter while running)

This will print the ID of the newly created container.

To enter an existing container's shell, run:
sudo docker exec -it <container_id_or_name> /bin/bash

Once inside the container, to view its log messages, run:
tail -f /var/log/mtacrypt.log


