#include "RasPiHardwareSPI.h"
#include <stdexcept>

namespace acan2517fd
{

    RasPiHardwareSPI::RasPiHardwareSPI(uint8_t spi_channel, uint32_t spi_hz, uint8_t cs_pin)
        : spi_channel_(spi_channel), spi_handle_(-1), cs_pin_(cs_pin), spi_hz_(spi_hz), configuration_mode_(true)
    {
        // GPIO初期化
        if (gpioInitialise() < 0)
        {
            throw std::runtime_error("Failed to initialize GPIO");
        }

        // CSピンを出力に設定
        gpioSetMode(cs_pin_, PI_OUTPUT);
        deassertCS();
    }

    void RasPiHardwareSPI::beginTransaction(bool configuration_mode)
    {

        if (configuration_mode)
        {
            spi_handle_ = spiOpen(spi_channel_, 1000UL * 1000, 0); // mode 0
            if (spi_handle_ < 0)
            {
                throw std::runtime_error("Failed to open SPI channel");
            }
        }
        else
        {
            spi_handle_ = spiOpen(spi_channel_, spi_hz_, 0); // mode 0
            if (spi_handle_ < 0)
            {
                throw std::runtime_error("Failed to open SPI channel");
            }
        }
        assertCS();
    }

    void RasPiHardwareSPI::endTransaction()
    {

        spiClose(spi_handle_);
        deassertCS();
    }

    int RasPiHardwareSPI::transfer(const uint8_t *buffer, int length)
    {
        int result = spiXfer(spi_handle_, (char *)buffer, (char *)buffer, length);
        if (result < 0)
        {
            throw std::runtime_error("SPI transfer failed");
        }
        return result;
    }

    int RasPiHardwareSPI::transfer16(const uint16_t data)
    {
        int result = spiXfer(spi_handle_, (char *)&data, (char *)&data, 2);
        if (result < 0)
        {
            throw std::runtime_error("SPI 16-bit transfer failed");
        }
        return result;
    }

    void RasPiHardwareSPI::initCS()
    {
        gpioSetMode(cs_pin_, PI_OUTPUT);
        deassertCS();
    }

    inline void RasPiHardwareSPI::assertCS()
    {
        gpioWrite(cs_pin_, 0);
    }

    inline void RasPiHardwareSPI::deassertCS()
    {
        gpioWrite(cs_pin_, 1);
    }

    void RasPiHardwareSPI::setSPIClock(const uint32_t spiClock)
    {
        spi_hz_ = spiClock;
    }

    RasPiHardwareSPI::~RasPiHardwareSPI()
    {
        if (spi_handle_ >= 0)
        {
            spiClose(spi_handle_);
        }
        gpioTerminate();
    }

}