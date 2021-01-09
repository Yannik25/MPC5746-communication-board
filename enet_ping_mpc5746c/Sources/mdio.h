/*
 * mdio.h
 *
 *  Created on: Dec 16, 2020
 *      Author: a0484903
 */

#ifndef MDIO_H_
#define MDIO_H_

void init_mdio(void);
uint16_t read_register(uint16_t phyid, uint16_t reg);
void write_register(uint16_t phyid, uint16_t reg, uint16_t data);
uint16_t read_registermmd(uint16_t phyid, uint16_t mmdfield, uint16_t reg);
void write_registermmd(uint16_t phyid, uint16_t mmdfield, uint16_t reg, uint16_t data);


#endif /* MDIO_H_ */
