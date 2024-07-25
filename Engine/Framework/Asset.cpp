#include "Asset.h"

#include <cassert>
#include <thread>

void Asset::Load(const std::filesystem::path& path, const std::string& name) {
    assert(!path.empty());

    path_ = path;
    // 名前が指定されていない場合はパスの拡張子を除いた名前を使用
    name_ = name.empty() ? path.stem().string() : name;

    // 非同期読み込み
    std::thread thread([this]() {
        state_ = State::Loading;
        InternalLoad();
        state_ = State::Loaded;
        });
    thread.detach();
}