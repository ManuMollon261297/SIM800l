/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*! @name PORTC16 (number 90), J1[2]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPINS_TMR_1588_0_PORT PORTC                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINS_TMR_1588_0_PIN 16U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_TMR_1588_0_PIN_MASK (1U << 16U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*! @name PORTC17 (number 91), J1[4]
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_INITPINS_TMR_1588_1_PORT PORTC                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITPINS_TMR_1588_1_PIN 17U                   /*!<@brief PORT pin number */
#define BOARD_INITPINS_TMR_1588_1_PIN_MASK (1U << 17U)      /*!<@brief PORT pin mask */
                                                            /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/