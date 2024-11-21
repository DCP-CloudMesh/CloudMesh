# Messaging Framework

All communication between nodes in the network communicate through our custom message class defined in `message.h`. These messages contain the following information:
- Destination UUID
- Sender UUID
- Sender IP address
- Payload

The `Payload` type is an abstract class that can be populated with specific payload types:
```
UNKNOWN
REGISTRATION
DISCOVERY_REQUEST
DISCOVERY_RESPONSE
TASK_REQUEST
TASK_RESPONSE
```

If any new payload types need to be created, add a new enum type to `payload.h` and implement a corresponding payload child class.
