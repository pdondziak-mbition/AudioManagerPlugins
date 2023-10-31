
#include "CAmCommandReceiverPipewire.h"

namespace am
{

CAmCommandReceiverPipewire::CAmCommandReceiverPipewire()
{
    pw_init(NULL, NULL);
}

} // namespace am
