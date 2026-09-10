# user interface variants and versions

No longer maintaining separate directories with the specific variants of user interfaces as this causes problems when an update is applied and there is a need to replicate this across all variants. Therefore, only the default user interface is at the latest version and building variants involves changing the necessary parameters and building separately.

| Variant        | default              | no-force              | open-day              |
| ---------------| ---------------------| --------------------- | --------------------- |
| Latest version | v2.1.0-default       | v1.0.0-no-force !       | v2.0.0-open-day       |
| Latest release | NA                   | NA                    | NA
| Description    | Fully functional UI without logging interactions        | Includes logging of interactions and removes display of load cell data | Simplified control interaction with no logging interactions |

! currently being updated to v2

Further details of the differences between variant UIs and versions can be found in the corresponding directories:

- [default](./default/)
- [no-force](./no-force-v1/)
- [open-day](./open-day/)