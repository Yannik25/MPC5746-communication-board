#include <stdint.h>
#include <mdio.h>
#include "device_registers.h"
#include "pin_mux.h"

#define REGCR   0xd
#define ADDAR   0xe

#define MDCPORT PTG
#define MDCPIN  0

#define MDIOPORT PTF
#define MDIOPIN  14

#define NUM_OF_CONFIGURED_PIN 1

pin_settings_config_t g_pin_mux_mdioinArr[NUM_OF_CONFIGURED_PINS] =
{
	{
		.base              = SIUL2,
		.pinPortIdx        = 94u,
		.mux               = PORT_MUX_AS_GPIO,
		.outputBuffer      = PORT_OUTPUT_BUFFER_DISABLED,
		.openDrain         = PORT_OPEN_DRAIN_DISABLED,
		.slewRateCtrlSel   = HALF_STRENGTH_WITH_SLEWRATE_CONTROL,
		.hysteresisSelect  = PORT_HYSTERESYS_DISABLED,
		.safeMode          = PORT_SAFE_MODE_DISABLED,
		.analogPadCtrlSel  = PORT_ANALOG_PAD_CONTROL_DISABLED,
		.inputBuffer       = PORT_INPUT_BUFFER_ENABLED,
		.pullConfig        = PORT_INTERNAL_PULL_NOT_ENABLED,
		.gpioBase          = PTF,
		.intConfig         =
		{
			.eirqPinIdx    = SIUL2_INT_UNSUPPORTED,
		},
		.inputMux[0]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[1]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[2]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[3]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[4]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[5]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[6]       = PORT_INPUT_MUX_NO_INIT,
		.inputMux[7]       = PORT_INPUT_MUX_NO_INIT,
	},
};

pin_settings_config_t g_pin_mux_mdiooutArr[NUM_OF_CONFIGURED_PINS] =
{
    {
		.base              = SIUL2,
		.pinPortIdx        = 94u,
		.mux               = PORT_MUX_AS_GPIO,
		.outputBuffer      = PORT_OUTPUT_BUFFER_ENABLED,
		.openDrain         = PORT_OPEN_DRAIN_DISABLED,
		.slewRateCtrlSel   = HALF_STRENGTH_WITH_SLEWRATE_CONTROL,
		.hysteresisSelect  = PORT_HYSTERESYS_DISABLED,
		.safeMode          = PORT_SAFE_MODE_DISABLED,
		.analogPadCtrlSel  = PORT_ANALOG_PAD_CONTROL_DISABLED,
		.inputBuffer       = PORT_INPUT_BUFFER_DISABLED,
		.pullConfig        = PORT_INTERNAL_PULL_NOT_ENABLED,
		.gpioBase          = PTF,
		.initValue         = 0u,
    },
};



static inline void mdc_low(){
    PINS_DRV_ClearPins(MDCPORT, (1 << MDCPIN));
}

static inline void mdc_high(){
    PINS_DRV_SetPins(MDCPORT, (1 << MDCPIN));
}

static inline void mdio_low(){
    PINS_DRV_ClearPins(MDIOPORT, (1 << MDIOPIN));
}

static inline void mdio_high(){
    PINS_DRV_SetPins(MDIOPORT, (1 << MDIOPIN));
}

static inline void mdio_in(){
    PINS_DRV_Init(NUM_OF_CONFIGURED_PIN, g_pin_mux_mdioinArr);
}

static inline void mdio_out(){
	PINS_DRV_Init(NUM_OF_CONFIGURED_PIN, g_pin_mux_mdiooutArr);
}

static inline int mdio_value(){
	return (PINS_DRV_ReadPins(MDIOPORT)&0x4000) ? 1 : 0;
}

static void write_mdio(uint16_t data, uint16_t len){
    int16_t i;
    len--;
    for(i=len; i>=0; i--){
        if(data & (1<<i)) mdio_high();
        else mdio_low();
        mdc_high();
        mdc_low();
    }
}

static uint16_t read_mdio(void){
    int16_t timeout = 32;
    int16_t i;
    uint16_t data = 0;
    mdio_in();
    while(mdio_value() && (timeout--)){
        mdc_high();
        mdc_low();
    }

    if(timeout < 0) return 0;

    for(i=15; i>=0; i--){
        mdc_high();
        data |= (mdio_value()<<i);
        mdc_low();
    }
    mdio_out();
	mdc_high();
	mdc_low();
    return data;
}

static void write_register_standard(uint16_t phyid, uint16_t reg, uint16_t data){
	mdio_out();
    write_mdio(0xffff, 16); // begin communication
    write_mdio(0xffff, 16); // begin communication
    write_mdio((0x1<<14) | (0x1<<12) | (phyid<<7) | (reg<<2) | 0x2, 16); // start, write, phyid, reg, ta
    write_mdio(data, 16);
}

static uint16_t read_register_standard(uint16_t phyid, uint16_t reg){
	mdio_out();
    write_mdio(0xffff, 16); // begin communication
    write_mdio(0xffff, 16); // begin communication
    write_mdio((0x1<<12) | (0x2<<10) | (phyid<<5) | reg, 14); // start, read, phyid, reg, ta
    return read_mdio();
}

uint16_t read_register(uint16_t phyid, uint16_t reg){
    uint16_t mmdfield;
    if(reg <= 0x1f){
        return read_register_standard(phyid, reg);
    }
    if((reg >= 0x20) && (reg <= 0xfff)){
        mmdfield = 0x1F;
        return read_registermmd(phyid, mmdfield, reg);
    }
    else{
        mmdfield = 0x01;
        return read_registermmd(phyid, mmdfield, reg);
    }
}

uint16_t read_registermmd(uint16_t phyid, uint16_t mmdfield, uint16_t reg){
    uint16_t regmmd;
    if(reg <= 0x1f){
        return read_register_standard(phyid, reg);
    }
    else{
        regmmd = 0x4000 + mmdfield;
        write_register_standard(phyid, REGCR, mmdfield);
        write_register_standard(phyid, ADDAR, reg);
        write_register_standard(phyid, REGCR, regmmd);
        return read_register_standard(phyid, ADDAR);
    }
}

void write_register(uint16_t phyid, uint16_t reg, uint16_t data){
    uint16_t mmdfield;
    if(reg <= 0x1f){
        return write_register_standard(phyid, reg, data);
    }
    if((reg >= 0x20) && (reg <= 0xfff)){
        mmdfield = 0x1F;
        return write_registermmd(phyid, mmdfield, reg, data);
    }
    else{
        mmdfield=0x01;
        return write_registermmd(phyid, mmdfield, reg, data);
    }
}

void write_registermmd(uint16_t phyid, uint16_t mmdfield, uint16_t reg, uint16_t data){
    uint16_t regmmd;
    if((reg <= 0x1f)&&(mmdfield == 0x1f)){
        write_register_standard(phyid, reg, data);
    }
    else{
        regmmd = 0x4000 + mmdfield;
        write_register_standard(phyid, REGCR, mmdfield);
        write_register_standard(phyid, ADDAR, reg);
        write_register_standard(phyid, REGCR, regmmd);
        write_register_standard(phyid, ADDAR, data);
    }
}

void init_mdio(void){
	mdio_out();
}
