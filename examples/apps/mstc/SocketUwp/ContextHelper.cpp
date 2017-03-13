#include "pch.h"
#include "ContextHelper.h"

using namespace SocketUwp;

ContextHelper::ContextHelper(
    IAsyncThreadPage^ page) :
    page_{ std::move(page) }
{
}

String^
ContextHelper::ReadString(
    DataReader^ dataReader)
{
    auto strLen = static_cast<unsigned int>(dataReader->UnconsumedBufferLength);
    if (!strLen) { return nullptr; }

    return dataReader->ReadString(strLen);
}