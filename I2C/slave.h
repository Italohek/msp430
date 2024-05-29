/*
 * slave.h
 *
 *  Created on: 28 de mai de 2024
 *      Author: UNB
 */

#ifndef SLAVE_H_
#define SLAVE_H_

#define MAX_B1 10 // Define o tamanho máximo do vetor para recepção

void USCI_B1_config(void);
void USCI_B1_rx(void);
void USCI_B1_tx(void);

// Variáveis globais
volatile char b1_vet[MAX_B1]; // Vetor para receber os dados
volatile char b1; // Indexador para o vetor
volatile char b1_cont; // Contador para quando for transmitir dados


#endif /* SLAVE_H_ */
