# thiolsTemp
Just a private project for a temperature regulation system


Application structure
=====================

static configuration
--------------------
All the static configurations are written in the common.h header

configuration files
-------------------

main configuration folder : /etc/thiolsTemp/

bluetooth hci node : hcinode


- hcinode
---
Contains only the hci node number that must be passed to the hcidump function




Processes
=========

main
----
The main process must:
- launch the sub processes
- monitor the sub processes and detect any failure


bleListener
-----------
This process uses the hcidump command and pipe the output to the bleParser


bleParser
---------
This task must parse the incoming ble frames and find the mac adresses configured in the bleNodes file

When a node is found, the content must be sent to the database using the appropriate library.


dbBackup
--------
This process takes care to periodically backup the database to flash


tempRegul
---------
This process manages the temperature regulation



Libraries
=========

dbInfos
-------
Manage all the database accesses
- database object structure
- open/close database
- register temperature point
- get last temperature point
- get average temperature on xx time
- get current temperature thresholds


Database
=========

- Nodes
Contains the know nodes with mac addresses and flag to indicate the main sensor


