# Aeroponics System   
## Transmission Control Protocol (TCP) in MQTT   
### What is TCP?   
**Transmission Control Protocol (TCP)** is a connection-oriented communication protocol that ensures reliable, ordered, and error-checked data transmission between networked devices. It operates at the transport layer of the **OSI model** and provides a stable communication channel for MQTT messages.   
### Why Does MQTT Use TCP?   
MQTT is built on TCP because:   
- **Reliable Transmission**: TCP ensures that messages are delivered without duplication or loss.   
- **Ordered Delivery**: Packets arrive in the same order they were sent.   
- **Error Detection and Retransmission**: TCP handles corrupted or lost packets by requesting retransmission.   
- **Persistent Connection**: MQTT requires a stable connection for efficient message exchanges, which TCP provides.   
   
### How MQTT Works Over TCP   
1. The MQTT client initiates a TCP connection with the broker using the default port **1883**.   
2. Once connected, the client subscribes to topics or publishes messages.   
3. The broker maintains the connection and ensures the ordered delivery of messages.   
4. If the connection is lost, TCP will attempt to retransmit data before disconnecting.   
5. MQTT has built-in **Keep Alive** mechanisms to detect connection failures and reconnect automatically.   
   
### TCP Data Format in MQTT   
A standard MQTT packet over TCP consists of:   
- **Fixed Header**: Contains the control packet type and flags.   
- **Variable Header**: Includes information such as message ID and topic name.   
- **Payload**: The actual message content being transmitted.   
- **Length Fields**: Defines the total size of the packet.   
   
Example Packet Structure:   
```
+------------------+------------------+------------------+
| Fixed Header    | Variable Header  | Payload         |
+------------------+------------------+------------------+

```
## Secure Communication: SSL/TLS in MQTT   
### What are SSL and TLS?   
**Secure Sockets Layer (SSL)** and **Transport Layer Security (TLS)** are cryptographic protocols that secure data transmission over networks. TLS is the successor of SSL and provides better security, performance, and encryption standards.   
### Why Use SSL/TLS in MQTT?   
By default, MQTT over TCP does not encrypt data, making it vulnerable to various cyber threats such as:   
- **Man-in-the-Middle (MITM) Attacks**   
- **Packet Sniffing** (intercepting unencrypted data)   
- **Unauthorized Access** to messages   
   
Using SSL/TLS provides:   
- **Encryption**: Ensures that MQTT messages are securely transmitted.   
- **Authentication**: Validates the identity of the MQTT broker and client.   
- **Data Integrity**: Prevents message tampering during transmission.   
   
### How SSL/TLS Works in MQTT   
1. The client and broker perform a **TLS handshake** to establish a secure connection.   
2. The broker presents an **SSL/TLS certificate** to prove its authenticity.   
3. The client verifies the certificate using a trusted Certificate Authority (CA).   
4. A secure, encrypted session is established for message transmission.   
5. The client and broker communicate using **AES (Advanced Encryption Standard)** or other cryptographic methods.   
   
### SSL/TLS Data Format in MQTT   
An SSL/TLS packet structure typically consists of:   
- **Handshake Protocol**: Exchange of cryptographic keys.   
- **Record Protocol**: Secure transmission of application data.   
- **Encryption Algorithms**: Such as AES, RSA, or ECC.   
   
Example Packet Structure:   
```
+------------------+------------------+------------------+
| Handshake Data  | Record Protocol  | Encrypted Data  |
+------------------+------------------+------------------+

```
### MQTT Ports for Secure and Non-Secure Connections   
|       Protocol | Default Port |
|:---------------|:-------------|
|     MQTT (TCP) |         1883 |
| MQTT (SSL/TLS) |         8883 |

### Types of SSL/TLS Authentication in MQTT   
1. **One-Way Authentication (Server Authentication)**:   
    - The broker provides a certificate to the client for verification.   
    - The client trusts the broker and establishes a secure connection.   
2. **Two-Way Authentication (Mutual TLS)**:   
    - Both the client and broker exchange certificates for verification.   
    - Provides higher security, ensuring both parties are authenticated.   
   
## Benefits of Using SSL/TLS in MQTT   
- **Data Encryption**: Prevents attackers from intercepting or modifying messages.   
- **Authentication**: Ensures that only trusted devices communicate with the broker.   
- **Data Integrity**: Protects against message corruption and tampering.   
   
   
   
 MIT APP INVENTORY   
## MIT Designer   
![Screenshot from 2025-03-07 01-15-47.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-07%2001-15-47.png?raw=true)    
- Design the app using Designer   
- Add [UrsAI2PahoMqtt](https://ullisroboterseite.de/android-AI2-PahoDown.html)Client by adding the zip file to extensions and dropping it to designer from extension   
- Add broker and port for mqtt   
   
## MIT Blocks   
### Global Variables Initialization   
- **Global Variable:** `data`   
    - Initialized as an empty list.   
![Screenshot from 2025-03-04 12-33-42.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-04%2012-33-42.png?raw=true)    
   
### Button Click Event   
- **Trigger:** When `Button1` is clicked.   
- **Logic:**   
    - If `UrsPahoMqttClient1` is connected:   
        - Disconnect `UrsPahoMqttClient1`   
        - Call `CleanSession` function.   
    - Else:   
        - Connect `UrsPahoMqttClient1`   
![Screenshot from 2025-03-04 12-35-52.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-04%2012-35-52.png?raw=true)    
- **Trigger:** When Button2 is clicked.   
![Screenshot from 2025-03-07 01-47-53.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-07%2001-47-53.png?raw=true)    
   
### MQTT Connection State Change   
- **Trigger:** When `UrsPahoMqttClient1` connection state changes.   
- **Logic:**   
    - If connected:   
        - Change `Button1` background to green.   
        - Change `Button1` text to "Disconnect".   
        - Update `Label7` to prompt disconnection.   
        - Subscribe to `Battery143` topic with QoS = 1.   
    - Else:   
        - Change `Button1` background to red.   
        - Change `Button1` text to "Connect".   
        - Update `Label7` to prompt connection.   
![Screenshot from 2025-03-04 12-34-56.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-04%2012-34-56.png?raw=true)    
   
### MQTT Message Received Event   
- **Trigger:** When a message is received.   
- **Logic:**   
    - Check if the topic is `Battery143`.   
    - Split the message and store it in the global list `data`.   
    - Update `Label3` with the received value.   
    - Assign appropriate battery level images based on conditions:   
        - If value > 75%: `levels100-removebg-preview.png`.   
        - If 50% <= value <= 75%: `levels75-removebg-preview.png`.   
        - If 25% <= value < 50%: `levels50-removebg-preview.png`.   
        - Else: `levels25-removebg-preview.png`.   
    - Update `Label4` and `Label5` with values from `data`.   
    - Control visibility and image of `Image1`:   
        - If condition met, show `battery-charger.png`.   
        - Otherwise, hide `Image1`.   
    - Control `Label10` text and background:   
        - If condition met, set to "ON".   
        - Otherwise, set to "OFF".   
![Screenshot from 2025-03-04 12-38-03.png](https://github.com/anandhupanilkumar/aeroponics-system/blob/main/images/Screenshot%20from%202025-03-04%2012-38-03.png?raw=true)    
   
   
   
