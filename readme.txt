ASOCKET
    simple async socket for ipv4 (listening to localhost setting
    a port) or sock (ie, mysock.sock, only linux)

NOTES
    - no heap memory is allocated.
    - only listens to localhost.
    - listening on sock partial support, just for linux at the moment.
    - windows was lazily made, it could be incorrect.

INSTALLATION
    - copy asocket.h and asocket_windows.c or asocket_linux.c to your project.
    - check build.sh for linux and build.bat for windows.

HOW TO USE IT
    check main.c for a simple http server example.
    use the stress.sh script to test the server under
    heavy load.