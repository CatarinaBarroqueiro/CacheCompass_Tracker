# BLE Concepts

## Usage on the project

In geocaching is very important for the people to be able to guide themselves with some mechanism helping through the path. Normally the technology chosen for this goal is GPS. However, in certain locations the GPS signal might be weak or inexistent, leading to more difficulties on finding the stashes by the players.
Bluetooth Low Energy (BLE) comes in clutch in this situation, since it can be used between two devices with it, meaning we can use it to create a communication between the cache and the user's phone. This way, when the user gets close enough of the BLE range, he can be guided to the cache, without having to worry about GPS functioning.

Another problem with the usual geocaching is that bad actors can access the geocache, removing the items from it and not placing anything for the next people to find.
With our idea, this can be solved. Only when the person is really close to the cache, and confirms that he has found it, the cache opens. By clicking the confirmation, this person's information will be sent from the it's BLE device to the BLE server, that will after communicate by LoRa with the main website, in order to review the information sent and classify the person as authorized, or not, to open the cache.
This way, people not using the app or that are non-authorized, can not take what's inside the cache. 



## Concepts

### RSSI Obtainment (Scans)

### Service and Characteristics

### BLE Notify method (our method)

### Messages between server and app




