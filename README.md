# rivermesh
ENG EC500 L2 Course Project
By Shiza Ali, Anthony Byrne, and Hammas Saeed

## Introduction
This is the source code for the "RiverMesh" project, where we explored the use of LoRa mesh networking for data transfer between multiple sensor-equipped "source" nodes and a single cellular-equipped "sink" node. We evaluate the latency and fault-tolerance of our application using both YAKINDU Statechart-based and UPPAAL-based virtual models as well as Arduino-based physical models. We hope that the information we’ve collected throughout this project will help biologists and local wildlife authorities better understand our waterways’ fragile but crucial ecosystems.

## Usage
### Physical Model
Install the Arduino IDE and import our modified version of the RadioHead library. Open each directory starting with `rf95_mesh` in the IDE and flash it to an Adafruit Feather M0 with 900MHz RF95 Lora Radio. `rf95_mesh_client`, `rf95_mesh_server1`, and `rf95_mesh_server2` are the source/carrier nodes. `rf95_mesh_server3` is the sink node. Wire each node according to the wiring diagram in our final project report.

Connect all 4 nodes to power and enable the toggle switch to begin sending colors through the mesh. Have fun!

### Virtual Models
Open `YAKINDU/default.sct` in YAKINDU State Charts to view our YAKINDU Model.

Open `UPPAAL/project.xml` in UPPAAL to view our UPPAAL model.

See our final report for more details