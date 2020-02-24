/**************************************************************************/
/*!
    @file     BLEHidKbd.cpp
    @author   samveen (samveen.in)

    @section LICENSE

    See LICENSE file for details.
*/
/**************************************************************************/

#include "BLEHidKbd.h"

#ifndef REPORT_ID_KEYBOARD
enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_CONSUMER_CONTROL,
//  REPORT_ID_MOUSE,
//  REPORT_ID_GAMEPAD
};
#endif

uint8_t const hid_report_descriptor[] =
{
  //------------- Keyboard Report  -------------//
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ),
  HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD ),
  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),
    HID_REPORT_ID ( REPORT_ID_KEYBOARD        ),
    HID_USAGE_PAGE( HID_USAGE_PAGE_KEYBOARD ),
      // 8 bits Modifier Keys (Shfit, Control, Alt)
      HID_USAGE_MIN    ( 224                                    ),
      HID_USAGE_MAX    ( 231                                    ),
      HID_LOGICAL_MIN  ( 0                                      ),
      HID_LOGICAL_MAX  ( 1                                      ),

      HID_REPORT_COUNT ( 8                                      ),
      HID_REPORT_SIZE  ( 1                                      ),
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

      // 8 bit reserved
      HID_REPORT_COUNT ( 1                                      ),
      HID_REPORT_SIZE  ( 8                                      ),
      HID_INPUT        ( HID_CONSTANT                           ),

    // 6-byte Keycodes
    HID_USAGE_PAGE (HID_USAGE_PAGE_KEYBOARD),
      HID_USAGE_MIN    ( 0                                   ),
      HID_USAGE_MAX    ( 255                                 ),
      HID_LOGICAL_MIN  ( 0                                   ),
      HID_LOGICAL_MAX  ( 255                                 ),

      HID_REPORT_COUNT ( 6                                   ),
      HID_REPORT_SIZE  ( 8                                   ),
      HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ),

    // LED Indicator Kana | Compose | Scroll Lock | CapsLock | NumLock
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED                   ),
      /* 5-bit Led report */
      HID_USAGE_MIN    ( 1                                       ),
      HID_USAGE_MAX    ( 5                                       ),
      HID_REPORT_COUNT ( 5                                       ),
      HID_REPORT_SIZE  ( 1                                       ),
      HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ),
      /* led padding */
      HID_REPORT_COUNT ( 1                                       ),
      HID_REPORT_SIZE  ( 3                                       ),
      HID_OUTPUT       ( HID_CONSTANT                            ),
  HID_COLLECTION_END,

  //------------- Consumer Control Report -------------//
  HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    ),
  HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL ),
  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),
    HID_REPORT_ID( REPORT_ID_CONSUMER_CONTROL ),
    HID_LOGICAL_MIN  ( 0x00                                ),
    HID_LOGICAL_MAX_N( 0x03FF, 2                           ),
    HID_USAGE_MIN    ( 0x00                                ),
    HID_USAGE_MAX_N  ( 0x03FF, 2                           ),
    HID_REPORT_COUNT ( 1                                   ),
    HID_REPORT_SIZE  ( 16                                  ),
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ),
  HID_COLLECTION_END,

