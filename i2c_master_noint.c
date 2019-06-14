#include<xc.h>
#include "i2c_master_noint.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
  I2C2BRG = 53;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {       // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void write_i2c(unsigned char add, unsigned char reg, unsigned char val){
    i2c_master_start();                      //begin the start sequence
    i2c_master_send(add << 1|0);       // send the slave address of the register, left shifted by 1, clearing last bit and setting it to 0 indicating to write
    i2c_master_send(reg);                    //send the adress
    i2c_master_send(val);                    //send the bits
    i2c_master_stop();                       //stop sequence
    
}

unsigned char read_i2c(unsigned char add, unsigned char reg){
    i2c_master_start();                      //begin the start sequence
    i2c_master_send(add << 1|0);       // send the slave address, left shifted by 1, clearing last bit and setting it to 0 indicating to write
    i2c_master_send(reg);                   //send the address of the general purpose i/o register
    i2c_master_restart();                    //restart
    i2c_master_send(add << 1|1);       // send the slave address, left shifted by 1, clearing last bit and setting it to 1 indicating to write
    unsigned char r = i2c_master_recv();     //save the output of the slave
    i2c_master_ack(1);                       //done talking to the chip (0 if not sone but just recieved))
    i2c_master_stop();                      //stop sequence
    return r;                               //return the value read by the chip
}

unsigned char read_multiple_i2c(unsigned char add, unsigned char reg1, unsigned char* outs, unsigned int length){
    i2c_master_start();                      //begin the start sequence
    i2c_master_send(add << 1|0);       // send the slave address, left shifted by 1, clearing last bit and setting it to 0 indicating to write
    i2c_master_send(reg1);                   //send the address of the general purpose i/o register
    i2c_master_restart();                    //restart
    i2c_master_send(add << 1|1);       // send the slave address, left shifted by 1, clearing last bit and setting it to 1 indicating to write
    unsigned int i=0;
    for (i=0; i<=length; i++) {               //loop over all reading registers
    outs[i] = i2c_master_recv();             //save the output of the slave
    if(i==length){                           //when the loop reaches the last value
    i2c_master_ack(1);                       //not done talking to the chip 
    }
    else {                                   //for every other value read out
    i2c_master_ack(0);                       //done talking to the chip  
    }
    }
    i2c_master_stop();                      //stop sequence
}
    
//initialize LSM
void initExp (){
    ANSELBbits.ANSB2 = 0; //make B2 output digital
    ANSELBbits.ANSB3 = 0; //make B3 output digital
    
    i2c_master_setup();
    write_i2c(0b1101011,0x12,0b00000100); //communicate with crl3_c and set ifing (6th bit) to 1, which allows to read multiple registers in a row
    write_i2c(0b1101011,0x10,0b10000010); //communicate with ctrl1_xl and set the sample rate to 1.66 kHz (first four bits 1000), with 2g sensitivity (fifth and sixth bits to 00), and 100 Hz filter (last two bits to 10).
    write_i2c(0b1101011,0x11,0b10001100); //communicate with ctrl2_g and set the sample rate to 1.66 kHz (first four bits 1000), with with 1000 dps sensitivity (5th and 6th bits to 11)
    
}