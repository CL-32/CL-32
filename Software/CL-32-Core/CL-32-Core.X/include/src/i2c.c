#include "../i2c.h"
#include "../power.h"
#include "../keyb.h"
#include "../general.h"
#include <util/delay.h>

volatile uint8_t i2c_regs[30];
volatile uint8_t reg_index = 0;
volatile uint8_t addr_phase = 1;
volatile uint8_t isAddressByte = 0;
volatile uint8_t seto = 0;
volatile uint8_t reseto = 0;
volatile uint8_t reflag = 0;

void i2c_slave_init(uint8_t address)
{
    // Route TWI0 to PC2/PC3
    PORTMUX.TWIROUTEA = PORTMUX_TWI0_ALT2_gc;

    // Slave address
    TWI0.SADDR = (address << 1);
    PORTC_DIRCLR = PIN1_bm;//set as input
    
    // Enable slave mode interrupts
    TWI0.SSTATUS = TWI_APIF_bm | TWI_DIF_bm | TWI_COLL_bm | TWI_BUSERR_bm;
    TWI0.SCTRLA = TWI_ENABLE_bm | TWI_APIEN_bm | TWI_DIEN_bm | TWI_PIEN_bm | TWI_SMEN_bm;
}

void set_i2c_flag(void){  
    //set the interrupt pin to be an output, and pull it low, to flag there being data
    //check if this was a missfire and the interrupt flag is actuallly empty
    if(i2c_regs[CL32_INTERRUPT]==0){
        PORTC_DIRCLR = PIN1_bm;//set as input
        reseto++;
    }
    else{
        //lets check if the pin is already up, that might be a stale flag
        if (VPORTC.IN & PIN1_bm){
            PORTC_DIRCLR = PIN1_bm;//reset the flag
            _delay_ms(100);//give it chance to be seen
            reflag++;
        }
        PORTC_DIRSET = PIN1_bm;//set as output
        PORTC_OUTCLR = PIN1_bm;//set low
        seto++;
    }
}

void clear_i2c_flag(void){
    //set the pin as input si the pull-up resistor can set it high, so we know there is no data
    PORTC_DIRCLR = PIN1_bm;//set as input
    reseto++;
}

ISR(TWI0_TWIS_vect){
    uint8_t status = TWI0.SSTATUS;

    // =========================
    // Error handling
    // =========================
    if (status & (TWI_COLL_bm | TWI_BUSERR_bm))
    {
        TWI0.SSTATUS = TWI_COLL_bm | TWI_BUSERR_bm;
        return;
    }

    // =========================
    // Address / STOP
    // =========================
    if (status & TWI_APIF_bm)
    {
        if (status & TWI_AP_bm)
        {
            // Consume address byte (MCC does this)
            volatile uint8_t dummy = TWI0.SDATA;
            (void)dummy;

            isAddressByte = 1;
            addr_phase = 1;

            // If master immediately wants to read,
            // preload first byte
            if (status & TWI_DIR_bm)
            {
//                if (reg_index == CL32_EVENT1)
//                {
//                    TWI0.SDATA = fifo_pop();
//                }
//                else
//                {
                   // TWI0.SDATA = i2c_regs[reg_index];
//                }
            }

            TWI0.SCTRLB =
                TWI_ACKACT_ACK_gc |
                TWI_SCMD_RESPONSE_gc;
        }
        else
        {
            // STOP condition
            TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
        }

        TWI0.SSTATUS = TWI_APIF_bm;
        return;
    }

    // =========================
    // Data phase
    // =========================
    if (status & TWI_DIF_bm)
    {
        // -------------------------
        // Master writing to us
        // -------------------------
        if (!(status & TWI_DIR_bm))
        {
            uint8_t data = TWI0.SDATA;

            if (addr_phase)
            {
                reg_index = data;
                addr_phase = 0;
            }
            else
            {
                i2c_regs[reg_index] = data;
                if(reg_index==CL32_INTERRUPT){
                    set_i2c_flag();//this will also clear if the interrupt is 0
                }
            }

            TWI0.SCTRLB =
                TWI_ACKACT_ACK_gc |
                TWI_SCMD_RESPONSE_gc;
        }

        // -------------------------
        // Master reading from us
        // -------------------------
        else
        {
            /*
             * Ignore RXACK on the first DIF after
             * the address phase.
             */
            if (!isAddressByte &&
                (status & TWI_RXACK_bm))
            {
                TWI0.SCTRLB =
                    TWI_SCMD_COMPTRANS_gc;
            }
            else
            {
                // After first transmit
                isAddressByte = 0;

                if (reg_index == CL32_EVENT1)
                {
                    TWI0.SDATA = fifo_pop();
                }
                else
                {
                    TWI0.SDATA = i2c_regs[reg_index++];
                }

                TWI0.SCTRLB =
                    TWI_ACKACT_ACK_gc |
                    TWI_SCMD_RESPONSE_gc;
            }
        }

        TWI0.SSTATUS = TWI_DIF_bm;
    }

    if (reg_index >= sizeof(i2c_regs))
        reg_index = 0;

}
    