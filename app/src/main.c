/*
 * main.c
 */

 /* IMPORTS -------------------------------------------------------------------------------------- */

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>

#include "BTN.h"
#include "LED.h"

/* MACROS --------------------------------------------------------------------------------------- */
#define SLEEP_MS 1

static struct bt_uuid_128 BLE_CUSTOM_SERVICE_UUID =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x11111111, 0x1111, 0x1111, 0x1111, 0x111111111111));
static struct bt_uuid_128 BLE_CUSTOM_CHARACTERISTIC_UUID =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x11111111, 0x1111, 0x1111, 0x1111, 0x111111111112));


/* PROTOTYPES ----------------------------------------------------------------------------------- */

static void ble_start_scanning(void);

/* VARIABLES  ----------------------------------------------------------------------------------- */

static struct bt_conn* my_connection;

/* FUNCTIONS ------------------------------------------------------------------------------------ */

static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data) {
  char* name = user_data;

  if (data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED) {
    memcpy(name, data->data, data->data_len); // copy name to the  user data buffer
    name[data->data_len] = '\0'; // terminate the string
    return false; // stop parsing after finding the name
  }

  return true; // continue parsing
}


static void ble_on_advertisement_received(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
                                          struct net_buf_simple* ad) {
  
  char addr_str[BT_ADDR_LE_STR_LEN];
  int err;

  if(my_connection) {
    return;
  }

  if(type != BT_GAP_ADV_TYPE_ADV_IND && type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
    return;
  }

  char name[32] = {0};
  bt_data_parse(ad, ble_get_adv_device_name_cb, name);
  bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
  printk("Device found: %s (RSSI %d) - '%s'\n", addr_str, rssi, name);

  if(rssi < -50) {
    return;
  }
  if(memcmp(name, "My_BLE_Device", sizeof("My_BLE_Device")) != 0) {
    return;
  }

  if (bt_le_scan_stop()) {
    return;
  }

  err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &my_connection);
  if (err) {
    printk("Create conn to %s failed (%d)\n", addr_str, err);
    ble_start_scanning();
  }
}

static void ble_start_scanning(void) {
  int err;

  /* This demo doesn't require active scan */
  err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, ble_on_advertisement_received);
  if (err) {
    printk("Scanning failed to start (err %d)\n", err);
    return;
  }

  printk("Scanning successfully started\n");
}

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  if (err) {
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    bt_conn_unref(conn);
    my_connection = NULL;
    ble_start_scanning();
    return;
  }
  if (conn != my_connection) {
    return;
  }

  printk("Connected: %s\n", addr);
}

static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
  char addr[BT_ADDR_LE_STR_LEN];
  if(conn != my_connection)
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  printk("Disconnected: %s\n", addr);
  printk("Disconnected reason: %s\n", bt_hci_err_to_str(reason));
  bt_conn_unref(conn);
  my_connection = NULL;
}

/* CALLBACK CONFIG ------------------------------------------------------------------------------ */

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = ble_on_device_connected,
    .disconnected = ble_on_device_disconnected,
};

int main(void) {

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  int err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return 0;
  } else {
    printk("Bluetooth initialized\n");
  }

  ble_start_scanning();
  int counter = 0;
  while(1) {
    k_msleep(SLEEP_MS);
    }
  return 0;
}
