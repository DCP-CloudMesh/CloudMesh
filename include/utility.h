#ifndef __UTILITY__
#define __UTILITY__

#include "proto/payload.pb.h"
#include "proto/utility.pb.h"

#include <arpa/inet.h>
#include <filesystem>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

namespace fs = std::filesystem;

/*
 * Defines the universal buffer size used in FTP. Packets are transferred
 * blocks of bytes limited to the specified size.
 */
#define FTP_BUFFER_SIZE 4096

#define MIN_PORT 49152
#define MAX_PORT 65535
#define MAX_PORT_TRIES 10

/*
 * Defines the data location of training files and index files for the requestor.
 */
const std::string SOURCE_DATA_DIR = "CIFAR10/train";

/*
 * Defines the data location of training files being used locally by a provider.
 */
const std::string TARGET_DATA_DIR = "data/CIFAR10";


struct IpAddress {
    std::string host;
    unsigned short port;

    IpAddress() {}
    IpAddress(const std::string& host, const unsigned short port);

    friend std::ostream& operator<<(std::ostream& os, const IpAddress& ip);
};

utility::IpAddress* serializeIpAddressToProto(const IpAddress& ipAddress);
IpAddress deserializeIpAddressFromProto(const utility::IpAddress& proto);

typedef std::unordered_map<std::string, IpAddress> AddressTable;

utility::AddressTable* serializeAddressTable(const AddressTable& addressTable);
AddressTable deserializeAddressTable(const utility::AddressTable at);

namespace uuid {
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string generate_uuid_v4();
} // namespace uuid

std::string vectorToString(std::vector<int> v);

int FTP_create_socket_client(int port, const char* addr);

int FTP_create_socket_server(int port);

int FTP_accept_conn(int sock);

/*
 * Resolves the path of a file within the data directory.
 * Accepts a filename and returns a relative path.
 */
fs::path resolveDataFile(const std::string filename);

/*
 * Resolves the path of a file within a directory.
 * Accepts a filename, directory and returns a relative path.
 */
fs::path resolveDataFileInDirectory(const std::string filename,
                                    const std::string dir);

/*
 * Verifies if a file is present in a directory. Accepts
 * a filename and directory as input.
 */
bool isFileWithinDirectory(const std::string& filename, const std::string dir);

/*
 * Generates a random port number that is available for use between MIN_PORT and
 * MAX_PORT.
 */
int get_available_port();

std::vector<std::string> getMatchingDataFiles(const std::regex& pattern,
                                              const std::string& directory);

std::regex convertToRegexPattern(const std::string& pattern);

#endif // __UTILITY__
