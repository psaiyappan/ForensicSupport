# ForensicSupport
1. APK starts a service, that collect all forensic events inlcuding file-syetem events. 

2. APK uses inotify-tools NDK code for collecting file-syetem events.

3. The phone events are collected by using Broadcast reciever and stores to log.txt file in /sdcard/forensic/ directory.
