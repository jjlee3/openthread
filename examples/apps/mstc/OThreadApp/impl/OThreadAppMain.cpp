#include "detail/pch.h"
#include <iostream>
#include <base/Scope.h>
#include <network/Wsa.h>
#include <tcOtApi/OtApiInstance.h>
#include "detail/Log.h"
#include "detail/Options.h"
#include "detail/OThreadMgr.h"
#include "detail/StdOutput.h"
#include "detail/Client.h"

Options      g_options;
IOThreadMgr* g_ioThreadMgr = nullptr;

void
MainMode(
    OThreadMgr& oThreadMgr)
{
    std::string line;

    for (char ch, prvch; std::cin.get(ch);)
    {
        try
        {
            prvch = ch;

            switch (ch)
            {
            case '\b':
                if (!line.empty())
                {
                    line.resize(line.size() - 1);
                }
                break;
            case 27: // ESC
                if (!line.empty())
                {
                    auto exit =
                        mstc::base::scope([&line]() { line.clear(); });
                    oThreadMgr.switchToOThreadMenu();
                }
                break;
            case 13: // CR
                if (!line.empty())
                {
                    auto exit =
                        mstc::base::scope([&line]() { line.clear(); });
                    oThreadMgr.execute(line);
                }
                break;
            case 10: // LF
                if (!line.empty())
                {
                    auto exit =
                        mstc::base::scope([&line]() { line.clear(); });
                    oThreadMgr.execute(line);
                }
                break;
            default:
                if (ch < 0)
                {
                    line += static_cast<char>(ch);
                }
                else if (ch < ' ')
                {
                }
                else if (ch < 127)
                {
                    line += static_cast<char>(ch);
                }
                else if (ch == 127)
                {
                }
                else
                {
                }
                break;
            }
        }
        catch (Menu::Stay&)
        {
        }
        catch (Menu::Refresh&)
        {
            oThreadMgr.refresh();
        }
        catch (Menu::MainMenu&)
        {
            oThreadMgr.switchToOThreadMenu();
        }
        catch (Menu::Failure& e)
        {
            if (e.whats().empty()) { continue; }

            g_out([&e](std::ostream& os)
            {
                os << e.whats() << std::endl;
            });
        }
    }
}

void
SwitchToClientMode()
{
    g_log([](std::ostream& os)
    {
        os << "Client Mode started" << std::endl;
    });

    auto exit = mstc::base::scope([]
    {
        g_log([](std::ostream& os)
        {
            os << "Client Mode ended" << std::endl;
        });
    });

    Client client;

    client.start(g_options);
    client.wait();
}

int main(int argc, char** argv)
{
    g_log([](std::ostream& os)
    {
        os << "OThread Application started" << std::endl;
    });

    auto exit = mstc::base::scope([]
    {
        g_log([](std::ostream& os)
        {
            os << "OThread Application ended" << std::endl;
        });
    });

    try
    {
        mstc::network::Wsa     wsa;
        mstc::ot::KApiInstance kApiInst;
        OThreadMgr             oThreadMgr{kApiInst};
        std::string            line;

        g_options.parse(argc, argv);
        oThreadMgr.init();
        g_ioThreadMgr = &oThreadMgr;

        try
        {
            MainMode(oThreadMgr);
        }
        catch (Menu::ClientMode&)
        {
            oThreadMgr.unInit();
            SwitchToClientMode();
        }
    }
    catch (Menu::Quit& e)
    {
        g_log([&e](std::ostream& os)
        {
            os << "Quit" << std::endl;
        });
    }
    catch (mstc::base::Exception& e)
    {
        g_log([&e](std::ostream& os)
        {
            os << e.diagnose() << std::endl;
        });
        return 1;
    }

    return 0;
}
