#pragma once
#include <atomic>
#include <vector>
#include <Poco/AutoPtr.h>
#include <Poco/Channel.h>
#include "ExtendedLogChannel.h"
#include <Common/SensitiveDataMasker.h>
#include <Interpreters/TextLog.h>


namespace DB
{
/// Works as Poco::SplitterChannel, but performs additional work:
///  passes logs to Client via TCP interface
///  tries to use extended logging interface of child for more comprehensive logging
class OwnSplitChannel : public Poco::Channel
{
public:
    /// Makes an extended message from msg and passes it to the client logs queue and child (if possible)
    void log(const Poco::Message & msg) override;

    void setMasker(DB::SensitiveDataMasker * _sensitive_data_masker);

    /// Adds a child channel
    void addChannel(Poco::AutoPtr<Poco::Channel> channel);

    void addTextLog(std::shared_ptr<DB::TextLog> log);

private:
    void logSplit(const Poco::Message & msg);

    using ChannelPtr = Poco::AutoPtr<Poco::Channel>;
    /// Handler and its pointer casted to extended interface
    using ExtendedChannelPtrPair = std::pair<ChannelPtr, ExtendedLogChannel *>;
    std::vector<ExtendedChannelPtrPair> channels;
    std::atomic<DB::SensitiveDataMasker *> sensitive_data_masker = nullptr; // global context owns that object, pointer should be reset before context destroying.

    std::mutex text_log_mutex;
    std::weak_ptr<DB::TextLog> text_log;
};

}
