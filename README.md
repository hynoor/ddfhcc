# DDFHCC

A software to delegate requests/response to/from outside of GFW.

### General Steps
- receive the request from client applications
- build a simplified SOCK header(here I call aisock) on top of application data
- encrypt the data in above step with openssl toolkit
- send the data to VPS which locates outside of GFW
- VPS server received encrtypted data then decipher the data
- VPS server fetch the destination from deciphered data
- VPS server send the the deciphered data to target 
- VPS received the responsed from target (ex: google.com)
- VPS build the aisock header followed by the received data
- VPS encrypt the received data 
- VPS send the build data back to client
- client recive the data from VPS 
- client deciphers the payload of received data
- client send the deciphered data to applications

```


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
                      |                 |            +++++++++++++++++++++++++++
                      |-----------------------------> VPS Server DDFHCC (server)
                                        |            +++++++++++++++++++++++++++

```

[Under Developing ...]
