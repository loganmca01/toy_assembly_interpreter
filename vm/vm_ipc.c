#include "vm.h"

#define RECEIVER_NAME ("../vm_receiver")
#define TRANSMITTER_NAME ("../vm_transmitter")

int receiver_fd;
int transmitter_fd;

int make_pipes() {

    mkfifo(RECEIVER_NAME, 0666);
    mkfifo(TRANSMITTER_NAME, 0666);

    receiver_fd = open(RECEIVER_NAME, O_RDONLY);
    transmitter_fd = open(TRANSMITTER_NAME, O_WRONLY);

    if (receiver_fd == -1 || transmitter_fd == -1) return 1;

    



}