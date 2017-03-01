//
// ThreadInterface.xaml.h
// Declaration of the Interface class
//

#pragma once

#include "ThreadInterface.g.h"
#include "Types.h"
#include "IInterfaceUIElements.h"
#include "OtDevice.h"

namespace OpenThreadTalk
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ThreadInterface sealed
    {
    public:
        ThreadInterface();

        static UIElement^ Create(Guid interfaceGuid,
            IInterfaceUIElements^ interfaceUIElements);

        void Init(Guid interfaceGuid,
            IInterfaceUIElements^ interfaceUIElements);

    private:
        using Device = ot::Device;

        Guid    interfaceGuid_;
        Device* pDevice_ = nullptr;
    };
}
