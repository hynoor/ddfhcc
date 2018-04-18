# DDFHCC

A software to delegate requests/response to/from outside of GFW.

### General Steps
- receive the request from client applications
- encrypt the the request with openssl toolkit
- build a simplified SOCK header(here I call aisock) on top of encrtypted data
- send the data to VPS which locates outside of GFW
- VPS server received encrtypted data then decipher the data
- VPS server fetch the destination from deciphered data
- VPS server send the the deciphered data to target 
- VPS received the responsed from target (ex: google.com)
- VPS encrypt the received data 
- VPS build the aisock header followed by the encrypted data
- VPS send the build data back to client
- client recive the data from VPS 
- client deciphers the payload of received data
- client send the deciphered data to applications


+++++++++++++                           |            ++++++++++++
client appl.                            |             google.com
+++++++++++++                           |            ++++++++++++
         ^                              |                 ^
         |                              |                 |
         |         ---------------    GREATE              |
         |--------> DDFHC (client)     FIRE               |
                   ---------------     WALL               |
                      ^                 |                 |
                      |                 |                 |
                      |                 |                 |
                      |                 |            ++++++++++++
                      |-----------------------------> VPS Server
                                        |            ++++++++++++



[Under Developing ...]
