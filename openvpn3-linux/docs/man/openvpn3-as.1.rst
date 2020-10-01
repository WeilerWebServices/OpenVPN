===========
openvpn3-as
===========

-----------------------------------
OpenVPN 3 Access Server integration
-----------------------------------

:Manual section: 1
:Manual group: OpenVPN 3 Linux

SYNOPSIS
========
| ``openvpn3-as`` [ OPTIONS ] ``URL``
| ``openvpn3-as`` ``-h`` | ``--help``


DESCRIPTION
===========
This utility can download a configuration profile directly from an
OpenVPN Access Server.  It requires an URL to the Access Server and will
query the user for authentication credentials before connecting to the
server.


OPTIONS
=======

-h, --help            show this help message and exit

--autologin
                      Download the auto-login profile (default:
                      ``user-login`` profile).  The auto-login profile will
                      not ask the user for credentials before connecting
                      to the Access Server, but this feature may not be
                      available for all user profiles.

--impersistent
                      Do not import the configuration profile as a persistent
                      profile (default: ``persistent``)

--name CONFIG-NAME
                      Override the automatically generated profile name
                      (default: :code:`AS:$servername`)

--username USERNAME
                      Username to use for OpenVPN Access Server login


SEE ALSO
========

``openvpn``\(8)
``openvpn3``\(1)
``openvpn3-config-manage``\(1)
``openvpn3-configs-list``\(1)

