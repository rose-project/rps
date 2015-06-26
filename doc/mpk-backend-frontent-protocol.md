# MPK Backend - Frontend Protocol

## Definitions

### Revision

A revision number of a package identifies a specific state of a packge. The 
revision number starts with 0 and grows with each new revision of the package. 
The revision 1 describes the first real revision, revision 0 means the 
non-existence of the package. 


## Archtitecture

A single *device software revision manager* (DSRM) is running on a server and 
monitors software revisions of clients and manages the installation of software 
packages on these clients. A client is typically a device which has one ore more
software packages installed on it.

Communication between manager and client is handled via JSON-RPC.


### Client

The client is defined by the software and its version that is installed on it.
A client may request installation of new software, up- or downgrading of
software that is already installed or removing of software. Every request is
always managed by the DSRM to which the request is send.

Every client has to report its state to the DSRM. The DSRM can send install or
uninstall requests to the client.

Each client has a list of features that may enable specific software for the
client. These features may changes during life time of the client. Each
feature change has to be requested at the DSRM.

#### Client Properties

| Name       | Description                                                |
|------------|------------------------------------------------------------|
| vendor_id  |                                                            |
| product_id |                                                            |
| device_id  | A unique identifier for the client that never changes.     |
| name       | A human readable name of the client.                       |
| features   | Client features that enable specific software.             |
| packages   | List of packages and their versions currently installed.   |
| release    | The release branch the client currently uses.              |

Any device can be identified uniquely by the following parameters:
vendor\_id, product\_id and device\_id which altogether form the serial number.

For example:
01ab2412 e1e2a123 abcd1234a1b2d3e4
where vendor\_id = 0x01ab2412, product\_id = 0xe1e2a123 and device\_id = 
0xabcd1234a1b2d3e4


### Device Software Revision Manager

#### Client Table

The client table contains one line per client and contains all information the
DSRM needs about the client.

| Field     | Description                                                |
|-----------|------------------------------------------------------------|
| device_id | A unique identifier for the client that never changes.     |
| vendor_id |                                                            |
| name      | A human readable name of the client.                       |
| features  | Client features that enable specific software.             |
| packages  | List of packages and their versions currently installed.   |
| release   | The release branch the client currently uses.              |


#### Package Table

The package table contains a line per package and version which describe the 
information the DSRM needs for handling software packages.

| Field       | Description                                                |
|-------------|------------------------------------------------------------|
| name        | package name                                               |
| revision    | uinique revision of the package                            |
| release     | the release set this package is part of                    |
| depends     | packages that have to be installed                         |
| conflicts   | package that are not allowed to be installed               |
| requires    | features the client has to fulfill to 'see' the package    |
| version     | (human readable) version of the package                    |
| description | description of the package                                 |
| source      | place the package can be copied from                       |


## Status Reports

### Device Status Report

#### Method `status`

A devices sends its status to the DSRM to report any change in its system. The
DSRM will anwer if it has accepted the device status.

***Request***

Sender: Device

| Parameter | Description                  | Example                     |
|-----------|------------------------------|-----------------------------|
| device_id | the device id                | 0x0123456789abcdef          |
| device_id | the device id                | 'HVAC'                      |
| release   | id of the release set        | 'rivendell-1.2'             |
| | |


    {
        "jsonrpc": "2.0",
        "method": "status",
        "params": {
            0x0123456789abcdef,
            'rivendell-1.2',
            ''
        },
        "id": 1
    }

*** Response ***

    {
        "jsonrpc": "2.0",
        "result": 0,
        "id": 1
    }

*** Error Response ***

    {
        "jsonrpc": "2.0",
        "error": {
            "code": 5,
            "message": "unknown device"
        },
        "id": null
    }


## Method `getRevisions`

### Request
Sender: Device
Parameter: package name, revision

Get a specific revision of one or more packages. This includes installation and 
uninstallation of packages. The response of the GetRevision call is a list 
of packges the system has to install or remove. 

Example: install revision 123 of 'package-a' and revision 2 of 'package-b', 
remove 'package-b'

    {
        "jsonrpc": "2.0",
        "method": "getRevisions",
        "params": {
            [
                { "package-a", 123 },
                { "package-b", 2 },
                { "package-c", 0 }
            ]
        },
        "id": 1
    }

### Response
Sender: Server
Parameterst: list of package-names, revisions and URIs of the packages to 
install or remove

The server answers with a list of packages (and their dependencies) to install 
or remove. The order of this list is also the order in which the installation 
should take place. 

Example: response to the request shown above

    {
        "jsonrpc": "2.0",
        "result": {
            [
                { "package-b", 2, 'http://mypackages.de/package-b_2.mpk },
                { "package-a", 123, 'ftp://mypackages.de/package-a_123.mpk },
                { "something", 123, 'ftp://mypackages.de/package-a_123.mpk },
                { "abc", 123, 'ftp://mypackages.de/package-a_123.mpk },
                { "package-c", 0 , ''}
            ]
        },
        "id": 1
    }
 

## Method `getRelease`

Update the device to a new release set. 

TODO

## Method `setRevision`

Offer the device new resisions of packages. The client may decline the request. 

### Request
Sender: Server

### Response


TODO

## Method `setRelease`

Request installation of a release to the device. The device may decline this 
request. 

