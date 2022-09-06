# odroid-truss
How to set up an Odroid to run a truss experiment. 

The process is fairly simple
0. Obtain an eMMC card with Ubuntu 20.04 LTS 
0. Connect Odroid to power, network and a Nano IoT33 
0. Log in to check you have ssh access
0. Edit the sudoers file to allow passwordless sudo
1. Run a configure script on your admin machine
2. Run three ansible playbooks on your admin machine (which set things up on the rpi over ssh)
3. Reboot your Odroid
3. Your experiment should be working

## Abbreviations

- BYOD - Bring Your Own Device
- OS - Operating System
- SBC - single board computer (Raspberry Pi, Odroid, etc)

## Formatting

### Trimming information

`<snip>` is used to denote that line(s) have been removed for clarity

```
something useful
<snip>
other useful stuff
```

#### repo location

`$this_repo` is used to indicate the root of the git repo for clarity in the documentation, and does not actually occur in any of the scripts.

### Indicating where to enter commands?

Code snippets will sometimes clarify which machine they should be run on, if it is not obvious from the context. E.g. your local admin machine:
```
# local
$ <some commands>
```

or on the SBC (via the ssh connection you've established from your local machine). 

```
# sbc
$ <some command>
```



## Prequisites

### Hardware

This is our tested configuration. 

- Odroid C4 
- 32GB eMMC preloaded with ubuntu 20.04 LTS (or a blank eMMC and an adapter so you can image it)
- USB A to micro B cable for arduino
- Arduino Nano IOT33
- USB Webcam (Logitech C920)
- 12V power supply with barrel jack connector
- RJ45 Ethernet connection (avoid WiFi because it is laggy for users)
	
For assembling the full experiment, you will also need some additional items that are outside the scope of this repo:
- Truss PCBs including sensor and connector boards
- Truss hardware (truss, guages, actuator, load cell)
- Experimental tray and 1/6th scale shipping container

### Software

The setup is best done from a linux machine. We have no plans to support commercial operating systems.

- Linux 
   - recommend Ubuntu 20.04 LTS
- Practable's [relay](https://github.com/practable/relay) 
    - ensure the `sessionrelay` and `shellrelay` commands are in your path 
- Ansible 
   - Tasks developed with v2.9.17 and python 3.7.4
   - [Installation](https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html)
   
### Information

You will need either access tokens or the access secret(s) to your relay(s). Your site administrator for practable will advise. These scripts are written for the case you have the secret. If you have been provided some tokens, you just copy them into `$this_repo/autogenerated` and skip some steps where indicated.

We assume that your relay access secret is the same for all the relays in use, and available at `~/secret/practable.pat`. That way, it's not in the repo.

### Network

You need a wired staging network where unknown devices can obtain an IP address via DHCP. Your admin machine is ideally also in the same subnet as your RPI. If you are at work/university, then you will need to create your own staging network because most institutional networks prevent unknown devices from accessing the wired networks. Without a staging network, you are are stuck in a catch-22 of not knowing enough about the device to get that permission for access to the production network (typically a BYOD network for devices that staff and students might bring in).

Good staging networks: your home network (no re-configuration is needed) or a linux laptop with a wifi connection and an RJ45 dongle (share the wifi to the dongle) - I use one for initial setups and the other for ad-hoc direct connections to experiments during field-testing.

### User environment

#### Mandatory

None listed

## Setup

Overview: The process starts by loading an OS onto an SD card, modifying it to permit `ssh` access, then finishing the setup over an ssh connection. There are a mixture of steps conducted on the SBC

### SBC OS

#### Base Image

Obtain a preloaded eMMC with Ubuntu 20.04 LTS, or image this to your blank eMMC. The most likely looking offical distro is [this](https://odroid.in/ubuntu_22.04lts/C4_HC4/ubuntu-22.04-4.9-minimal-odroid-c4-hc4-20220705.img.xz).  

SSH is enabled by default.


#### Find the IP address of your SBC

Before booting your SBC, you need to figure out the IP addresses of other devices on your network. In the next step, we will be looking for the IP address of the SBC, and it is easier if you already know which devices are NOT the SBC.

First, let's figure out the IP address of the wired ethernet port on your admin machine 
```
$ ip addr 
<snip>
3: enp0s31f6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether xx:xx:xx:xx:xx:xx brd ff:ff:ff:ff:ff:ff
    inet 192.168.0.46/24 brd 192.168.0.255 scope global dynamic noprefixroute enp0s31f6
       valid_lft 784687sec preferred_lft 784687sec
    inet6 xxxx::xxxx:xxxx:xxxx:xxxx/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
<snip>
```
Our network card is at 192.168.0.46. The first three number groups matter here. Find all the devices on the same sub-net, using that IP address by set the last number group to `1/24` which is [shorthand](https://www.ietf.org/rfc/rfc1878.txt) for 0-254 (all possible IP in the subnet).
```
$ sudo nmap -sP 192.168.0.1/24
sudo nmap -sP 192.168.0.1/24
Starting Nmap 7.80 ( https://nmap.org ) at 2022-08-25 13:18 BST
Nmap scan report for _gateway (192.168.0.1)
Host is up (0.0016s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.62
Host is up (0.00059s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.52
Host is up (0.036s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.184
Host is up (0.00027s latency).
Nmap done: 256 IP addresses (3 hosts up) 
```

Put the SD card in your Raspberry Pi, connect the network cable, and power on. Wait about a minute (or repeat until a new device shows up)


```
$ sudo nmap -sP 192.168.0.1/24
sudo nmap -sP 192.168.0.1/24
Starting Nmap 7.80 ( https://nmap.org ) at 2022-08-25 13:20 BST
Nmap scan report for _gateway (192.168.0.1)
Host is up (0.0016s latency).
MAC Address: yy:yy:yy:yy:yy:yy (Arris Group)
Nmap scan report for 192.168.0.29
Host is up (0.032s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.62
Host is up (0.00059s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.52
Host is up (0.036s latency).
MAC Address: xx:xx:xx:xx:xx:xx 
Nmap scan report for 192.168.0.184
Host is up (0.00027s latency).
Nmap done: 256 IP addresses (4 hosts up) 
```
Now you should see a new device. If this step does not work, then the eMMC is corrupt, the network cable/power are disconnected, or the router on your network is not permitting DHCP requests from unknown devices.

In this case the IP address is 192.168.0.29.

Make a note of the MAC address of your SBC so that you can share that with your network administrators at the University - sometimes you must declare the MAC address to be able to access a wired BYOD network.

#### ssh into your SBC

The standard user and password on the odroid image is usually `odroid/odroid`

On your machine, open a terminal
```
$ ssh pi@192.168.0.29 #change to whatever IP you found in previous step

The authenticity of host '192.168.0.29 (192.168.0.29)' can't be established.
ECDSA key fingerprint is SHA256:/0jZ9w8zL4h94rojSnSQ0Yewv5G8qK2AjCod/09pBkE.
Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
Warning: Permanently added '192.168.0.29' (ECDSA) to the list of known hosts.
pi@192.168.0.29's password: <--- enter the password here 

```
Then you should see something like this:

```
Welcome to Ubuntu 20.04 LTS (GNU/Linux 4.9.218-13 aarch64)

 * Documentation:  https://help.ubuntu.com
 * Management:     https://landscape.canonical.com
 * Support:        https://ubuntu.com/advantage

 * Super-optimized for small spaces - read how we shrank the memory
   footprint of MicroK8s to make it the smallest full K8s around.

   https://ubuntu.com/blog/microk8s-memory-optimisation

607 updates can be installed immediately.
426 of these updates are security updates.
To see these additional updates run: apt list --upgradable

Last login: Fri Jul  1 20:11:11 2022 from 192.168.0.46
odroid@odroid:~$  
```

##### Password

Change the password 

```
$ passwd
```

Ideally we would also set up ssh certificates but this is a future step.


##### Sudo

Set up passwordless sudo

```
#sbc
sudo visudo 
```

At the end of the file add

```
odroid ALL=(ALL) NOPASSWD: ALL
```

Save the file. The default editor might be joe, in which case you hit `^KX` (i.e. hold CTRL, type KX, release CTRL) to save. Assuming you made no errors, you will see:

```
File /etc/sudoers.tmp saved
```

Once the sudoers file is edited, you can check it by logging back out and back in, then trying 

```
$ sudo su
root@odroid:/home/odroid# 
```

which should work without needing a password.

You can now exit the session - we will do the next steps using ansible.

```
# sbc
$ exit
```

### SBC software and configuration

We need to generate some configuration files that are customised to your experiment. Your local admin for the practable system will advise on any naming conventions that apply at your site, as well as providing the relay key(s) which you need. 


#### Ansible hosts file

We need to let ansible know about this host.

```
sudo nano /etc/ansible/hosts
```
Add in a section like this (or modify an existing section if you have one)

```
<snip>
[truss]
trus00 ansible_host=192.168.0.29 practable_label=trus00

[truss:vars]
ansible_connection=ssh
ansible_user=odroid
ansible_password=somepassword
<snip>

```



#### Practable connections

We need to configure routing and tokens for the video, data, shell, and shell2 connections. There are scripts provided to help you. They put files into `$this_repo./autogenerated`

Create the configuration files we need, assuming we are using the standard practable relay URLs.

```
#local
cd scripts
.configure truss 00 00 https://relay-access.practable.io https://shell-access.practable.io https://shell-access2.practable.io
```

##### Session

Now we can run our ansible playbooks - these each take some time, so get the kettle ready. It's nearly as fast doing multiple machines, because the playbooks are run in parallel. There is no need to do each SBC separately one after another.

```
#local
cd playbooks
ansible-playbook install-session
ansible-playbook install-shell
ansible-playbook install-firmware
```

If those commands go smoothly, then you are done with the install. If not, check out the troubleshooting guide below.

## Checking the installation

Go to the booking page link the adminstrator gave you, and you should be able to access/control the experiment. 

## Troubleshooting

If things are not working right, then you might be able to fix them by troubleshooting. 

### No DHCP

This is usually caused by 
- eMMC corrupt 
- network administrator is unaware of the MAC address of the wired network port on the SBC

### No SSH connection

For odroid, `sshd` is enabled by default, so you will need to use a keyboard and mouse to re-enable, or reflash (depending on whether there is anything on the eMMC you want to keep).

### Remote host identification has changed

If you work with a lot of experiments, sooner or later an old IP address on your staging network will be re-assigned to a new experiment you are setting up, and then you will see something like this:

```
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@    WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!     @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
IT IS POSSIBLE THAT SOMEONE IS DOING SOMETHING NASTY!
Someone could be eavesdropping on you right now (man-in-the-middle attack)!
It is also possible that a host key has just been changed.
The fingerprint for the ECDSA key sent by the remote host is
SHA256:/0jZ9w8zL4h94rojSnSQ0Yewv5G8qK2AjCod/09pBkE.
Please contact your system administrator.
Add correct host key in /home/tim/.ssh/known_hosts to get rid of this message.
Offending ECDSA key in /home/tim/.ssh/known_hosts:178
  remove with:
  ssh-keygen -f "/home/tim/.ssh/known_hosts" -R "192.168.0.184"
ECDSA host key for 192.168.0.184 has changed and you have requested strict checking.
Host key verification failed.
```

Simply run the suggested command, and then carry on where you left off with trying to connect over `ssh`. The command is unique each time it happens (you're overwriting an entry in the known-hosts file so it must correspond to the host causing the problem _this_ time). E.g.

```
$ssh-keygen -f "/home/tim/.ssh/known_hosts" -R "192.168.0.184"

# Host 192.168.0.184 found: line 178
/home/tim/.ssh/known_hosts updated.
Original contents retained as /home/tim/.ssh/known_hosts.old
```

### Check the OS
If you're not sure if you have the right OS for the ansible scripts, you can check. For this experiment, you should see something like this

```
$ uname -a
Linux odroid 4.9.218-13 #1 SMP PREEMPT Tue Apr 21 20:02:01 UTC 2020 aarch64 aarch64 aarch64 GNU/Linux
```

### Ansible cannot reach SBC

If you are having trouble connecting via ansible, following the guidance on verbose logging [here](https://docs.ansible.com/ansible/latest/network/user_guide/network_debug_troubleshooting.html)

You might find you've got the format of the /etc/ansible/hosts incorrect (e.g. didn't name the vars section correctly, got the password wrong etc).


### ttyACM0 Device not found 

If you get this error, reboot the SBC and try again. If the error persists, check which device name it has been assigned (`/dev/ttyACM0` is expected for Nano 33 IOT)
```
#rpi
lsusb
dmesg | tail
```

### Error with sudo / sudoers

If you made an error, you might see something like this
```
>>> /etc/sudoers: syntax error near line 26 <<<
What now? 
```
In which case, hit `e` to go back to the editor and correct the mistake.

#### Direct editing error

Editing the sudoers file directly can lock you out of running commands as sudo, if you make an error. Recovering it is [straightforward](https://askubuntu.com/questions/73864/how-to-modify-an-invalid-etc-sudoers-file).

Ali Tou says:
"When this happens to a non-GUI system (your production server, maybe) the pkexec fails with this error message:

```
polkit-agent-helper-1: error response to PolicyKit daemon: GDBus.Error:org.freedesktop.PolicyKit1.Error.Failed: No session for cookie
==== AUTHENTICATION FAILED ===
Error executing command as another user: Not authorized
```
In this situation, using `pkttyagent` can be helpful. If you want to remove a corrupted file in `sudoers.d` directory, use this:

```
pkttyagent -p $(echo $$) | pkexec rm /etc/sudoers.d/FILENAME
```

If you want to recover the default `/etc/sudoers`, you can use [this gist](https://gist.github.com/alitoufighi/679304d9585304075ba1ad93f80cce0e) to copy the default configurations, putting it in a non-root accessed place (e.g. your $HOME). Then, you can overwrite your sudoers file:

```
pkttyagent -p $(echo $$) | pkexec cp ~/sudoers /etc/sudoers
```
NOTE: Using this approach, after running your command, probably your access to the shell will be gone. But I'm sure losing one shell session is much better than losing your server! (According to the manpage, this is the normal behavior: When its services are no longer needed, the process can be killed.)

End of Answer from Ali Tou


In short, create a file `~/sudoers` with the following contents:

```
#
# This file MUST be edited with the 'visudo' command as root.
#
# Please consider adding local content in /etc/sudoers.d/ instead of
# directly modifying this file.
#
# See the man page for details on how to write a sudoers file.
#
Defaults	env_reset
Defaults	mail_badpass
Defaults	secure_path="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin"

# Host alias specification

# User alias specification

# Cmnd alias specification

# User privilege specification
root	ALL=(ALL:ALL) ALL

# Members of the admin group may gain root privileges
%admin ALL=(ALL) ALL

# Allow members of group sudo to execute any command
%sudo	ALL=(ALL:ALL) ALL

# See sudoers(5) for more information on "#include" directives:

#includedir /etc/sudoers.d

```

Then run this command:
```
pkttyagent -p $(echo $$) | pkexec cp ~/sudoers /etc/sudoers
```

It should say
```
==== AUTHENTICATING FOR org.freedesktop.policykit.exec ===
Authentication is needed to run `/bin/cp' as the super user
Authenticating as: ,,, (odroid)
Password: 
==== AUTHENTICATION COMPLETE ===
```

At which point, I used a Ctrl-C to get back to the shell. `sudo` now works as expected:

```
odroid@odroid:~$ sudo su
root@odroid:/home/odroid# 
```


### FFMPEG

FFMPEG dependencies can cause issues, and `apt` is better at resolving them than `apt-get`. Should issues occur in future, consider trying something similar to the trick we use successfully so far: is issuing `sudo apt install libsdl2-2.0-0` first to get the right version of the dependency. Note the use of `apt`, not `apt-get`.  `apt-get` wants to install 2.0.8, while `apt` can install the 2.0.10 that `ffmpeg` needs. 