#if 0
  //------------- Mouse Report: buttons + dx + dy + scroll + pan -------------//
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ),
  HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE    ),
  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),
    HID_REPORT_ID( REPORT_ID_MOUSE        ),
    HID_USAGE      (HID_USAGE_DESKTOP_POINTER ),
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL  ),
      HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON ),
        HID_USAGE_MIN    ( 1                                      ),
        HID_USAGE_MAX    ( 5                                      ),
        HID_LOGICAL_MIN  ( 0                                      ),
        HID_LOGICAL_MAX  ( 1                                      ),

        HID_REPORT_COUNT ( 5                                      ), /* Forward, Backward, Middle, Right, Left */
        HID_REPORT_SIZE  ( 1                                      ),
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),

        HID_REPORT_COUNT ( 1                                      ),
        HID_REPORT_SIZE  ( 3                                      ),
        HID_INPUT        ( HID_CONSTANT                           ), /* 5 bit padding followed 3 bit buttons */

      HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP ),
        HID_USAGE        ( HID_USAGE_DESKTOP_X                    ),
        HID_USAGE        ( HID_USAGE_DESKTOP_Y                    ),
        HID_LOGICAL_MIN  ( 0x81                                   ), /* -127 */
        HID_LOGICAL_MAX  ( 0x7f                                   ), /* 127  */

        HID_REPORT_COUNT ( 2                                      ), /* X, Y position */
        HID_REPORT_SIZE  ( 8                                      ),
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), /* relative values */

        HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL                ), /* mouse scroll */
        HID_LOGICAL_MIN ( 0x81                                   ), /* -127 */
        HID_LOGICAL_MAX ( 0x7f                                   ), /* 127  */
        HID_REPORT_COUNT( 1                                      ),
        HID_REPORT_SIZE ( 8                                      ), /* 8-bit value */
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), /* relative values */

      HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER ),
        HID_USAGE_N     ( HID_USAGE_CONSUMER_AC_PAN, 2           ), /* Horizontal wheel scroll */
        HID_LOGICAL_MIN ( 0x81                                   ), /* -127 */
        HID_LOGICAL_MAX ( 0x7f                                   ), /* 127  */
        HID_REPORT_COUNT( 1                                      ),
        HID_REPORT_SIZE ( 8                                      ), /* 8-bit value */
        HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), /* relative values */
    HID_COLLECTION_END,
  HID_COLLECTION_END,

  //------------- Gamepad Report -------------//
  /* Byte 0: 4 pad | 2 Y-axis | 2 X-axis
   * Byte 1: Button7-Button0
   */
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     ),
  HID_USAGE      ( HID_USAGE_DESKTOP_GAMEPAD  ),
  HID_COLLECTION ( HID_COLLECTION_APPLICATION ),
    HID_REPORT_ID ( REPORT_ID_GAMEPAD      ),
    HID_USAGE      (HID_USAGE_DESKTOP_POINTER ),
    HID_COLLECTION ( HID_COLLECTION_PHYSICAL  ),
      // X,Y joystick
      HID_USAGE    ( HID_USAGE_DESKTOP_X                    ),
      HID_USAGE    ( HID_USAGE_DESKTOP_Y                    ),
      HID_LOGICAL_MIN ( 0xFF                                   ), /* -1 */
      HID_LOGICAL_MAX ( 0x01                                   ), /* 1  */
      HID_REPORT_COUNT( 2                                      ), /* X, Y position */
      HID_REPORT_SIZE ( 2                                      ), /* 2-bit value */
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), /* input values */
    HID_COLLECTION_END,

    /* X,Y padding */
    HID_REPORT_COUNT ( 4                                       ),
    HID_REPORT_SIZE  ( 1                                       ),
    HID_INPUT        ( HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE),

    // Buttons
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON ),
      HID_USAGE_MIN    ( 1                                      ),
      HID_USAGE_MAX    ( 8                                      ),
      HID_LOGICAL_MIN  ( 0                                      ),
      HID_LOGICAL_MAX  ( 1                                      ),
      HID_REPORT_COUNT ( 8                                      ),    // Keyboard
      HID_REPORT_SIZE  ( 1                                      ),
      HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
  HID_COLLECTION_END
#endif
};

BLEHidKbd::BLEHidKbd(void)
  : BLEHidGeneric(3, 1, 0)
{
  _mse_buttons = 0;
  _kbd_led_cb = NULL;
}

err_t BLEHidKbd::begin(void)
{
  uint16_t input_len [] = { sizeof(hid_keyboard_report_t),  sizeof(hid_consumer_control_report_t), sizeof(hid_mouse_report_t) };
  uint16_t output_len[] = { 1 };

  setReportLen(input_len, output_len, NULL);
  enableKeyboard(true);
  enableMouse(false);
  setReportMap(hid_report_descriptor, sizeof(hid_report_descriptor));

  VERIFY_STATUS( BLEHidGeneric::begin() );

  // Attempt to change the connection interval to 11.25-15 ms when starting HID
  Bluefruit.Periph.setConnInterval(9, 12);

  return ERROR_NONE;
}

/*------------------------------------------------------------------*/
/* Keyboard Multiple Connections
 *------------------------------------------------------------------*/
void BLEHidKbd::blehid_kbd_keyboard_output_cb(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  LOG_LV2("HID", "Keyboard LED : 0x%02X", data[0]);

  BLEHidKbd& svc = (BLEHidKbd&) chr->parentService();
  if ( svc._kbd_led_cb ) svc._kbd_led_cb(conn_hdl, data[0]);
}

