#include "note_c_hooks.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(notecard, CONFIG_NOTECARD_LOG_LEVEL);

#ifdef CONFIG_I2C

#include <zephyr/drivers/i2c.h>

const struct device *i2c_dev = NULL;
bool i2c_initialized = false;

static const size_t REQUEST_HEADER_SIZE = 2;

const char *note_i2c_receive(uint16_t device_address_, uint8_t *buffer_, uint16_t size_,
			     uint32_t *available_)
{
	// Let the Notecard know that we are getting ready to read some data
	uint8_t size_buf[2];
	size_buf[0] = 0;
	size_buf[1] = (uint8_t)size_;
	uint8_t write_result = i2c_write(i2c_dev, size_buf, sizeof(size_buf), device_address_);

	if (write_result != 0) {
		return "i2c: Unable to initate read from the Notecard\n";
	}

	// Read from the Notecard and copy the response bytes into the response buffer
	const int request_length = size_ + REQUEST_HEADER_SIZE;
	uint8_t read_buf[256];
	uint8_t read_result = i2c_read(i2c_dev, read_buf, request_length, device_address_);

	if (read_result != 0) {
		return "i2c: Unable to receive data from the Notecard.\n";
	} else {
		*available_ = (uint32_t)read_buf[0];
		uint8_t bytes_to_read = read_buf[1];
		for (size_t i = 0; i < bytes_to_read; i++) {
			buffer_[i] = read_buf[i + 2];
		}

		return NULL;
	}
}

bool note_i2c_reset(uint16_t device_address_)
{
	(void)device_address_;

	if (i2c_initialized) {
		return true;
	}

	if (!i2c_dev) {
		i2c_dev = DEVICE_DT_GET(DT_ALIAS(notecard));
	}

	if (!device_is_ready(i2c_dev)) {
		LOG_ERR("i2c: Device is not ready.\n");
		return false;
	}

	LOG_DBG("i2c: Device is ready.\n");

	i2c_initialized = true;
	return true;
}

const char *note_i2c_transmit(uint16_t device_address_, uint8_t *buffer_, uint16_t size_)
{
	// Create a buffer that contains the number of bytes and the data to write to the Notecard
	uint8_t write_buf[size_ + 1];
	write_buf[0] = (uint8_t)size_;
	for (size_t i = 0; i < size_; i++) {
		write_buf[i + 1] = buffer_[i];
	}

	// Write the message
	uint8_t write_result = i2c_write(i2c_dev, write_buf, sizeof(write_buf), device_address_);

	if (write_result != 0) {
		return "i2c: Unable to transmit data to the Notecard\n";
	} else {
		return NULL;
	}
}

#elif CONFIG_SERIAL

#include <zephyr/drivers/uart.h> 
#define MSG_SIZE 128

const struct device *serial_dev = NULL;
bool serial_initialized = false;
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;
static int serial_buf_pos;

void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(serial_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(serial_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(serial_dev, &c, 1) == 1) {
		if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
            rx_buf_pos++;
			rx_buf[rx_buf_pos] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

bool note_serial_reset() {
	if (serial_initialized) {
		return true;
	}

	if (!serial_dev) {
		serial_dev = DEVICE_DT_GET(DT_ALIAS(notecard));
	}

	if (!device_is_ready(serial_dev)) {
		LOG_ERR("serial: Device is not ready.\n");
		return false;
	}

    int ret = uart_irq_callback_user_data_set(serial_dev, serial_cb, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			LOG_ERR("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			LOG_ERR("UART device does not support interrupt-driven API\n");
		} else {
			LOG_ERR("Error setting UART callback: %d\n", ret);
		}
		return false;
	}

    rx_buf_pos = 0;
    serial_buf_pos = 1;

	uart_irq_rx_enable(serial_dev);
	LOG_DBG("serial: Device is ready.\n");

	serial_initialized = true;
	return true;
}

void note_serial_transmit(uint8_t *text, size_t len, bool flush){
	for (int i = 0; i < len; i++) {
		uart_poll_out(serial_dev, text[i]);
	}
}

bool note_serial_available(){
    if(rx_buf_pos > 0) return true;
    else return false;
}

char note_serial_receive(){
    char serial_buffer = rx_buf[serial_buf_pos];

    if(serial_buf_pos >= rx_buf_pos) {
        rx_buf_pos = 0;
        serial_buf_pos = 1;
    } 
    else serial_buf_pos++;
    return serial_buffer;
}

#else
#error "No transport method defined, enable CONFIG_I2C or CONFIG_SERIAL."
#endif

uint32_t platform_millis(void)
{
	return (uint32_t)k_uptime_get();
}

void platform_delay(uint32_t ms)
{
	k_msleep(ms);
}

size_t note_log_print(const char *message_)
{
	if (message_) {
		LOG_INF("%s", message_);
		return 1;
	}

	return 0;
}
