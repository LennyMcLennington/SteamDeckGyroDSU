#include "hiddev/hiddevfinder.h"
#include "shell/shell.h"
#include <sstream>
#include <iomanip>
    
using namespace kmicki::shell;

namespace kmicki::hiddev
{
    // Find N of the HID device matching provided vendor ID and product ID.
    // N being the number in path: /dev/usb/hiddevN
    int FindHidDevNo(uint16_t vid, uint16_t pid)
    {
        std::string output;
        int bus,dev,checkBus,checkDev;
        int testLen;

        // Get bus number and dev number from vendor ID and product ID
        std::ostringstream cmd;
        cmd << "lsusb | grep -i '";
        cmd << std::setw(4) << std::setfill('0') << std::setbase(16) << vid << ":" ;
        cmd << std::setw(4) << std::setfill('0') << std::setbase(16) << pid;
        cmd << "' | sed -e \"s/.*Bus \\([0-9]\\+\\) Device \\([0-9]\\+\\).*/\\1 \\2/g\"";
        
        if(ExecuteCommand(cmd.str(),output) || (testLen = output.length()) != 8)
            return -1;

        std::istringstream str(output);
        str >> bus >> dev;
        if (bus == 0 || dev == 0)
            return -1;

        // loop through info of /dev/usb/hiddev0 to /dev/usb/hiddev10 to find the one matching
        // bus and dev number.
        for (int i = 0; i < 10; i++)    
        {
            std::ostringstream ostr;
            ostr << "udevadm info --query=all /dev/usb/hiddev" << i << " | grep 'P:' | sed -e \"s/.*usb\\([0-9]\\+\\).*\\.\\([0-9]\\+\\).*/\\1 \\2/g\"";
            std::string cmd = ostr.str();
            if(ExecuteCommand(cmd,output) || output.length() > 8 ||  output.length() < 4)
                continue; // wrong output length - probably device doesn't exist
            std::istringstream str(output);
            str >> checkBus >> checkDev;
            if (bus == checkBus || dev == checkDev)
                return i; // matching bus and dev
        }
        return -1;
    }

}