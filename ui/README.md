# user interface variants and versions

No longer maintaining separate directories with the specific variants of user interfaces as this causes problems when an update is applied and there is a need to replicate this across all variants. Therefore, only the default user interface is at the latest version and building variants involves changing the necessary parameters and building separately.

## Variants

To build the different variants of this UI make the following changes:

- In modules/logging.js toggle logging to true or false
- In App.vue update the loadControlVersion data parameter to the appropriate value
- in ConnectAndDisplay.vue choose whether to display the load force arrow on the display


| Variant        | default              | no-force              | open-day              |
| ---------------| ---------------------| --------------------- | --------------------- |
| Latest version | v2.1.0-default       | v2.1.0-no-force       | v2.1.0-open-day       |
| Description    | Fully functional UI without logging interactions        | Includes logging of interactions and removes display of load cell data | Simplified control interaction with no logging interactions |

The original directories for variants have been moved to old/