void BLEHidKbd::setKeyboardLedCallback(kbd_led_cb_t fp)
{
  _kbd_led_cb = fp;

  // Report mode
  this->setOutputReportCallback(REPORT_ID_KEYBOARD, fp ? blehid_kbd_keyboard_output_cb : NULL);

  // Boot mode
  _chr_boot_keyboard_output->setWriteCallback(fp ? blehid_kbd_keyboard_output_cb : NULL);
}

bool BLEHidKbd::keyboardReport(uint16_t conn_hdl, hid_keyboard_report_t* report)
{
  if ( isBootMode() )
  {
    return bootKeyboardReport(conn_hdl, report, sizeof(hid_keyboard_report_t));
  }else
  {
    return inputReport(conn_hdl, REPORT_ID_KEYBOARD, report, sizeof(hid_keyboard_report_t));
  }
}

bool BLEHidKbd::keyboardReport(uint16_t conn_hdl, uint8_t modifier, uint8_t keycode[6])
{
  hid_keyboard_report_t report =
  {
      .modifier = modifier,
  };
  memcpy(report.keycode, keycode, 6);

  return keyboardReport(conn_hdl, &report);
}

bool BLEHidKbd::keyPress(uint16_t conn_hdl, char ch)
{
  hid_keyboard_report_t report;
  varclr(&report);

  report.modifier = ( hid_ascii_to_keycode[(uint8_t)ch][0] ) ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
  report.keycode[0] = hid_ascii_to_keycode[(uint8_t)ch][1];

  return keyboardReport(conn_hdl, &report);
}

bool BLEHidKbd::keyRelease(uint16_t conn_hdl)
{
  hid_keyboard_report_t report;
  varclr(&report);

  return keyboardReport(conn_hdl, &report);
}

bool BLEHidKbd::keySequence(uint16_t conn_hdl, const char* str, int interval)
{
  // Send each key in sequence
  char ch;
  while( (ch = *str++) != 0 )
  {
    char lookahead = *str;

    keyPress(conn_hdl, ch);
    delay(interval);

    /* Only need to empty report if the next character is NULL or the same with
     * the current one, else no need to send */
    if ( lookahead == ch || lookahead == 0 )
    {
      keyRelease(conn_hdl);
      delay(interval);
    }
  }

  return true;
}

/*------------------------------------------------------------------*/
/* Consumer Media Key
 *------------------------------------------------------------------*/
bool BLEHidKbd::consumerReport(uint16_t conn_hdl, uint16_t usage_code)
{
  return inputReport(conn_hdl, REPORT_ID_CONSUMER_CONTROL, &usage_code, sizeof(usage_code));
}

bool BLEHidKbd::consumerKeyPress(uint16_t conn_hdl, uint16_t usage_code)
{
  return consumerReport(conn_hdl, usage_code);
}

bool BLEHidKbd::consumerKeyRelease(uint16_t conn_hdl)
{
  uint16_t usage = 0;
  return consumerReport(conn_hdl, usage);
}

/*------------------------------------------------------------------*/
/* Single Connections
 *------------------------------------------------------------------*/

//------------- Keyboard -------------//
bool BLEHidKbd::keyboardReport(hid_keyboard_report_t* report)
{
  return keyboardReport(BLE_CONN_HANDLE_INVALID, report);
}

bool BLEHidKbd::keyboardReport(uint8_t modifier, uint8_t keycode[6])
{
  return keyboardReport(BLE_CONN_HANDLE_INVALID, modifier, keycode);
}

bool BLEHidKbd::keyPress(char ch)
{
  return keyPress(BLE_CONN_HANDLE_INVALID, ch);
}

bool BLEHidKbd::keyRelease(void)
{
  return keyRelease(BLE_CONN_HANDLE_INVALID);
}

bool BLEHidKbd::keySequence(const char* str, int interval)
{
  return keySequence(BLE_CONN_HANDLE_INVALID, str, interval);
}

//------------- Consumer Media Keys -------------//
bool BLEHidKbd::consumerReport(uint16_t usage_code)
{
  return consumerReport(BLE_CONN_HANDLE_INVALID, usage_code);
}

bool BLEHidKbd::consumerKeyPress(uint16_t usage_code)
{
  return consumerKeyPress(BLE_CONN_HANDLE_INVALID, usage_code);
}

bool BLEHidKbd::consumerKeyRelease(void)
{
  return consumerKeyRelease(BLE_CONN_HANDLE_INVALID);
}
