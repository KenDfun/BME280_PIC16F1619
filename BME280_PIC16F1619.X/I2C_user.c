#include <stdio.h>
#include "xc.h"
#include "./I2C_user.h"


void I2C_UserInit(void)
{
    SSPCLKPPS = 0x0E;   //RB6->MSSP:SCL;
    SSPDATPPS = 0x0C;   //RB4->MSSP:SDA;
    RB6PPS = 0x10;   //RB6->MSSP:SCL;
    RB4PPS = 0x11;   //RB4->MSSP:SDA;

    SSP1STAT = 0x80; // standerd
    SSP1CON1 = 0x28;
    SSP1CON2 = 0x00;
    SSP1ADD = 0x4f;
    
    SSP1CON1bits.SSPEN =  1;
}


int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
 
    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */
 
    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
    
    
    PIR1bits.SSP1IF = 0; // clear flag
    
    // set start condition
    SSP1CON2bits.SEN = 1;
    I2C_WAIT_SSPIF();
    
    // send device address
    I2C_UserWriteByte(dev_id<<1); // write
    
    // send data
    I2C_UserWriteByte(reg_addr);
    
    // set repeat start condition
    SSP1CON2bits.RSEN = 1;
    I2C_WAIT_SSPIF();
    
    //send device address
    I2C_UserWriteByte((dev_id<<1)|0x01); // read
    
    // receive data
    for(int i=0;i<len;i++){
        if(i==(len-1)){
            reg_data[i]=I2C_UserReadByte(1);  // send nack
        }
        else{
            reg_data[i]=I2C_UserReadByte(0);  // send ack
        }
    }
    
    // set stop condition
    I2C_UserStopCondition();
     
    return rslt;
}
 
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
 
    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */
 
    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    PIR1bits.SSP1IF = 0; // clear flag
    
    // set start condition
    SSP1CON2bits.SEN = 1;
    I2C_WAIT_SSPIF();
    
    // send device address
    I2C_UserWriteByte(dev_id<<1); // write
    
    // send data
    I2C_UserWriteByte(reg_addr);

    for(int i=0;i<len;i++){
        I2C_UserWriteByte(reg_data[i]);
    }
    
        // set stop condition
    I2C_UserStopCondition();
    
    return rslt;
}



void I2C_UserWriteByte(uint8_t data)
{
    SSP1BUF = data; // write data
    I2C_WAIT_SSPIF();
    
    // ACK check
    if(SSP1CON2bits.ACKSTAT!=0){
        I2C_UserAlert(I2C_ERROR_WRITE_NO_ACK); // NACK
    }
    
    return;
}

uint8_t I2C_UserReadByte(uint8_t ackbit)
{
    uint8_t rcvdata;
    
    SSP1CON2bits.RCEN = 1; // Receive enable
    I2C_WAIT_SSPIF();
    
    rcvdata = SSP1BUF;
    
    // send ack or nak
    SSP1CON2bits.ACKDT=ackbit;
    SSP1CON2bits.ACKEN = 1;
    I2C_WAIT_SSPIF();    
    
    return (rcvdata);
}

void I2C_UserStopCondition(void)
{
    SSP1CON2bits.PEN = 1;
    I2C_WAIT_SSPIF();
}

void debug_error(void);
void I2C_UserAlert(I2C_ERROR status)
{
    I2C_UserStopCondition();
    printf("I2C ERROR!!! : %d\r\n",status);
    debug_error();

}

