#include "detail/pch.h"
#include <utility>
#include <tcOtApi/OtApiInstance.h>
#include "detail/OThreadMgr.h"

OThreadMgr::OThreadMgr(
    mstc::ot::KApiInstance& kApiInst) :
    kApiInst_{kApiInst},
    devChangeCallback_{[](bool added, const GUID* deviceGuid, void* context)
    {
        static_cast<OThreadMgr*>(context)->DeviceChangedCallback(added, deviceGuid);
    }}
{
}

void
OThreadMgr::init()
{
    kApiInst_.SetDeviceAvailabilityChangedCallback(devChangeCallback_, this);

    BuildInterfaceList();
}

void
OThreadMgr::unInit()
{
    kApiInst_.RemoveDeviceAvailabilityChangedCallback(devChangeCallback_, this);
    
    lock_t lck{mtxOThrerad_};
    if (oThread_)
    {
        oThread_->RemoveStateChangeCallback();
    }
}

void
OThreadMgr::DeviceChangedCallback(
    bool        added,
    const GUID *deviceGuid)
{
    BuildInterfaceList();
}

void
OThreadMgr::ThreadStateChangeCallback(
    uint32_t flags)
{
    if ((flags & OT_NET_ROLE) == 0) { return; }

    BuildInterfaceList();
}

/*
void
Options::BuildInterfaceList()
{
    auto oThread = std::make_unique<othread_t>(kApiInst_,
        [](uint32_t flags, void* context)
    {
        static_cast<Options*>(context)->ThreadStateChangeCallback(flags);
    }, this);
    oThread->BuildInterfaceList();

    {
        lock_t lck{mtxOThrerad_};
        oThread_ = std::move(oThread);
        menu_ = oThread_->Menu();
        menu_->show();
    }
}*/

void
OThreadMgr::BuildInterfaceList()
{
    {
        lock_t lck{mtxOThrerad_};
        if (!oThread_)
        {
            oThread_ = std::make_unique<othread_t>(kApiInst_,
                [](uint32_t flags, void* context)
            {
                static_cast<OThreadMgr*>(context)->ThreadStateChangeCallback(flags);
            }, this);
        }
        oThread_->BuildInterfaceList();
        menu_ = oThread_->Menu();
        menu_->show();
    }
}

void
OThreadMgr::switchToOThreadMenu()
{
    lock_t lck{mtxOThrerad_};
    menu_ = oThread_->Menu();
    menu_->show();
}

void
OThreadMgr::execute(
    const std::string& line)
{
    lock_t lck{mtxOThrerad_};
    menu_ = menu_->input(line);
    menu_->show();
}

void
OThreadMgr::refresh()
{
    lock_t lck{mtxOThrerad_};
    menu_->show();
}

void
OThreadMgr::SetListener(
    listener_csptr&& listener)
{
    listener_ = std::move(listener);
}