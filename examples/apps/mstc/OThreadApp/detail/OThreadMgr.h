#pragma once

#include <memory>
#include <mutex>
#include "detail/IOThreadMgr.h"
#include "detail/OThread.h"

namespace mstc
{
namespace ot
{
class KApiInstance;
}
}

class OThreadMgr : public IOThreadMgr
{
public:
//  using intf_menu_t = OThread::intf_menu_t;
    explicit OThreadMgr(mstc::ot::KApiInstance& kApiInst);

    void init();
    void unInit();

    void DeviceChangedCallback(bool added, const GUID *deviceGuid);

    void ThreadStateChangeCallback(uint32_t flags);

    void switchToOThreadMenu();

    void execute(const std::string& line);

    void refresh();

    void SetListener(listener_csptr&& listener);

protected:
    using mutex_t       = std::mutex;
    using othread_t     = OThread;
    using othread_uptr  = std::unique_ptr<othread_t>;
    using othread_csptr = othread_uptr;
    using lock_t        = std::lock_guard<mutex_t>;
    using menu_csptr    = Menu::menu_csptr;

    void BuildInterfaceList();

    mstc::ot::KApiInstance&             kApiInst_;
    otDeviceAvailabilityChangedCallback devChangeCallback_;

    mutable mutex_t         mtxOThrerad_;
    othread_csptr           oThread_;
    menu_csptr              menu_;
    listener_sptr           listener_;
};