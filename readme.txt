ASYNC
  simple async socket for ipv4 (listening to localhost setting
  a port) or sock (ie, mysock.sock) intended to be used
  only in linux.

NOTES
  - no heap memory is allocated.
  - only listens to localhost.
  - it's only intended to be used in linux.

INSTALLATION
  copy asocket.h/c to your project.

HOW TO USE IT
  check main.c for a simple http server example.
  use the stress.sh script to test the server under
  heavy load.