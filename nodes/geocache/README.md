## Distance calculation from geocache using BLE

Our idea is to calculate the distance from the geocache and the person (with it's personal device) using BLE.

The idea behind this is to use RSSI (Received Signal Strength Indicator). By sending a beacon to the receiving device we can see the strength with which the signal was received. This strength will depend on two factors: distance and Broadcasting Power value.

In order to calculate the approximate distance between the device and the beacon we need another value defined by the iBeacon standard: Measured Power. 

Measured Power is a factory-calibrated, read-only constant which indicates what's the expected RSSI at a distance of 1 meter to the beacon. Combined with RSSI, it allows estimating the distance between the device and the beacon.

We decided to implement this using a BLE server and a BLE scanner. The server acts as a **beacon**, since it advertises its presence and allows nearby devices to get Rssi values.
By utilizing the BLE service and characteristic, our implementation establishes a standardized communication protocol, enabling the BLE scanner to efficiently retrieve RSSI values from the BLE server. 

Scanning Interval is also important to take into consideration when calculating distance values. If Scanning Interval is higher, we will not have such precised indications of the correct direction, because the person can have moved a lot or some unexpected movements can have happened.

The distance can be calculated using the formula Distance = 10 ^ ((Measured Power – RSSI)/(10 * N)).

## Tests/Ideas

Now lets take into consideration outside factors than can compromise the accuracy of the distance calculation.
The distance estimates gets worse at greater distances. This is due to various factors such as:
- Obstacles and Reflections
- Environmental Changes > movement of people, objects
- Signal Attenuation > air humidity, temperature, atmospheric conditions
- Device Orientation
- etc.

A possible alternative to use BLE Rssi, could be the usage of **UWB (Ultra-wideband)**. UWB is a technology that uses a very wide frequency range to transmit data and it is characterized by the ability to transmit a high amount of data over a short distance, with very low power consumption (power comsuption similar to BLE).
Benefits of UWB over BLE:
- More accurate, higher precision for location tracking
- Longer range > UWB can transmit signals over a longer distance
- UWB signals can pass through solid objects such as walls and floors > this makes it easier to track a geocache even if it's behind a wall or a tree, for example
- Resistance to interference from other wireless signals

Although the presence of all the advantages of UWB over BLE, presented above, the UWB technology is generally more expensive to implement compared to BLE. The energy efficiency is also better in BLE.